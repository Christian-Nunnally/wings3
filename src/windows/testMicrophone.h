#ifndef TEST_MICROPHONE_H
#define TEST_MICROPHONE_H

#include <stdio.h>
#include <stdlib.h>

void setupTestMicrophone();
int getLastTestMicrophoneRMS();
bool processTestMicrophoneAudio();
void teardownTestMicrophone();

#endif