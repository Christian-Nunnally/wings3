#include <PeakDetection.h> 
#include <SingleEMAFilterLib.h>
#include <PDM.h>
#include "microphone.h"

#define AUDIO_SAMPLE_BATCH_SIZE     256
#define MICROPHONE_SAMPLE_FREQUENCY 16000
#define MICROPHONE_CHANNELS         1
#define EMA_FILTER_ALPHA            .25
#define PEAK_DETECTOR_LAG           48
#define PEAK_DETECTOR_THRESHOLD     3
#define PEAK_DETECTOR_INFLUENCE     0.6
#define MAX_RMS_DECAY_RATE          0.002
#define MUSIC_DETECTION_COUNTDOWN 3000
#define MUSIC_DETECTION_RMS_THRESHOLD 800

short audioSampleBuffer[512];
volatile int numberOfAudioSamplesRead;
SingleEMAFilter<int> singleEMAFilter(EMA_FILTER_ALPHA);
PeakDetection peakDetector;

int samplesRead;
float sumOfSquaredSample;
double currentRootMeanSquare = 1.0;
double currentPeakRootMeanSquare = 1.0;
double currentFilteredAudioLevel;
int currentPeakDetectorValue;
bool isMusicDetectedInternal = false;
int musicDetectionCountdown = MUSIC_DETECTION_COUNTDOWN;

inline void monitorAudioLevelsToToggleMusicDetection();
inline void incrementMusicDetectionToggle();

bool setupMicrophone()
{
    PDM.onReceive(onAudioDataReceived);
    peakDetector.begin(PEAK_DETECTOR_LAG, PEAK_DETECTOR_THRESHOLD, PEAK_DETECTOR_INFLUENCE); 
    return PDM.begin(MICROPHONE_CHANNELS, MICROPHONE_SAMPLE_FREQUENCY);
}

double getAudioIntensityRatio()
{
    return currentRootMeanSquare / currentPeakRootMeanSquare;
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
    return true;
    return isMusicDetectedInternal;
}

void onAudioDataReceived() 
{
  int bytesAvailable = PDM.available();
  PDM.read(audioSampleBuffer, bytesAvailable);
  numberOfAudioSamplesRead = bytesAvailable / 2;
}

inline void resetSampleBatch()
{
    samplesRead = 0;
    sumOfSquaredSample = 0;
}

inline void applyFiltering()
{
    currentRootMeanSquare = sqrt(sumOfSquaredSample / samplesRead);
    singleEMAFilter.AddValue(currentRootMeanSquare);
    peakDetector.add(singleEMAFilter.GetLowPass());
    currentPeakDetectorValue = peakDetector.getPeak();
    currentFilteredAudioLevel = peakDetector.getFilt();
}

inline void setMaxRootMeanSquare()
{
    if (currentRootMeanSquare > currentPeakRootMeanSquare) currentPeakRootMeanSquare = currentRootMeanSquare;
}

inline void decayMaxRootMeanSquare()
{
    if (currentPeakRootMeanSquare > 1) currentPeakRootMeanSquare = currentPeakRootMeanSquare - (currentPeakRootMeanSquare * MAX_RMS_DECAY_RATE);
}

inline void processSampleBatch()
{
    applyFiltering();
    setMaxRootMeanSquare();
    decayMaxRootMeanSquare();
    resetSampleBatch();
    monitorAudioLevelsToToggleMusicDetection();
}

void processAudioStream()
{
    if (numberOfAudioSamplesRead == 0) return;
    for (int i = 0; i < numberOfAudioSamplesRead; i++) {
        sumOfSquaredSample += audioSampleBuffer[i] * audioSampleBuffer[i];
        samplesRead += 1;
        if (samplesRead == AUDIO_SAMPLE_BATCH_SIZE) processSampleBatch();
    }
    numberOfAudioSamplesRead = 0;
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
}

inline void incrementMusicDetectionToggle()
{
    musicDetectionCountdown -= 2;
    if (musicDetectionCountdown <= 2)
    {
        isMusicDetectedInternal = !isMusicDetectedInternal;
        musicDetectionCountdown = MUSIC_DETECTION_COUNTDOWN;
    }
}