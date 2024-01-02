#include "initializeAndRun.h"
#include "remoteControl.h"
#include "../commonHeaders.h"
#include "../settings.h"
#include "../IO/leds.h"
#include "../IO/analogInput.h"
#include "../Graphics/effectController.h"
#include "../Utility/time.h"
#include "../IO/tracing.h"
#include "../IO/serial.h"
#include "../Peripherals/microphone.h"
#include "../Peripherals/movementDetection.h"

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
        processRemoteInput();
    }
}