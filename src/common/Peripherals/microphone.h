#ifndef MICROPHONE_H
#define MICROPHONE_H

bool setupMicrophone();
void processAudioStream();
void onAudioDataReceived();
float getAudioIntensityRatio();
int getCurrentPeakDetectorValue();
int getCurrentPeakRootMeanSquare();
int getCurrentFilteredAudioLevel();
bool isMusicDetected();
void enableMusicDetection();
void disableMusicDetection();

#endif