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
#include "../Graphics/mixingModes.h"
#include "../Graphics/brightnessControlModes.h"
#include "../Graphics/Effects/gradientWaveEffect.h"
#include "../Utility/time.h"
#include "../IO/analogInput.h"
#include "../Observers/stepDectectedObserver.h"
#include "../Observers/movementDetectedObserver.h"
#include "../Utility/fastRandom.h"

inline Color getEffectWithAudioDrivenIntensity(Effect *effect, float threshold, int index);
void handleStepDetected();
void handleMovementDetected();
void incrementTime();
void incrementTransition();
inline void incrementTransitionFromSecondaryToPrimaryEffect();
void incrementColorPalettesTowardsTargets();
void incrementColorPalettesTowardsTargetsForEffect(Effect *effect);
void detectBeat();
void randomizeEffectsNaturally();
void pickRandomScreenMap();
void pickRandomPalette();
void pickRandomPaletteForEffect(Effect *effect);
void pickRandomSubPalette();
void pickRandomSubPaletteForEffect(Effect *effect);
int pickRandomSubPaletteFromPalette(int palette);
void pickRandomTransformMaps();
void pickRandomTransformMaps(Effect *effect, byte likelihood);
void pickRandomMirroredTransformMaps(Effect *effect, byte likelihood);
void swapEffects();
void pickRandomSizeParametersForEffects();
void pickRandomGlobalBrightnessControlModes();
void pickRandomTransitionTime();
void switchTransitionDirection();
void pickRandomAudioLevelThresholdForMoreIntenseEffect();
void handleBeatDetected();

Effect currentPrimaryEffectA;
Effect currentPrimaryEffectB;
Effect currentSecondaryEffectA;
Effect currentSecondaryEffectB;
EffectSettings effectSettingsStationary;
EffectSettings effectSettingsMoving;

// Screen Mode variables.
byte *currentScreenMap[TOTAL_LEDS];
Color ledColorMap[TOTAL_LEDS];

// Transition variables.
uint16_t percentOfEffectBToShow;
uint8_t percentOfEffectBToShow8Bit;
uint8_t percentOfSecondaryEffectToShow;
int currentTransitionIncrement;
bool transitionDirection;

int frameTimeDelta;
int currentTime;
int lastPeakDetectorValue;
int millisecondsLeftInTransitionFromSecondaryToPrimaryEffect;
int millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax = 1;
bool primaryEffectToggle;
float effectA1AudioLevelThresholdToShowMoreIntenseEffect = .98;
float effectB1AudioLevelThresholdToShowMoreIntenseEffect = .6;

Color getLedColorForFrame(int ledIndex)
{
    byte currentScreen = (*currentScreenMap)[ledIndex];
    if (currentScreen == 4) return {0, 0, 0};
    else if (currentScreen & 1) getEffectWithAudioDrivenIntensity(&currentPrimaryEffectA, effectA1AudioLevelThresholdToShowMoreIntenseEffect, ledIndex);
    
    Color color1 = getEffectWithAudioDrivenIntensity(&currentPrimaryEffectA, effectA1AudioLevelThresholdToShowMoreIntenseEffect, ledIndex);
    Color color2 = getEffectWithAudioDrivenIntensity(&currentPrimaryEffectB, effectB1AudioLevelThresholdToShowMoreIntenseEffect, ledIndex);
    Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2, percentOfEffectBToShow8Bit);
    if (millisecondsLeftInTransitionFromSecondaryToPrimaryEffect)
    {
        Color color3 = getEffectWithAudioDrivenIntensity(&currentSecondaryEffectA, effectA1AudioLevelThresholdToShowMoreIntenseEffect, ledIndex);
        Color color4 = getEffectWithAudioDrivenIntensity(&currentSecondaryEffectB, effectB1AudioLevelThresholdToShowMoreIntenseEffect, ledIndex);
        Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4, percentOfEffectBToShow8Bit);
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
    incrementMixingModeBlend(frameTimeDelta);
    incrementTransitionFromSecondaryToPrimaryEffect();
    incrementColorPalettesTowardsTargets();
    setAudioIntensityLevel(getAudioIntensityRatio());
    detectBeat();
    incrementEffects();
}

void setupEffects()
{
    initializeEffect(&effectA1, &frameTimeDelta, [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectA1); });
    initializeEffect(&effectB1, &frameTimeDelta, [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectB1); });
    initializeEffect(&effectA2, &frameTimeDelta, [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectA2); });
    initializeEffect(&effectB2, &frameTimeDelta, [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectB2); });
    swapEffects();
    setupMixingModes();
    setupTimeModes();
    subscribeToStepDetectedEvent(handleStepDetected);
    subscribeToMovementDetectedEvent(handleMovementDetected);
    *currentScreenMap = normalScreenMap;
    effectSettingsStationary = effectSettings;
    effectSettingsMoving = effectSettings;
    setupNormalMood(&effectSettings);
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

inline void incrementTime()
{
    int newTime = (int)getTime();
    frameTimeDelta = newTime - currentTime;
    currentTime = newTime;
    incrementTimesForEffect(&effectA1, frameTimeDelta);
    incrementTimesForEffect(&effectB1, frameTimeDelta);
    incrementTimesForEffect(&effectA2, frameTimeDelta);
    incrementTimesForEffect(&effectB2, frameTimeDelta);
}

inline void incrementTransition()
{
    int incrementAmount = currentTransitionIncrement * frameTimeDelta;
    if (transitionDirection) percentOfEffectBToShow = (UINT16_MAX - percentOfEffectBToShow) > incrementAmount ? percentOfEffectBToShow + incrementAmount : UINT16_MAX;
    else percentOfEffectBToShow = (percentOfEffectBToShow > incrementAmount) ? percentOfEffectBToShow - incrementAmount : 0; 
    percentOfEffectBToShow8Bit = percentOfEffectBToShow >> 8;
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
    if (nextPeak == PositivePeak) handleBeatDetected();
    else if (!nextPeak && fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero) randomizeEffectsNaturally();
    else if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenAntiBeatDetected) randomizeEffectsNaturally();
}

void handleBeatDetected()
{
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetected) randomizeEffectsNaturally();
    effectA1.effectFunctionBonusTrigger();
    if (effectA1.effectFunctionIncrementUniqueId != effectB1.effectFunctionIncrementUniqueId) effectB1.effectFunctionBonusTrigger();
}

void randomizeEffectsNaturally()
{
    if (fastRandomByte() < effectSettings.LikelihoodNothingChangesWhenRandomizingEffect) return;
    if (fastRandomByte() < effectSettings.LikelihoodBlendingModeChangesWhenRandomizingEffect) pickRandomMixingMode(effectSettings.MillisecondsForBlendingModeTransitionsMinimum, effectSettings.MillisecondsForBlendingModeTransitionsMaximum);
    if (fastRandomByte() < effectSettings.LikelihoodSubPaletteChangesWhenRandomizingEffect) pickRandomSubPalette();
    if (fastRandomByte() < effectSettings.LikelihoodPaletteChangesWhenRandomizingEffect) pickRandomPalette();
    if (fastRandomByte() < effectSettings.LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect) pickRandomTransformMaps();
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreSwappedWhenRandomizingEffect) swapEffects();
    if (fastRandomByte() < effectSettings.LikelihoodTimeModesRandomizeWhenRandomizingEffect) pickRandomTimeModesForAllEffects(&effectA1, &effectB1, &effectA2, &effectB2, effectSettings.LikelihoodMovementBasedTimeModeIsPicked, effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes);
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenRandomizingEffect) pickRandomEffects();
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

    if (fastRandomByte() < effectSettings.LikelihoodTransformMapsAreSwitchedWhenEffectsAreSwapped)
    {
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
    }
    millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax = fastRandomInteger(effectSettings.MillisecondsForEffectTransitionsMinimum, effectSettings.MillisecondsForEffectTransitionsMaximum);
    millisecondsLeftInTransitionFromSecondaryToPrimaryEffect = millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax;
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
    int desiredMillisecondTransitionDuration = fastRandomInteger(effectSettings.MillisecondsForBlendingModeTransitionsMinimum, effectSettings.MillisecondsForBlendingModeTransitionsMaximum) + 1; 
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