#define ENABLE_TRACING

#include "../tests/testLeds.h"
#include "../tests/SocketReader.h"
#include "../src/IO/leds.h"
#include "../src/Graphics/effectController.h"
#include "../src/Utility/time.h"
#include "../src/IO/tracing.h"
#include "../src/Peripherals/microphone.h"
#include "../src/Peripherals/movementDetection.h"
#include "../src/Control/remoteCommandInterpreter.h"
#include "../tests/SocketReader.h"
#include <iostream>
#include <chrono>


#include <iostream>
#include <cstring>
#include <unistd.h>
#include <WinSock2.h>
#include <string>
#include <sstream>

using namespace std::chrono;

int main() {
    runInNewThread();

    setupLeds();
    //setupMicrophone();
    //setupImu();
    setupEffects();
    //setupAnalogInputs();

    while(true)
    {
        setTime(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
        incrementEffectFrame();
        renderLeds();
        //processAudioStream();
        //checkForMovement();
        //readAnalogValues();
        //D_emitIntegerMetric("frame", frameNumber++);
        //D_emitMetric("frame", "test");

        std::string line;
        line = readLineNonBlocking();
        if (!line.empty()) {
            std::stringstream ss(line);

            std::string token;
            uint8_t operationCode = 0;
            int16_t operationValue = 0;
            uint8_t operationFlags = 0;

            std::getline(ss, token, ',');
            if (!token.empty()) {
                operationCode = static_cast<uint8_t>(std::stoi(token));
            }

            std::getline(ss, token, ',');
            if (!token.empty()) {
                operationValue = static_cast<int16_t>(std::stoi(token));
            }

            std::getline(ss, token, ',');
            if (!token.empty()) {
                operationFlags = static_cast<uint8_t>(std::stoi(token));
            }

            interpretRemoteCommand(operationCode, operationValue, operationFlags);
            D_emitMetricString("Received", line);
        } else {
        }
    }
}