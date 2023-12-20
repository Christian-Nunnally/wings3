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

void processFakeRemoteInput();

#endif