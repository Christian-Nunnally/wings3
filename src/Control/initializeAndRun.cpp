#include "../Control/initializeAndRun.h"
#include "../windows/socketRemoteControl.h"
#include "../../src/commonHeaders.h"
#include "../../src/settings.h"
#include "../windows/testLeds.h"
#include "../windows/socketRemoteControl.h"
#include "../../src/IO/leds.h"
#include "../../src/IO/analogInput.h"
#include "../../src/Graphics/effectController.h"
#include "../../src/Utility/time.h"
#include "../../src/IO/tracing.h"
#include "../../src/IO/serial.h"
#include "../../src/Peripherals/microphone.h"
#include "../../src/Peripherals/movementDetection.h"
#include "../../src/Control/remoteCommandInterpreter.h"

void initialize()
{
    D_serialBegin();
    setupLeds();
    setupMicrophone();
    setupImu();
    setupEffects();
    setupAnalogInputs();
}

void run()
{
    while(true)
    {
        setTime(getSystemTime());
        incrementEffectFrame();
        renderLeds();
        processAudioStream();
        checkForMovement();
        readAnalogValues();
        D_serialRead();

        #ifdef RP2040
        //processRealRemoteInput();
        #else
        processFakeRemoteInput();
        #endif
    }
}