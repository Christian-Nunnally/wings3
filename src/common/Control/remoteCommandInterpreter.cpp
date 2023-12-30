#include "remoteCommandInterpreter.h"
#include "../IO/leds.h"
#include "../Graphics/effectController.h"
#include "../Graphics/savedEffectsSettings.h"
#include "../Graphics/effects.h"
#include "../Peripherals/microphone.h"

#define BRIGHTNESS_CHANGE_INCREMENT 10

SavedEffectSettings preset1;
SavedEffectSettings preset2;
SavedEffectSettings preset3;

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

    else if (operationCode == REMOTE_OPERATION_CODE_INCREASE_BRIGHTNESS)
    {
        uint8_t currentBrightness = getGlobalLedBrightness();
        uint16_t incrementedBrightness = currentBrightness + BRIGHTNESS_CHANGE_INCREMENT;
        if (incrementedBrightness <= 255)
        {
            setGlobalLedBrightness(incrementedBrightness);
        }
        else 
        {
            setGlobalLedBrightness(255);
        }
    }
    else if (operationCode == REMOTE_OPERATION_CODE_DECREASE_BRIGHTNESS)
    {
        uint8_t currentBrightness = getGlobalLedBrightness();
        int16_t decrementedBrightness = currentBrightness - BRIGHTNESS_CHANGE_INCREMENT;
        if (decrementedBrightness >= 0)
        {
            setGlobalLedBrightness(decrementedBrightness);
        }
        else 
        {
            setGlobalLedBrightness(0);
        }
    }

    else if (operationCode == REMOTE_OPERATION_CODE_ENABLE_RANDOM_EFFECT_CHANGE)
    {
        enableRandomEffectChangeBasedOnElapsedTime();
    }
    else if (operationCode == REMOTE_OPERATION_CODE_DISABLE_RANDOM_EFFECT_CHANGE)
    {
        disableRandomEffectChangeBasedOnElapsedTime();
    }
    else if (operationCode == REMOTE_OPERATION_CODE_EFFECT_TRIGGER)
    {
        switch (value)
        {
            case 0:
                for (int i = 0; i < 10; i++) randomizeEffectsNaturally();
                break;
            case 1:
                for (int i = 0; i < 100; i++) randomizeEffectsNaturally();
                break;    
            default:
                break;
        }
    }
    else if (operationCode == REMOTE_OPERATION_CODE_SET_PRESET)
    {
        switch (value)
        {
            case 1:
                saveCurrentEffectsState(&preset1);
                break;
            case 2:
                saveCurrentEffectsState(&preset2);
                break;
            case 3:
                saveCurrentEffectsState(&preset3);
                break;
            default:
                break;
        }
    }
    else if (operationCode == REMOTE_OPERATION_CODE_SELECT_PRESET)
    {
        switch (value)
        {
            case 1:
                loadCurrentEffectsState(&preset1);
                break;
            case 2:
                loadCurrentEffectsState(&preset2);
                break;
            case 3:
                loadCurrentEffectsState(&preset3);
                break;
            default:
                break;
        }
    }
}

