#ifndef SOCKET_READER_H
#define SOCKET_READER_H

#include <iostream>
#include <string>
#include <winsock2.h>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

void startSocketReader();
bool stopSocketReader();
bool connectToServer(const char* serverIP, int port);
std::string readLine();
std::string readLineNonBlocking();
void runInNewThread();
DWORD WINAPI socketReaderThreadLoop(LPVOID lpParam);

#endif