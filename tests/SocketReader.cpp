#include "../tests/SocketReader.h"

#include <iostream>
#include <string>
#include <winsock2.h>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

std::queue<std::string> fifoQueue;
SOCKET clientSocket;

void startSocketReader() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
    }
}

bool stopSocketReader() {
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
    }
    WSACleanup();
}

bool connectToServer(const char* serverIP, int port) {
    struct sockaddr_in serverAddr;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed\n";
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }

    return true;
}

std::string readLine() {
    std::string data;
    char ch;
    int bytesReceived;
    while (true) {
        bytesReceived = recv(clientSocket, &ch, 1, 0);
        if (bytesReceived <= 0 || ch == '\n') {
            break;
        }
        if (ch != '\r') {
            data += ch;
        }
    }
    return data;
}

std::string readLineNonBlocking() {
    std::string line = "";
    if (!fifoQueue.empty())
    {
        line = fifoQueue.front(); 
        fifoQueue.pop(); 
    }
    return line;
}

void runInNewThread() {
    DWORD threadId;
    HANDLE hThread;

    hThread = CreateThread(NULL, 0, socketReaderThreadLoop, NULL, 0, &threadId);
    if (hThread == NULL) {
        std::cerr << "Thread creation failed." << std::endl;
    }
}

DWORD WINAPI socketReaderThreadLoop(LPVOID lpParam)
{
    startSocketReader();
    if (connectToServer("127.0.0.1", 3232)) 
    {
        std::string line;
        while (true) {
            line = readLine();
            if (!line.empty())
            { 
                fifoQueue.push(line);
            } 
        }
    }
    stopSocketReader();

    return 0;
}