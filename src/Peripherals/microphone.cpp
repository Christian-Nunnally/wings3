#ifdef RP2040
#include <ADCInput.h>
#include <PeakDetection.h> 
#include <SingleEMAFilterLib.h>
#include <PDM.h>
#else
#include <cmath>
#endif
#include "../Peripherals/microphone.h"
#include "../IO/leds.h"
#include "../IO/serial.h"
#include "../Utility/time.h"
#include "../Utility/fastmath.h"
#include "../settings.h"

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
#ifdef RP2040
SingleEMAFilter<int> singleEMAFilter(EMA_FILTER_ALPHA);
PeakDetection peakDetector;
ADCInput mic(26, 28);
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

inline void monitorAudioLevelsToToggleMusicDetection();
inline void incrementMusicDetectionToggle();

bool micDataReady = false;
void onMicDataReady(void) {micDataReady = true;}

bool setupMicrophone()
{
    #ifdef RP2040
    mic.onReceive(onMicDataReady);
    mic.begin(MICROPHONE_SAMPLE_FREQUENCY);
    peakDetector.begin(PEAK_DETECTOR_LAG, PEAK_DETECTOR_THRESHOLD, PEAK_DETECTOR_INFLUENCE); 
    bool result = PDM.begin(MICROPHONE_CHANNELS, MICROPHONE_SAMPLE_FREQUENCY);
    #else
    bool result = true;
    #endif
    if (result) D_serialWriteNewLine("Microphone Initalized.");
    return result;
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

inline void resetSampleBatch()
{
    samplesRead = 0;
    sumOfSquaredSample = 0;
}

inline void applyFiltering()
{
    currentRootMeanSquare = ((sqrt(sumOfSquaredSample / samplesRead) - 830) * .7) + (currentRootMeanSquare * .3);
    #ifdef RP2040
    singleEMAFilter.AddValue(currentRootMeanSquare);
    peakDetector.add(singleEMAFilter.GetLowPass());
    int newPeakDetectorValue = peakDetector.getPeak();
    D_serialWrite(">pk:");
    D_serialWriteNewLine(newPeakDetectorValue);
    currentPeakDetectorValue = newPeakDetectorValue;
    currentFilteredAudioLevel = peakDetector.getFilt();
    D_serialWrite(">rms:");
    D_serialWriteNewLine(currentRootMeanSquare);
    D_serialWrite(">intensity:");
    D_serialWriteNewLine(getAudioIntensityRatio());
    D_serialWrite(">filt:");
    D_serialWriteNewLine(peakDetector.getFilt());
    D_serialWrite(">max:");
    D_serialWriteNewLine(currentPeakRootMeanSquare);
    D_serialWrite(">min:");
    D_serialWriteNewLine(currentMinRootMeanSquare);
    #endif
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
    //monitorAudioLevelsToToggleMusicDetection();
}

unsigned long lastPotCheckTime;
short lastPotValue;
void processAudioStream()
{
  if (micDataReady) 
  {
    micDataReady = false;
    #ifdef RP2040
    int samplesAvailable = mic.available();
    #else
    int samplesAvailable = 0; 
    #endif
    for (int i = 0; i < samplesAvailable; i++) {
        #ifdef RP2040
        short audioSample = mic.read() - 831;
        short potValue = mic.read();
        #else
        short audioSample = 0;
        short potValue = 0;
        #endif
        if (getTime() - lastPotCheckTime > 1000)
        {
            lastPotCheckTime = getTime();
            D_serialWrite(">potValue:");
            D_serialWriteNewLine(potValue);

            if (D_abs(lastPotValue - potValue) > 100)
            {
                lastPotValue = potValue;
                uint8_t brightness;
                if (potValue > 2048) 
                {
                    isMusicDetectedInternal = true;
                    brightness = potValue - 2048 >> 3;
                }
                else 
                {
                    isMusicDetectedInternal = false;
                    brightness = (2047 - potValue) >> 3;
                }
                setGlobalLedBrightness(brightness);
                D_serialWrite(">brightness:");
                D_serialWriteNewLine(brightness);
                D_serialWrite(">isMusic:");
                D_serialWriteNewLine(isMusicDetectedInternal);
            }
        }

        sumOfSquaredSample += audioSample * audioSample;
        samplesRead += 1;
        if (samplesRead == AUDIO_SAMPLE_BATCH_SIZE) processSampleBatch();
        }
    }
}

inline void monitorAudioLevelsToToggleMusicDetection()
{
    if (isMusicDetectedInternal)
    {
        if (getCurrentPeakRootMeanSquare() < MUSIC_DETECTION_RMS_THRESHOLD) incrementMusicDetectionToggle();
    }
    else 
    {
        if (getCurrentPeakRootMeanSquare() > MUSIC_DETECTION_RMS_THRESHOLD) incrementMusicDetectionToggle();
    }
    if (musicDetectionCountdown < MUSIC_DETECTION_COUNTDOWN) musicDetectionCountdown++;
    D_serialWrite(">isMusic:");
    D_serialWriteNewLine(isMusicDetectedInternal);
}

inline void incrementMusicDetectionToggle()
{
    musicDetectionCountdown -= 5;
    if (musicDetectionCountdown <= 5)
    {
        isMusicDetectedInternal = !isMusicDetectedInternal;
        musicDetectionCountdown = MUSIC_DETECTION_COUNTDOWN;
    }
}