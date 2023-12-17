#define ENABLE_TRACING

#include "../tests/testLeds.h"
#include "../tests/SocketReader.h"
#include "../src/IO/leds.h"
#include "../src/Graphics/effectController.h"
#include "../src/Utility/time.h"
#include "../src/IO/tracing.h"
#include "../src/Peripherals/microphone.h"
#include "../src/Peripherals/movementDetection.h"
#include "../tests/SocketReader.h"
#include <iostream>
#include <chrono>


#include <iostream>
#include <cstring>
#include <unistd.h>
#include <WinSock2.h>
#include <string>

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
            //std::cout << "Received: " << line << std::endl;
            D_emitMetricString("Received", line);
        } else {
        }
    }
}