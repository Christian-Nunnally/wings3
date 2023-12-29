#include "remoteCommandInterpreter.h"
#include "../IO/leds.h"
#include "../Graphics/effectController.h"
#include "../Peripherals/microphone.h"

void interpretRemoteCommand(uint8_t operationCode, int16_t value, uint8_t flags)
{
    if (operationCode == REMOTE_OPERATION_CODE_ENABLE_LEDS)
    {
        enableLeds();
    }
    else if (operationCode == REMOTE_OPERATION_CODE_DISABLE_LEDS)
    {
        disableLeds();
    }
    else if (operationCode == REMOTE_OPERATION_CODE_RANDOMIZE_EFFECTS)
    {
        randomizeEffectsNaturally();
    }
    else if (operationCode == REMOTE_OPERATION_CODE_ENABLE_MUSIC_DETECTION)
    {
        enableMusicDetection();
    }
    else if (operationCode == REMOTE_OPERATION_CODE_DISABLE_MUSIC_DETECTION)
    {
        disableMusicDetection();
    }

    else if (operationCode == REMOTE_OPERATION_CODE_ENABLE_RANDOM_EFFECT_CHANGE)
    {
        enableRandomEffectChangeBasedOnElapsedTime();
    }
    else if (operationCode == REMOTE_OPERATION_CODE_DISABLE_RANDOM_EFFECT_CHANGE)
    {
        disableRandomEffectChangeBasedOnElapsedTime();
    }
}

