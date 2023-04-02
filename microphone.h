#ifndef MICROPHONE_H
#define MICROPHONE_H

bool setupMicrophone();
void processAudioStream();
void onAudioDataReceived();
double getAudioIntensityRatio();
int getCurrentPeakDetectorValue();
int getCurrentPeakRootMeanSquare();

#endif