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
#include "../../windows/profiling.h"

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
        startProfile(100);
        renderLeds();
        stopProfile();
        processAudioStream();
        checkForMovement();
        readAnalogValues();
        D_serialRead();
        processRemoteInput();

        #ifdef ENABLE_TRACING
        processMetrics();
        #endif
    }
}