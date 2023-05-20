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
#include "../Graphics/timeModes.h"
#include "../Graphics/brightnessControlModes.h"
#include "../Graphics/Effects/gradientWaveEffect.h"
#include "../Utility/time.h"
#include "../IO/analogInput.h"
#include "../Observers/stepDectectedObserver.h"
#include "../Observers/movementDetectedObserver.h"
#include "../Utility/fastRandom.h"

Color blendIncorporatingOldMixingMode(Color color1, Color color2);
void handleStepDetected();
void handleMovementDetected();
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
void pickRandomTransitionTime();
void pickRandomAudioLevelThresholdForMoreIntenseEffect();
void switchTransitionDirection();
void incrementColorPalettesTowardsTargetsForEffect(Effect *effect);
inline Color getEffectWithAudioDrivenIntensity(Effect *effect, float threshold, int index);

Effect currentPrimaryEffectA;
Effect currentPrimaryEffectB;
Effect currentSecondaryEffectA;
Effect currentSecondaryEffectB;
EffectSettings effectSettingsStationary;
EffectSettings effectSettingsMoving;

// Mixing Mode variables.
const int MaxNumberOfMixingModeBlendFunctions = 20;
int numberOfMixingModeBlendFunctions;
Color (*mixingModeBlendFunctions[MaxNumberOfMixingModeBlendFunctions])(Color color1, Color color2, uint8_t transitionAmount);
Color (*mixingModeBlendFunction)(Color color1, Color color2, uint8_t transitionAmount) {};
Color (*oldMixingModeBlendFunction)(Color color1, Color color2, uint8_t transitionAmount) {};
int mixingMode;
int millisecondsLeftInMixingModeBlend;
int millisecondsLeftInMixingModeBlendTotalDuration = 1;

// Time Mode variables.
// const byte MaxNumberOfTimeModes = 40;
// byte numberOfTimeModes;
// byte numberOfMovementBasedTimeModes;
// int (*timeModeIncrementFunctions[MaxNumberOfTimeModes])(int currentTime, int timeDelta);

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
// const float AudioInfluenceFactorForAudioScaledTime = 2.0;

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
    incrementMixingModeBlend();
    incrementTransitionFromSecondaryToPrimaryEffect();
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

    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + (timeDelta >> 1); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)timeDelta / 3); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)timeDelta >> 2); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 1); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) / 3); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 2); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - (timeDelta >> 1); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)timeDelta / 3); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)timeDelta >> 2); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 1); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) / 3); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 2); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    // timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };

    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 11); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 11); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 11); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 12); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 12); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 12); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 10); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 10); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 10); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 11)); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 11)); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 11)); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 12)); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 12)); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 12)); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 10)); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 10)); };
    // timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 10)); };

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
    *currentScreenMap = normalScreenMap;
    for (int i = 0; i < 500; i++) randomizeEffectsNaturally();

    effectSettingsStationary = effectSettings;
    effectSettingsMoving = effectSettings;

}

inline Color blendIncorporatingOldMixingMode(Color color1, Color color2)
{
    Color newColor = mixingModeBlendFunction(color1, color2, percentOfEffectBToShow8Bit);
    if (!millisecondsLeftInMixingModeBlend) return newColor;
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
    else effectSettings = effectSettingsMoving;

    pickRandomTimeModesForEffect(&effectA1, movementType != Stationary, movementType == Stationary);
    pickRandomTimeModesForEffect(&effectB1, movementType != Stationary, movementType == Stationary);
    pickRandomTimeModesForEffect(&effectA2, movementType != Stationary, movementType == Stationary);
    pickRandomTimeModesForEffect(&effectB2, movementType != Stationary, movementType == Stationary);
}

// void pickRandomTimeModesForEffect(Effect *effect, bool pickFromMovementBasedTimeModes, bool pickFromNonMovementBasedTimeModes)
// {
//     if (pickFromMovementBasedTimeModes && pickFromNonMovementBasedTimeModes)
//     {
//         effect->timeMode1 = fastRandomInteger(numberOfMovementBasedTimeModes + numberOfTimeModes);
//         effect->timeMode2 = fastRandomInteger(numberOfMovementBasedTimeModes + numberOfTimeModes);
//     }
//     else if (pickFromMovementBasedTimeModes)
//     {
//         effect->timeMode1 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
//         effect->timeMode2 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
//     }
//     else 
//     {
//         effect->timeMode1 = fastRandomInteger(numberOfTimeModes);
//         effect->timeMode2 = fastRandomInteger(numberOfTimeModes);
//     }
// }

const int TimeDeltaResolutionIncreaseFactor = 64;
inline void incrementTime()
{
    int newTime = (int)getTime();
    frameTimeDelta = (newTime - currentTime);
    int timeDelta = (frameTimeDelta * TimeDeltaResolutionIncreaseFactor);
    currentTime = newTime;
    incrementTime(&effectA1, frameTimeDelta);
    incrementTime(&effectB1, frameTimeDelta);
    incrementTime(&effectA2, frameTimeDelta);
    incrementTime(&effectB2, frameTimeDelta);
    #ifdef ENABLE_SERIAL
    Serial.print("ft: ");
    Serial.println(frameTimeDelta);
    #endif
}

// inline void incrementTime(Effect *effect, int timeDelta)
// {
//     int increasedResolutionTimeDelta = (timeDelta * TimeDeltaResolutionIncreaseFactor);
//     effect->time1 = timeModeIncrementFunctions[effect->timeMode1](effect->time1 * TimeDeltaResolutionIncreaseFactor, increasedResolutionTimeDelta) / TimeDeltaResolutionIncreaseFactor;
//     effect->time2 = timeModeIncrementFunctions[effect->timeMode1](effect->time1 * TimeDeltaResolutionIncreaseFactor, increasedResolutionTimeDelta) / TimeDeltaResolutionIncreaseFactor;
// }

inline void incrementTransition()
{
    int incrementAmount = currentTransitionIncrement * frameTimeDelta;
    if (transitionDirection) percentOfEffectBToShow = (UINT16_MAX - percentOfEffectBToShow) > incrementAmount ? percentOfEffectBToShow + incrementAmount : UINT16_MAX;
    else percentOfEffectBToShow = (percentOfEffectBToShow > incrementAmount) ? percentOfEffectBToShow - incrementAmount : 0; 
    percentOfEffectBToShow8Bit = percentOfEffectBToShow >> 8;
}

inline void incrementMixingModeBlend()
{
    if (!millisecondsLeftInMixingModeBlend) return;
    millisecondsLeftInMixingModeBlend = millisecondsLeftInMixingModeBlend > frameTimeDelta ? millisecondsLeftInMixingModeBlend - frameTimeDelta : 0;
    percentOfOldMixingModeToMixIn8Bit = (millisecondsLeftInMixingModeBlend / millisecondsLeftInMixingModeBlendTotalDuration) * UINT8_MAX;
}

inline void incrementTransitionFromSecondaryToPrimaryEffect()
{
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
    millisecondsLeftInMixingModeBlendTotalDuration = fastRandomInteger(effectSettings.MillisecondsForBlendingModeTransitionsMinimum, effectSettings.MillisecondsForBlendingModeTransitionsMaximum);
    millisecondsLeftInMixingModeBlend = millisecondsLeftInMixingModeBlendTotalDuration;
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
        syncEffects(&effectA2, &effectA1);
        syncEffects(&effectB2, &effectB1);

        currentPrimaryEffectA = effectA1;
        currentPrimaryEffectB = effectB1;
        currentSecondaryEffectA = effectA2;
        currentSecondaryEffectB = effectB2;
    }
    else 
    {
        syncEffects(&effectA1, &effectA2);
        syncEffects(&effectB1, &effectB2);

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
    pickRandomTimeModesForAllEffects(&effectA1, &effectB1, &effectA2, &effectB2, effectSettings.LikelihoodMovementBasedTimeModeIsPicked, effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes);
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
    pickRandomGlobalBrightnessControlModesForEffect(&effectA1, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
    pickRandomGlobalBrightnessControlModesForEffect(&effectB1, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
    pickRandomGlobalBrightnessControlModesForEffect(&effectA2, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
    pickRandomGlobalBrightnessControlModesForEffect(&effectB2, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
}

void pickRandomTransitionTime()
{
    int desiredMillisecondTransitionDuration = fastRandomInteger(effectSettings.MillisecondsForBlendingModeTransitionsMinimum, effectSettings.MillisecondsForBlendingModeTransitionsMaximum); 
    currentTransitionIncrement = UINT16_MAX / desiredMillisecondTransitionDuration;
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