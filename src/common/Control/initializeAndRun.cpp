#include "initializeAndRun.h"
#include "../../windows/socketRemoteControl.h"
#include "../commonHeaders.h"
#include "../settings.h"
#include "../../windows/testLeds.h"
#include "../../windows/socketRemoteControl.h"
#include "../IO/leds.h"
#include "../IO/analogInput.h"
#include "../Graphics/effectController.h"
#include "../Utility/time.h"
#include "../IO/tracing.h"
#include "../IO/serial.h"
#include "../Peripherals/microphone.h"
#include "../Peripherals/movementDetection.h"
#include "remoteCommandInterpreter.h"

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