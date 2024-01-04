#include "remoteCommandInterpreter.h"
#include "../settings.h"
#include "../IO/tracing.h"
#include "../IO/leds.h"
#include "../Graphics/effectController.h"
#include "../Graphics/savedEffectsSettings.h"
#include "../Graphics/effects.h"
#include "../Graphics/timeModes.h"
#include "../Graphics/effectSettings.h"
#include "../Peripherals/microphone.h"
#include "../Peripherals/movementDetection.h"
#include "remoteControl.h"

#define BRIGHTNESS_CHANGE_INCREMENT 10
#define SPEED_CHANGE_INCREMENT .1

#define REMOTE_OPERATION_CODE_NO_OP 0
#define REMOTE_OPERATION_CODE_CONTROL_LEDS 1
#define REMOTE_OPERATION_CODE_CONTROL_MICROPHONE 2
#define REMOTE_OPERATION_CODE_CONTROL_SPEED 3
#define REMOTE_OPERATION_CODE_CONTROL_BRIGHTNESS 4
#define REMOTE_OPERATION_CODE_CONTROL_MOVEMENT_DETECTION 5
#define REMOTE_OPERATION_CODE_CONTROL_STEP_DETECTION 6
#define REMOTE_OPERATION_CODE_CONTROL_TEMPORAL_FADE 7
#define REMOTE_OPERATION_CODE_CONTROL_SECONDARY_EFFECT 8
#define REMOTE_OPERATION_CODE_CONTROL_BACKGROUND_EFFECT 9
#define REMOTE_OPERATION_CODE_CONTROL_RANDOM_EFFECT_CHANGE 10
#define REMOTE_OPERATION_CODE_CONTROL_PRESET 11
#define REMOTE_OPERATION_CODE_EFFECT_TRIGGER 12

#define REMOTE_OPERATION_TYPE_NO_OP 0
#define REMOTE_OPERATION_TYPE_GET_VALUE 1
#define REMOTE_OPERATION_TYPE_SET_VALUE 2
#define REMOTE_OPERATION_TYPE_INCREMENT 3
#define REMOTE_OPERATION_TYPE_DECREMENT 4
#define REMOTE_OPERATION_TYPE_ENABLE 5
#define REMOTE_OPERATION_TYPE_DISABLE 6


SavedEffectSettings preset1;
SavedEffectSettings preset2;
SavedEffectSettings preset3;

void interpretRemoteCommand(RemoteControlCommand command)
{
    if (command.operationCode == REMOTE_OPERATION_CODE_CONTROL_LEDS)
    {
        if (command.operationType == REMOTE_OPERATION_TYPE_ENABLE) enableLeds();
        else if (command.operationType == REMOTE_OPERATION_TYPE_DISABLE) disableLeds();
    }
    else if (command.operationCode == REMOTE_OPERATION_CODE_CONTROL_MICROPHONE)
    {
        if (command.operationType == REMOTE_OPERATION_TYPE_ENABLE) enableMusicDetection();
        else if (command.operationType == REMOTE_OPERATION_TYPE_DISABLE) disableMusicDetection();
    }
    else if (command.operationCode == REMOTE_OPERATION_CODE_CONTROL_MOVEMENT_DETECTION)
    {
        if (command.operationType == REMOTE_OPERATION_TYPE_ENABLE) enableMovementTypeDetection();
        else if (command.operationType == REMOTE_OPERATION_TYPE_DISABLE) disableMovementTypeDetection();
    }    
    else if (command.operationCode == REMOTE_OPERATION_CODE_CONTROL_STEP_DETECTION)
    {
        if (command.operationType == REMOTE_OPERATION_TYPE_ENABLE) enableStepDetection();
        else if (command.operationType == REMOTE_OPERATION_TYPE_DISABLE) disableStepDetection();
    }
    else if (command.operationCode == REMOTE_OPERATION_CODE_CONTROL_RANDOM_EFFECT_CHANGE)
    {
        if (command.operationType == REMOTE_OPERATION_TYPE_ENABLE) effectSettings.RandomizeEffectsAutomaticallyOverTime = true;
        else if (command.operationType == REMOTE_OPERATION_TYPE_DISABLE) effectSettings.RandomizeEffectsAutomaticallyOverTime = false;
    }
    else if (command.operationCode == REMOTE_OPERATION_CODE_CONTROL_SPEED)
    {
        if (command.operationType == REMOTE_OPERATION_TYPE_INCREMENT)
        {
            float incrementedSpeed = getGlobalTimeFactor() + SPEED_CHANGE_INCREMENT;
            setGlobalTimeFactor(incrementedSpeed);
        }
        else if (command.operationType == REMOTE_OPERATION_TYPE_DECREMENT)
        {
            float decrementedSpeed = getGlobalTimeFactor() - SPEED_CHANGE_INCREMENT;
            setGlobalTimeFactor(decrementedSpeed);
        }
        else if (command.operationType == REMOTE_OPERATION_TYPE_SET)
        {
            setGlobalTimeFactor(command.value);
        }
    }
    else if (command.operationCode == REMOTE_OPERATION_CODE_CONTROL_BRIGHTNESS)
    {
        if (command.operationType == REMOTE_OPERATION_TYPE_INCREMENT)
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
        else if (command.operationType == REMOTE_OPERATION_TYPE_DECREMENT)
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
        else if (command.operationType == REMOTE_OPERATION_TYPE_SET)
        {
            setGlobalLedBrightness(command.value);
        }
    }
    else if (command.operationCode == REMOTE_OPERATION_CODE_EFFECT_TRIGGER)
    {
        switch (command.value)
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
    else if (command.operationCode == REMOTE_OPERATION_CODE_CONTROL_PRESET)
    {
        if (command.operationType == REMOTE_OPERATION_TYPE_SET)
        {
            switch (command.value)
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
            };
        }
        else if (command.operationType == REMOTE_OPERATION_TYPE_GET)
        {
            switch (command.value)
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
            };
        }
    }
    else if (command.operationCode == REMOTE_OPERATION_CODE_CONTROL_TEMPORAL_FADE)
    {
        if (command.operationType == REMOTE_OPERATION_TYPE_SET)
        {
            effectSettings.GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps = command.value;
            D_emitMetric(METRIC_NAME_ID_FADE_SETTING_AMOUNT, effectSettings.GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps);
        }
    }
}

