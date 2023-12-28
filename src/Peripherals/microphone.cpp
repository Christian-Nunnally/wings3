#ifdef RP2040
#include <ADCInput.h>
#include <PDM.h>
#else
#include <cmath>
#include "../../tests/testMicrophone.h"
#endif
#include "../settings.h"
#include "../PeakDetection/peakDetection.h" 
#include "../Peripherals/microphone.h"
#include "../IO/leds.h"
#include "../IO/serial.h"
#include "../IO/tracing.h"
#include "../Utility/time.h"
#include "../Utility/fastmath.h"

// TODO: Move code for reading pot.

#define AUDIO_SAMPLE_BATCH_SIZE     128
#define MICROPHONE_SAMPLE_FREQUENCY 8000
#define MICROPHONE_CHANNELS         1
#define EMA_FILTER_ALPHA            .25
#define PEAK_DETECTOR_LAG           20
#define PEAK_DETECTOR_THRESHOLD     3.05
#define PEAK_DETECTOR_INFLUENCE     0.92
#define MAX_RMS_DECAY_RATE          0.00001
#define MIN_RMS_DECAY_RATE          0.00001
#define RMS_DECAY_FREQUENCY_MILLISECONDS 30
#define MUSIC_DETECTION_COUNTDOWN   3000
#define MUSIC_DETECTION_RMS_THRESHOLD 418
#define AUDIO_INTENSITY_RATIO_THRESHOLD_BUMP 3

short audioSampleBuffer[512];
volatile int numberOfAudioSamplesRead;
PeakDetection peakDetector;
#ifdef RP2040
#define MICROPHONE_PIN 26
ADCInput microphoneADCInput(MICROPHONE_PIN);
#endif

int samplesRead;
float sumOfSquaredSample;
double currentRootMeanSquare = 1.0;
double currentPeakRootMeanSquare = 1.0;
double currentMinRootMeanSquare = 0.0;
double currentFilteredAudioLevel;
double currentMinAccelRate = 0.0;
double currentMaxAccelRate = 0.0;
int currentPeakDetectorValue;
bool isMusicDetectedInternal = false;
int musicDetectionCountdown = MUSIC_DETECTION_COUNTDOWN;
unsigned long lastRmsDecayTime;

// EMA
double ema = 10.0;

bool micDataReady = false;
void onMicDataReady(void) {micDataReady = true;}
bool setupMicrophone()
{
    bool result = true;
    #ifdef RP2040
    microphoneADCInput.onReceive(onMicDataReady);
    microphoneADCInput.begin(MICROPHONE_SAMPLE_FREQUENCY);
    bool result = PDM.begin(MICROPHONE_CHANNELS, MICROPHONE_SAMPLE_FREQUENCY);
    #else
    setupTestMicrophone();
    #endif
    peakDetector.begin(PEAK_DETECTOR_LAG, PEAK_DETECTOR_THRESHOLD, PEAK_DETECTOR_INFLUENCE); 
    if (result) D_serialWriteNewLine("Microphone Initalized.");
    return result;
}

void updateExponentialMovingAverage(double newValue) {
    ema = EMA_FILTER_ALPHA * newValue + (1 - EMA_FILTER_ALPHA) * ema;
}

double getAudioIntensityRatio()
{
    if (isMusicDetected()) 
    {
        if (((currentPeakRootMeanSquare - currentMinRootMeanSquare) - (currentRootMeanSquare - currentMinRootMeanSquare)) < AUDIO_INTENSITY_RATIO_THRESHOLD_BUMP)
            return (currentRootMeanSquare - currentMinRootMeanSquare) / (currentPeakRootMeanSquare - currentMinRootMeanSquare + AUDIO_INTENSITY_RATIO_THRESHOLD_BUMP);
        else return (currentRootMeanSquare - currentMinRootMeanSquare) / (currentPeakRootMeanSquare - currentMinRootMeanSquare);
    }
    return .5;
}

int getCurrentPeakDetectorValue()
{
    return currentPeakDetectorValue;
}

int getCurrentPeakRootMeanSquare()
{
    return currentPeakRootMeanSquare;
}

int getCurrentFilteredAudioLevel()
{
    return currentFilteredAudioLevel;
}

bool isMusicDetected()
{
    return isMusicDetectedInternal;
}

void enableMusicDetection()
{
    isMusicDetectedInternal = true;
}

void disableMusicDetection()
{
    isMusicDetectedInternal = false;
}

inline void resetSampleBatch()
{
    samplesRead = 0;
    sumOfSquaredSample = 0;
}

inline void applyFiltering()
{
    #ifdef RP2040
    currentRootMeanSquare = ((sqrt(sumOfSquaredSample / samplesRead) - 830) * .7) + (currentRootMeanSquare * .3);
    #else
    currentRootMeanSquare = getLastTestMicrophoneRMS();
    D_emitIntegerMetric("RMS", currentRootMeanSquare);
    #endif
    updateExponentialMovingAverage(currentRootMeanSquare);
    peakDetector.add(ema);
    D_emitFloatMetric("EMA", ema);
    currentPeakDetectorValue = peakDetector.getPeak();
    currentFilteredAudioLevel = peakDetector.getFilt();
}

double exponentialMovingAverage(double new_data, double previous_ema, double alpha) {
    return alpha * new_data + (1 - alpha) * previous_ema;
}

inline void setMaxRootMeanSquare()
{
    if (currentRootMeanSquare > currentPeakRootMeanSquare) 
    {
        currentPeakRootMeanSquare = currentRootMeanSquare;
        currentMaxAccelRate = 0;
    }
    if (currentRootMeanSquare < currentMinRootMeanSquare) 
    {
        currentMinRootMeanSquare = currentRootMeanSquare;
        currentMinAccelRate = 0;
    }
}

inline void decayMaxRootMeanSquare()
{
    if (getTime() - lastRmsDecayTime > RMS_DECAY_FREQUENCY_MILLISECONDS)
    {
        lastRmsDecayTime = getTime();
        if (currentPeakRootMeanSquare > currentMinRootMeanSquare) currentPeakRootMeanSquare = currentPeakRootMeanSquare - (currentPeakRootMeanSquare * currentMaxAccelRate);
        if (currentMinRootMeanSquare < currentPeakRootMeanSquare) currentMinRootMeanSquare = currentMinRootMeanSquare + (currentMinRootMeanSquare * currentMinAccelRate);
        if (currentMinRootMeanSquare < 10) currentMinRootMeanSquare = 10;
        currentMaxAccelRate += MAX_RMS_DECAY_RATE;
        currentMinAccelRate += MIN_RMS_DECAY_RATE;
    }
}

inline void processSampleBatch()
{
    applyFiltering();
    setMaxRootMeanSquare();
    decayMaxRootMeanSquare();
    resetSampleBatch();
}

void processAudioStream()
{
    #ifdef RP2040
    if (micDataReady) 
    {
        micDataReady = false;
        int samplesAvailable = microphoneADCInput.available();
        for (int i = 0; i < samplesAvailable; i++) {
            short audioSample = microphoneADCInput.read() - 831;

            sumOfSquaredSample += audioSample * audioSample;
            samplesRead += 1;
            if (samplesRead == AUDIO_SAMPLE_BATCH_SIZE) processSampleBatch();
        }
    }
    #else
    while (processTestMicrophoneAudio())
    {
        processSampleBatch();
    }
    #endif
}