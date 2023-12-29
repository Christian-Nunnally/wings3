#include "testMicrophone.h"
#include "../common/settings.h"
#include "../common/IO/tracing.h"
#include <iostream>
#include <string>
#include <winsock2.h>
//#include <thread>
#include <queue>
#include <mutex>
#include <chrono>
#include <windows.h>

FILE *testMicrophonePipe;
int lastTestMicrophoneRMS;
char testMicrophoneBuffer[128];
bool shouldTestMicrophoneLoopContinue = true;

std::queue<int> testMicrophoneRMSQueue;
DWORD WINAPI testMicrophoneThreadLoop(LPVOID lpParam);

extern "C" FILE *popen(const char *command, const char *mode);
extern "C" void pclose(FILE *pipe); 

void setupTestMicrophone()
{
    DWORD threadId;
    HANDLE hThread;
    hThread = CreateThread(NULL, 0, testMicrophoneThreadLoop, NULL, 0, &threadId);
    if (hThread == NULL) {
        std::cerr << "Thread creation failed." << std::endl;
    }
}

DWORD WINAPI testMicrophoneThreadLoop(LPVOID lpParam)
{
    testMicrophonePipe = popen("poetry run python src\\windows\\audioDeviceInput.py", "r");
    if (!testMicrophonePipe) {
        fprintf(stderr, "Error: Failed to open pipe.\n");
    }
    while (fgets(testMicrophoneBuffer, sizeof(testMicrophoneBuffer), testMicrophonePipe) != NULL && shouldTestMicrophoneLoopContinue) {
        testMicrophoneRMSQueue.push(atoi(testMicrophoneBuffer));
    }
    return 0;
}

bool processTestMicrophoneAudio()
{
    bool result = false;
    if (!testMicrophoneRMSQueue.empty())
    {
        lastTestMicrophoneRMS = testMicrophoneRMSQueue.front();
        testMicrophoneRMSQueue.pop(); 
        result = true;
    }
    return result;
}

int getLastTestMicrophoneRMS()
{
    return lastTestMicrophoneRMS;
}

void teardownTestMicrophone()
{
    shouldTestMicrophoneLoopContinue = false;
    pclose(testMicrophonePipe);
}