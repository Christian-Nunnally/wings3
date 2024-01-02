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
#include <cstring>
#include <unistd.h>
#include <sstream>
#include "../common/Control/remoteControl.h"

bool isSocketInputReady();
RemoteControlCommand getSocketInput();

#endif