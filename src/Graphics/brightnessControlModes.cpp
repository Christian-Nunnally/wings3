
#include "../Graphics/brightnessControlModes.h"
#include "../Utility/fastRandom.h"
#include "../Peripherals/movementDetection.h"
#include "../Peripherals/microphone.h"

// Brightness Mode variables.
const byte NumberOfNormalGlobalBrightnessChangeModes = 3;
const byte NumberOfMovementBasedGlobalBrightnessChangeModes = 3;
const byte NumberOfMusicBasedGlobalBrightnessChangeModes = 3;
uint8_t brightnessModeMaxBrightness = UINT8_MAX;
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

void pickRandomGlobalBrightnessControlModesForEffect(Effect *effect, byte likelihood, byte likelihoodMusicBasedMode, byte likelihoodMovementBasedMode)
{
    if (fastRandomByte() > likelihood) return;
    if ((fastRandomByte() < likelihoodMovementBasedMode) && (getCurrentMovementType() != Stationary)) effect->globalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
    else if (fastRandomByte() < likelihoodMusicBasedMode) effect->globalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
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