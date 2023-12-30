
#include "brightnessControlModes.h"
#include "../settings.h"
#include "../IO/tracing.h"
#include "../Utility/fastRandom.h"
#include "../Peripherals/movementDetection.h"
#include "../Peripherals/microphone.h"

#define BRIGHTNESS_CONTROL_MODE_NORMAL 0
#define BRIGHTNESS_CONTROL_MODE_MUSIC_BASED 1
#define BRIGHTNESS_CONTROL_MODE_MOVEMENT_BASED 2

// Brightness Mode variables.
const uint8_t NumberOfNormalGlobalBrightnessChangeModes = 3;
const uint8_t NumberOfMovementBasedGlobalBrightnessChangeModes = 3;
const uint8_t NumberOfMusicBasedGlobalBrightnessChangeModes = 3;
uint8_t brightnessModeMaxBrightness = UINT8_MAX;

// TODO: remove these or anything too dim.
uint8_t brightnessModeAlmostMaxBrightness = 200;
uint8_t brightnessModeHalfBrightness = UINT8_MAX / 2;
uint8_t brightnessModeAudioLevelBasedBrightness;
uint8_t brightnessModeAudioLevelBasedBrightnessBrighter;
uint8_t brightnessModeAudioLevelBasedBrightnessInverse;
uint8_t brightnessModePitchBasedMovement;
uint8_t brightnessModeYawBasedMovement;
uint8_t brightnessModeRollBasedMovement;
uint8_t* normalBrightnessModes[NumberOfNormalGlobalBrightnessChangeModes] = {&brightnessModeMaxBrightness, &brightnessModeAlmostMaxBrightness, &brightnessModeHalfBrightness};
uint8_t* movementBasedBrightnessModes[NumberOfMovementBasedGlobalBrightnessChangeModes] = {&brightnessModePitchBasedMovement, &brightnessModeYawBasedMovement, &brightnessModeRollBasedMovement};
uint8_t* musicBasedBrightnessModes[NumberOfMovementBasedGlobalBrightnessChangeModes] = {&brightnessModeAudioLevelBasedBrightness, &brightnessModeAudioLevelBasedBrightnessBrighter, &brightnessModeAudioLevelBasedBrightnessInverse};

void pickRandomGlobalBrightnessControlModesForEffect(Effect *effect, uint8_t likelihood, uint8_t likelihoodMusicBasedMode, uint8_t likelihoodMovementBasedMode)
{
    if (fastRandomByte() > likelihood) return;

    if ((fastRandomByte() < likelihoodMovementBasedMode) && (getCurrentMovementType() != Stationary)) 
    {
        effect->brightnessControlMode = BRIGHTNESS_CONTROL_MODE_MOVEMENT_BASED;
        effect->brightnessControlIndex = fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes);
    }
    else if (fastRandomByte() < likelihoodMusicBasedMode && isMusicDetected()) 
    {
        effect->brightnessControlMode = BRIGHTNESS_CONTROL_MODE_MUSIC_BASED;
        effect->brightnessControlIndex = fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes);
    }
    else 
    {
        effect->brightnessControlMode = BRIGHTNESS_CONTROL_MODE_NORMAL;
        effect->brightnessControlIndex = fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes);
    }
    setBrightnessPointerFromIndexForEffect(effect);
}

void incrementBrightnessModeLevels()
{
    const uint8_t half8BitMax = UINT8_MAX / 2;
    double currentAudioIntensityLevel = getAudioIntensityRatio();
    brightnessModeAudioLevelBasedBrightness = currentAudioIntensityLevel * UINT8_MAX;
    brightnessModeAudioLevelBasedBrightnessBrighter = half8BitMax + half8BitMax * currentAudioIntensityLevel;
    brightnessModeAudioLevelBasedBrightnessInverse = UINT8_MAX - (currentAudioIntensityLevel * UINT8_MAX);
    brightnessModePitchBasedMovement = (getCurrentPitchPosition() >> 19) & 0xff;
    brightnessModeRollBasedMovement = (getCurrentRollPosition() >> 19) & 0xff;
    brightnessModeYawBasedMovement = (getCurrentYawPosition() >> 19) & 0xff;
    D_emitDoubleMetric("currentAudioIntensityLevel", currentAudioIntensityLevel);
}

void setBrightnessPointerFromIndexForEffect(Effect *effect)
{
    if (effect->brightnessControlMode == BRIGHTNESS_CONTROL_MODE_MOVEMENT_BASED) effect->globalBrightnessPointer = movementBasedBrightnessModes[effect->brightnessControlIndex];
    else if (effect->brightnessControlMode == BRIGHTNESS_CONTROL_MODE_MUSIC_BASED) effect->globalBrightnessPointer = musicBasedBrightnessModes[effect->brightnessControlIndex];
    else effect->globalBrightnessPointer = normalBrightnessModes[effect->brightnessControlIndex];
}