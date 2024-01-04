#include "../common/settings.h"
#include "../common/IO/tracing.h"
#include "socketRemoteControl.h"
#include "../common/Control/remoteCommandInterpreter.h"

std::queue<std::string> fifoQueue;
SOCKET clientSocket;
bool hasSocketRemoteBeenInitialized = false;
std::string lastReadLineFromSocket;

void startSocketReader();
bool stopSocketReader();
bool connectToServer(const char* serverIP, int port);
std::string readLine();
std::string readLineNonBlocking();
DWORD WINAPI socketReaderThreadLoop(LPVOID lpParam);

void startSocketReader() 
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        std::cerr << "WSAStartup failed\n";
    }
}

bool stopSocketReader() 
{
    if (clientSocket != INVALID_SOCKET) 
    {
        closesocket(clientSocket);
    }
    WSACleanup();
    return true;
}

bool connectToServer(const char* serverIP, int port) 
{
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

std::string readLine() 
{
    std::string data;
    char ch;
    int bytesReceived;
    while (true) 
    {
        bytesReceived = recv(clientSocket, &ch, 1, 0);
        if (bytesReceived <= 0 || ch == '\n') 
        {
            break;
        }
        if (ch != '\r') 
        {
            data += ch;
        }
    }
    return data;
}

std::string readLineNonBlocking() 
{
    std::string line = "";
    if (!fifoQueue.empty())
    {
        line = fifoQueue.front(); 
        fifoQueue.pop(); 
    }
    return line;
}

void runInNewThread() 
{
    DWORD threadId;
    HANDLE hThread;

    hThread = CreateThread(NULL, 0, socketReaderThreadLoop, NULL, 0, &threadId);
    if (hThread == NULL) {
        std::cerr << "Thread creation failed." << std::endl;
    }
}

bool isSocketInputReady() 
{
    if (!hasSocketRemoteBeenInitialized)
    {
        hasSocketRemoteBeenInitialized = true;
        runInNewThread();
        return false;
    }

    lastReadLineFromSocket = readLineNonBlocking();
    if (lastReadLineFromSocket.empty()) return false;
    return true;
}

RemoteControlCommand getSocketInput() 
{
    std::stringstream ss(lastReadLineFromSocket);

    std::string token;
    uint32_t commandCode = 0;
    uint8_t operationCode = 0;
    uint8_t operationType = 0;
    int16_t operationValue = 0;

    D_emitMetric(METRIC_NAME_ID_MISC_METRIC, lastReadLineFromSocket);
    std::getline(ss, token, ',');
    if (!token.empty()) 
    {
        commandCode = static_cast<uint32_t>(std::stoi(token));
        operationCode = commandCode >> 24 & 0xFF;
        operationType = commandCode >> 16 & 0xFF;
        operationValue = commandCode >> 0 & 0xFFFF;
    }
    D_emitMetric(METRIC_NAME_ID_MISC_METRIC, operationCode);

    // std::getline(ss, token, ',');
    // if (!token.empty()) 
    // {
    //     operationType = static_cast<uint8_t>(std::stoi(token));
    // }
    
    // std::getline(ss, token, ',');
    // if (!token.empty()) 
    // {
    //     operationValue = static_cast<int16_t>(std::stoi(token));
    // }

    return {operationCode, operationType, operationValue};
}

DWORD WINAPI socketReaderThreadLoop(LPVOID lpParam)
{
    startSocketReader();
    if (connectToServer("127.0.0.1", 3232)) 
    {
        std::string line;
        while (true) 
        {
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