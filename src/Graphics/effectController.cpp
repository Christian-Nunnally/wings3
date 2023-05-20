#include <Arduino.h>
#include "../settings.h"
#include "../Peripherals/microphone.h"
#include "../Peripherals/movementDetection.h"
#include "../Graphics/effectController.h"
#include "../Graphics/transformMaps.h"
#include "../Graphics/screenMaps.h"
#include "../Graphics/effectSettings.h"
#include "../Graphics/colorMixing.h"
#include "../Graphics/palettes.h"
#include "../Graphics/effect.h"
#include "../Graphics/effects.h"
#include "../Graphics/moods.h"
#include "../Graphics/Effects/gradientWaveEffect.h"
#include "../Utility/time.h"
#include "../IO/analogInput.h"
#include "../Observers/stepDectectedObserver.h"
#include "../Observers/movementDetectedObserver.h"
#include "../Utility/fastRandom.h"

Color blendIncorporatingOldMixingMode(Color color1, Color color2);
void handleStepDetected();
void handleMovementDetected();
void incrementBrightnessModeLevels();
void incrementTime();
void incrementTime(Effect *effect, int timeDelta);  
void incrementTransition();
void incrementColorPalettesTowardsTargets();
void detectBeat();
void randomizeEffectsNaturally();
void pickRandomBlendingMode();
void pickRandomSubPalette();
void pickRandomPalette();
void pickRandomPaletteForEffect(Effect *effect);
void pickRandomSubPaletteForEffect(Effect *effect);
int pickRandomSubPaletteFromPalette(int palette);
void pickRandomTransformMaps();
void pickRandomTransformMaps(Effect *effect, byte likelihood);
void pickRandomMirroredTransformMaps(Effect *effect, byte likelihood);
void swapEffects();
void pickRandomScreenMap();
void pickRandomTimeModes();
void pickRandomTimeModesForEffect(Effect *effect, bool pickFromMovementBasedTimeModes, bool pickFromNonMovementBasedTimeModes);
void pickRandomSizeParametersForEffects();
void pickRandomGlobalBrightnessControlModes();
void pickRandomGlobalBrightnessControlModesForEffect(Effect *effect, byte likelihood);
void pickRandomTransitionTime();
void pickRandomAudioLevelThresholdForMoreIntenseEffect();
void switchTransitionDirection();
void incrementColorPalettesTowardsTargetsForEffect(Effect *effect);
void incrementEffects();
inline Color getEffectWithAudioDrivenIntensity(Effect *effect, float threshold, int index);

Effect currentPrimaryEffectA;
Effect currentPrimaryEffectB;
Effect currentSecondaryEffectA;
Effect currentSecondaryEffectB;
EffectSettings effectSettingsStationary;
EffectSettings effectSettingsWalking;
EffectSettings effectSettingsJogging;
EffectSettings effectSettingsBiking;
EffectSettings effectSettingsDriving;

// Mixing Mode variables.
const int MaxNumberOfMixingModeBlendFunctions = 20;
int numberOfMixingModeBlendFunctions;
Color (*mixingModeBlendFunctions[MaxNumberOfMixingModeBlendFunctions])(Color color1, Color color2, uint8_t transitionAmount);
Color (*mixingModeBlendFunction)(Color color1, Color color2, uint8_t transitionAmount) {};
Color (*oldMixingModeBlendFunction)(Color color1, Color color2, uint8_t transitionAmount) {};
int mixingMode;
int oldMixingMode;
int mixingModeBlendCounter;
int mixingModeBlendCounterMax = 1;

// Time Mode variables.
const byte MaxNumberOfTimeModes = 40;
byte numberOfTimeModes;
byte numberOfMovementBasedTimeModes;
int (*timeModeIncrementFunctions[MaxNumberOfTimeModes])(int currentTime, int timeDelta);

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

// Screen Mode variables.
byte *currentScreenMap[TOTAL_LEDS];
Color ledColorMap[TOTAL_LEDS];

// Transition variables.
uint16_t percentOfEffectBToShow;
uint8_t percentOfEffectBToShow8Bit;
uint8_t percentOfOldMixingModeToMixIn8Bit;
uint8_t percentOfSecondaryEffectToShow;
int currentTransitionIncrement;
bool transitionDirection;

int frameTimeDelta;
int currentTime;
int lastPeakDetectorValue;
int millisecondsLeftInTransitionFromSecondaryToPrimaryEffect;
int millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax = 1;
bool primaryEffectToggle = false;
float effectA1AudioLevelThresholdToShowMoreIntenseEffect = .98;
float effectB1AudioLevelThresholdToShowMoreIntenseEffect = .6;
const float AudioInfluenceFactorForAudioScaledTime = 2.0;

Color getLedColorForFrame(int ledIndex)
{
    byte currentScreen = (*currentScreenMap)[ledIndex];
    if (currentScreen == 4) return {0, 0, 0};
    else if (currentScreen & 1) return effectA1.effectFunctionHighlight(ledIndex);
    
    Color color1 = getEffectWithAudioDrivenIntensity(&currentPrimaryEffectA, effectA1AudioLevelThresholdToShowMoreIntenseEffect, ledIndex);
    Color color2 = getEffectWithAudioDrivenIntensity(&currentPrimaryEffectB, effectB1AudioLevelThresholdToShowMoreIntenseEffect, ledIndex);
    Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2);
    if (millisecondsLeftInTransitionFromSecondaryToPrimaryEffect)
    {
        Color color3 = getEffectWithAudioDrivenIntensity(&currentSecondaryEffectA, effectA1AudioLevelThresholdToShowMoreIntenseEffect, ledIndex);
        Color color4 = getEffectWithAudioDrivenIntensity(&currentSecondaryEffectB, effectB1AudioLevelThresholdToShowMoreIntenseEffect, ledIndex);
        Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4);
        resultColor1 = blendColorsUsingMixing(resultColor1, resultColor2, percentOfSecondaryEffectToShow);
        ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps);
    }
    else ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps);
    return ledColorMap[ledIndex];
}

inline Color getEffectWithAudioDrivenIntensity(Effect *effect, float threshold, int index)
{
    return currentAudioIntensityLevel < threshold ? effect->effectFunction(index) : effect->effectFunctionHighlight(index);
}

void incrementEffectFrame()
{
    incrementBrightnessModeLevels();
    incrementTime();
    incrementTransition();
    incrementColorPalettesTowardsTargets();
    detectBeat();
    incrementEffects();
}

void setupEffects()
{
    currentPrimaryEffectA = effectA1;
    currentPrimaryEffectB = effectB1;
    currentSecondaryEffectA = effectA2;
    currentSecondaryEffectB = effectB2;
    effectA1.effectFunctionHighlight = [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectA1); };
    effectB1.effectFunctionHighlight = [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectB1); };
    effectA2.effectFunctionHighlight = [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectA2); };
    effectB2.effectFunctionHighlight = [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectB2); };
    effectA1.frameTimeDelta = &frameTimeDelta;
    effectB1.frameTimeDelta = &frameTimeDelta;
    effectA2.frameTimeDelta = &frameTimeDelta;
    effectB2.frameTimeDelta = &frameTimeDelta;

    mixingModeBlendFunction = blendColorsUsingMixing;
    oldMixingModeBlendFunction = blendColorsUsingMixing;
    subscribeToStepDetectedEvent(handleStepDetected);
    subscribeToMovementDetectedEvent(handleMovementDetected);

    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + (timeDelta >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)timeDelta / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)timeDelta >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - (timeDelta >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)timeDelta / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)timeDelta >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };

    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 11); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 11); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 11); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 12); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 12); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 12); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 10); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 10); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 10); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 11)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 11)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 11)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 12)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 12)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 12)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 10)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 10)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 10)); };

    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixing;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAdd;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingScreen;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAverage;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixingGlitched;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixing;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAdd;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingScreen;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAverage;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingShimmer;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixingGlitched;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixing;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAdd;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingScreen;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAverage;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingShimmer;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingOverlay;

    setupNormalMood(&effectSettings);
    //setupTestMood(&effectSettings);
    *currentScreenMap = normalScreenMap;
    for (int i = 0; i < 500; i++) randomizeEffectsNaturally();

    effectSettingsStationary = effectSettings;
    effectSettingsWalking = effectSettings;
    effectSettingsJogging = effectSettings;
    effectSettingsBiking = effectSettings;
    effectSettingsDriving = effectSettings;

}

inline Color blendIncorporatingOldMixingMode(Color color1, Color color2)
{
    Color newColor = mixingModeBlendFunction(color1, color2, percentOfEffectBToShow8Bit);
    if (oldMixingMode == mixingMode || mixingModeBlendCounter == 0) return newColor;
    Color oldColor = oldMixingModeBlendFunction(color1, color2, percentOfEffectBToShow8Bit);
    return blendColorsUsingMixing(newColor, oldColor, percentOfOldMixingModeToMixIn8Bit);
}

void handleStepDetected()
{
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenStepIsDetected) randomizeEffectsNaturally();
}

void handleMovementDetected()
{
    MovementType movementType = getCurrentMovementType();
    if (movementType == Stationary) effectSettings = effectSettingsStationary;
    if (movementType == Walking) effectSettings = effectSettingsWalking;
    if (movementType == Jogging) effectSettings = effectSettingsJogging;
    if (movementType == Biking) effectSettings = effectSettingsBiking;
    if (movementType == Driving) effectSettings = effectSettingsDriving;

    pickRandomTimeModesForEffect(&effectA1, movementType != Stationary, movementType == Stationary);
    pickRandomTimeModesForEffect(&effectB1, movementType != Stationary, movementType == Stationary);
    pickRandomTimeModesForEffect(&effectA2, movementType != Stationary, movementType == Stationary);
    pickRandomTimeModesForEffect(&effectB2, movementType != Stationary, movementType == Stationary);
}

void pickRandomTimeModesForEffect(Effect *effect, bool pickFromMovementBasedTimeModes, bool pickFromNonMovementBasedTimeModes)
{
    if (pickFromMovementBasedTimeModes && pickFromNonMovementBasedTimeModes)
    {
        effect->timeMode1 = fastRandomInteger(numberOfMovementBasedTimeModes + numberOfTimeModes);
        effect->timeMode2 = fastRandomInteger(numberOfMovementBasedTimeModes + numberOfTimeModes);
    }
    else if (pickFromMovementBasedTimeModes)
    {
        effect->timeMode1 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
        effect->timeMode2 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
    }
    else 
    {
        effect->timeMode1 = fastRandomInteger(numberOfTimeModes);
        effect->timeMode2 = fastRandomInteger(numberOfTimeModes);
    }
}

void incrementBrightnessModeLevels()
{
    const uint8_t half8BitMax = UINT8_MAX / 2;
    currentAudioIntensityLevel = getAudioIntensityRatio();
    brightnessModeAudioLevelBasedBrightness = currentAudioIntensityLevel * UINT8_MAX;
    brightnessModeAudioLevelBasedBrightnessBrighter = half8BitMax + half8BitMax * currentAudioIntensityLevel;
    brightnessModeAudioLevelBasedBrightnessInverse = UINT8_MAX - (currentAudioIntensityLevel * UINT8_MAX);
    brightnessModePitchBasedMovement = (getCurrentPitchPosition() >> 19) & 0xff;
    brightnessModeRollBasedMovement = (getCurrentRollPosition() >> 19) & 0xff;
    brightnessModeYawBasedMovement = (getCurrentYawPosition() >> 19) & 0xff;
}

const int TimeDeltaResolutionIncreaseFactor = 64;
inline void incrementTime()
{
    int newTime = (int)getTime();
    frameTimeDelta = (newTime - currentTime);
    int timeDelta = (frameTimeDelta * TimeDeltaResolutionIncreaseFactor);
    currentTime = newTime;
    incrementTime(&effectA1, timeDelta);
    incrementTime(&effectB1, timeDelta);
    incrementTime(&effectA2, timeDelta);
    incrementTime(&effectB2, timeDelta);
    #ifdef ENABLE_SERIAL
    Serial.print("ft: ");
    Serial.println(frameTimeDelta);
    #endif
}

inline void incrementTime(Effect *effect, int timeDelta)
{
    effect->time1 = timeModeIncrementFunctions[effect->timeMode1](effect->time1, timeDelta) / TimeDeltaResolutionIncreaseFactor;
    effect->time2 = timeModeIncrementFunctions[effect->timeMode1](effect->time1, timeDelta) / TimeDeltaResolutionIncreaseFactor;
}

inline void incrementTransition()
{
    int incrementAmount = currentTransitionIncrement * frameTimeDelta;

    if (transitionDirection)
    {
        if (UINT16_MAX - percentOfEffectBToShow > incrementAmount)
        {
            percentOfEffectBToShow += incrementAmount;
        }
        else 
        {
            percentOfEffectBToShow = UINT16_MAX;
        }
    }
    else 
    {
        if (percentOfEffectBToShow > incrementAmount)
        {
            percentOfEffectBToShow -= incrementAmount;
        }
        else 
        {
            percentOfEffectBToShow = 0;
        }
    }
    percentOfEffectBToShow8Bit = percentOfEffectBToShow >> 8;

    if (mixingModeBlendCounter) 
    {
        if (mixingModeBlendCounter - frameTimeDelta > 0) mixingModeBlendCounter -= frameTimeDelta;
        else mixingModeBlendCounter = 0;
        percentOfOldMixingModeToMixIn8Bit = (mixingModeBlendCounter / mixingModeBlendCounterMax) * UINT8_MAX;
    }
    else if (oldMixingMode != mixingMode) 
    {
        oldMixingMode = mixingMode;
        oldMixingModeBlendFunction = mixingModeBlendFunction;
    }

    if (millisecondsLeftInTransitionFromSecondaryToPrimaryEffect - frameTimeDelta > 0) millisecondsLeftInTransitionFromSecondaryToPrimaryEffect -= frameTimeDelta;
    else millisecondsLeftInTransitionFromSecondaryToPrimaryEffect = 0;
    percentOfSecondaryEffectToShow = (millisecondsLeftInTransitionFromSecondaryToPrimaryEffect / millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax) * UINT8_MAX;
}

void incrementColorPalettesTowardsTargets()
{
    static int lastMoveColorPalettesTowardsTargetsTime;
    if (currentTime - lastMoveColorPalettesTowardsTargetsTime < effectSettings.MillisecondToMoveToNextPaletteFrame) return;
    lastMoveColorPalettesTowardsTargetsTime = currentTime;
    incrementColorPalettesTowardsTargetsForEffect(&effectA1);
    incrementColorPalettesTowardsTargetsForEffect(&effectB1);
    incrementColorPalettesTowardsTargetsForEffect(&effectA2);
    incrementColorPalettesTowardsTargetsForEffect(&effectB2);
}

void incrementColorPalettesTowardsTargetsForEffect(Effect *effect)
{
    if (effect->currentPaletteOffset > effect->currentPaletteOffsetTarget) effect->currentPaletteOffset -= PALETTE_LENGTH;
    else if (effect->currentPaletteOffset < effect->currentPaletteOffsetTarget) effect->currentPaletteOffset += PALETTE_LENGTH;
}

void incrementEffects()
{
    effectA1.effectFunctionIncrement();
    if (effectA1.effectFunctionIncrementUniqueId != effectB1.effectFunctionIncrementUniqueId) effectB1.effectFunctionIncrement();
}

void detectBeat()
{
    const int PositivePeak = 1;
    int nextPeak = getCurrentPeakDetectorValue();
    if (nextPeak == lastPeakDetectorValue) return;
    lastPeakDetectorValue = nextPeak;
    if (nextPeak == PositivePeak && fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetected) randomizeEffectsNaturally();
    else if (!nextPeak && fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero) randomizeEffectsNaturally();
    else if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenAntiBeatDetected) randomizeEffectsNaturally();
}

void randomizeEffectsNaturally()
{
    Serial.write("random");
    Serial.flush();
    if (fastRandomByte() < effectSettings.LikelihoodNothingChangesWhenRandomizingEffect) return;
    if (fastRandomByte() < effectSettings.LikelihoodBlendingModeChangesWhenRandomizingEffect) pickRandomBlendingMode();
    if (fastRandomByte() < effectSettings.LikelihoodSubPaletteChangesWhenRandomizingEffect) pickRandomSubPalette();
    if (fastRandomByte() < effectSettings.LikelihoodPaletteChangesWhenRandomizingEffect) pickRandomPalette();
    if (fastRandomByte() < effectSettings.LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect) pickRandomTransformMaps();
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreSwappedWhenRandomizingEffect) swapEffects();
    if (fastRandomByte() < effectSettings.LikelihoodTimeModesRandomizeWhenRandomizingEffect) pickRandomTimeModes();
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAndRandomizedWhenRandomizingEffect) pickRandomEffects();
    if (fastRandomByte() < effectSettings.LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect) pickRandomSizeParametersForEffects();
    if (fastRandomByte() < effectSettings.LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect) pickRandomGlobalBrightnessControlModes();
    if (fastRandomByte() < effectSettings.LikelihoodTransitionDirectionChangesWhenRandomizingEffect) switchTransitionDirection();
    if (fastRandomByte() < effectSettings.LikelihoodBackgroundTransitionTimeChangesWhenRandomizingEffect) pickRandomTransitionTime();
    if (fastRandomByte() < effectSettings.LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect) pickRandomAudioLevelThresholdForMoreIntenseEffect();
    if (fastRandomByte() < effectSettings.LikelihoodScreenMapChangesWhenRandomizingEffect) pickRandomScreenMap();
}

void pickRandomScreenMap()
{
    *currentScreenMap = screenMaps[fastRandomInteger(screenMapsCount)];
}

void pickRandomBlendingMode()
{
    mixingModeBlendFunction = mixingModeBlendFunctions[fastRandomInteger(numberOfMixingModeBlendFunctions)];
    mixingModeBlendCounterMax = fastRandomInteger(effectSettings.MillisecondsForBlendingModeTransitionsMinimum, effectSettings.MillisecondsForBlendingModeTransitionsMaximum);
    mixingModeBlendCounter = mixingModeBlendCounterMax;
}

void pickRandomSubPalette()
{
    pickRandomSubPaletteForEffect(&effectA1);
    pickRandomSubPaletteForEffect(&effectB1);
    pickRandomSubPaletteForEffect(&effectA2);
    pickRandomSubPaletteForEffect(&effectB2);
}

void pickRandomSubPaletteForEffect(Effect *effect)
{
    effect->currentPaletteOffsetTarget = pickRandomSubPaletteFromPalette(effect->currentPalette);
}

void pickRandomPalette()
{
    pickRandomPaletteForEffect(&effectA1);
    if (fastRandomByte() < effectSettings.LikelihoodTwoPalettesAreUsedWhenPaletteChanges) 
    {
        pickRandomPaletteForEffect(&effectB1);
        pickRandomPaletteForEffect(&effectA2);
        pickRandomPaletteForEffect(&effectB2);
    }
    else
    {
        effectB1.currentPalette = effectA1.currentPalette;
        effectA2.currentPalette = effectA1.currentPalette;
        effectB2.currentPalette = effectA1.currentPalette;
    }
}

void pickRandomPaletteForEffect(Effect *effect)
{
    byte numberOfPossiblePalettesToSwitchTo = effectSettings.AllowedPalettes[effect->currentPalette][0];
    byte indexOfNextPalette = 1 + fastRandomInteger(numberOfPossiblePalettesToSwitchTo);
    byte nextPalette = effectSettings.AllowedPalettes[effect->currentPalette][indexOfNextPalette];
    effect->currentPalette = nextPalette;
}

int pickRandomSubPaletteFromPalette(int palette)
{
    int subPaletteOffset = fastRandomInteger(paletteHeights[palette]) * PALETTE_LENGTH;
    return paletteStarts[palette] + subPaletteOffset;
}

void pickRandomTransformMaps()
{
    if (fastRandomByte() < effectSettings.LikelihoodWingsAreMirroredWhenTransformMapsAreRandomized)
    {
        pickRandomMirroredTransformMaps(&effectA1, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
        pickRandomMirroredTransformMaps(&effectB1, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
        pickRandomMirroredTransformMaps(&effectA2, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
        pickRandomMirroredTransformMaps(&effectB2, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
    }
    else 
    {
        pickRandomTransformMaps(&effectA1, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
        pickRandomTransformMaps(&effectB1, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
        pickRandomTransformMaps(&effectA2, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
        pickRandomTransformMaps(&effectB2, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
    }
}

void pickRandomTransformMaps(Effect *effect, byte likelihood)
{
    if (fastRandomByte() < likelihood) (*effect->transformMap1) = transformMaps[fastRandomInteger(transformMapsCount)];
    if (fastRandomByte() < likelihood) (*effect->transformMap2) = transformMaps[fastRandomInteger(transformMapsCount)];
}

void pickRandomMirroredTransformMaps(Effect *effect, byte likelihood)
{
    if (fastRandomByte() < likelihood) (*effect->transformMap1) = mirroredTransformMaps[fastRandomInteger(transformMapsCount)];
    if (fastRandomByte() < likelihood) (*effect->transformMap2) = mirroredTransformMaps[fastRandomInteger(transformMapsCount)];
}

void swapEffects()
{
    primaryEffectToggle = !primaryEffectToggle;
    if (primaryEffectToggle)
    {
        currentPrimaryEffectA = effectA1;
        currentPrimaryEffectB = effectB1;
        currentSecondaryEffectA = effectA2;
        currentSecondaryEffectB = effectB2;
    }
    else 
    {
        currentPrimaryEffectA = effectA2;
        currentPrimaryEffectB = effectB2;
        currentSecondaryEffectA = effectA1;
        currentSecondaryEffectB = effectB1;
    }

    if (primaryEffectToggle)
    {
        pickRandomTransformMaps(&effectA1, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched);
        pickRandomTransformMaps(&effectB1, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched);
    }
    else
    {
        pickRandomTransformMaps(&effectA2, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched);
        pickRandomTransformMaps(&effectB2, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched);
    }
    millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax = fastRandomInteger(effectSettings.MillisecondsForEffectTransitionsMinimum, effectSettings.MillisecondsForEffectTransitionsMaximum);
    millisecondsLeftInTransitionFromSecondaryToPrimaryEffect = millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax;
}

void pickRandomTimeModes()
{
    int startIndex = 0;
    int length = numberOfTimeModes;
    if ((fastRandomByte() < effectSettings.LikelihoodMovementBasedTimeModeIsPicked) && (getCurrentMovementType() != Stationary))
    {
        startIndex = numberOfTimeModes;
        length = numberOfMovementBasedTimeModes;
    }
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effectA1.timeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effectB1.timeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effectA2.timeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effectB2.timeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effectA1.timeMode2 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effectB1.timeMode2 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effectA2.timeMode2 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effectB2.timeMode2 = fastRandomInteger(length) + startIndex;
}

void pickRandomSizeParametersForEffects()
{
    effectA1.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
    effectB1.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
    effectA2.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
    effectB2.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
}

void pickRandomGlobalBrightnessControlModes()
{
    pickRandomGlobalBrightnessControlModesForEffect(&effectA1, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange);
    pickRandomGlobalBrightnessControlModesForEffect(&effectB1, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange);
    pickRandomGlobalBrightnessControlModesForEffect(&effectA2, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange);
    pickRandomGlobalBrightnessControlModesForEffect(&effectB2, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange);
}

void pickRandomGlobalBrightnessControlModesForEffect(Effect *effect, byte likelihood)
{
    if (fastRandomByte() > likelihood) return;
    if ((fastRandomByte() < effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked) && (getCurrentMovementType() != Stationary)) effect->globalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
    else if (fastRandomByte() < effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked) effect->globalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
    else effect->globalBrightnessPointer = normalBrightnessModes[fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes)];
}

void pickRandomTransitionTime()
{
    currentTransitionIncrement = UINT16_MAX / fastRandomInteger(effectSettings.MillisecondsForBlendingModeTransitionsMinimum, effectSettings.MillisecondsForBlendingModeTransitionsMaximum);
}

void switchTransitionDirection()
{
    transitionDirection = !transitionDirection;
}

void pickRandomAudioLevelThresholdForMoreIntenseEffect()
{
    effectA1AudioLevelThresholdToShowMoreIntenseEffect = (float)fastRandomInteger(effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMinimum, effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMaximum) / (float)UINT8_MAX;
    effectB1AudioLevelThresholdToShowMoreIntenseEffect = (float)fastRandomInteger(effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMinimum, effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMaximum) / (float)UINT8_MAX;
}