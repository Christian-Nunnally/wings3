
#include "../Graphics/brightnessControlModes.h"
#include "../Utility/fastRandom.h"
#include "../Peripherals/movementDetection.h"
#include "../Peripherals/microphone.h"

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
    if ((fastRandomByte() < likelihoodMovementBasedMode) && (getCurrentMovementType() != Stationary)) effect->globalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
    else if (fastRandomByte() < likelihoodMusicBasedMode && isMusicDetected()) effect->globalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
    else effect->globalBrightnessPointer = normalBrightnessModes[fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes)];
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
}