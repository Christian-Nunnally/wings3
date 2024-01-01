#include "../commonHeaders.h"
#include "../settings.h"
#include "../Peripherals/microphone.h"
#include "../Peripherals/movementDetection.h"
#include "../Graphics/effectController.h"
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
#include "../IO/tracing.h"
#include "../Observers/stepDectectedObserver.h"
#include "../Observers/movementDetectedObserver.h"
#include "../Utility/fastRandom.h"

inline Color getEffectWithAudioDrivenIntensity(Effect *effect, int index);
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
void pickRandomTransformMaps(Effect *effect, uint8_t likelihood);
void pickRandomMirroredTransformMaps(Effect *effect, uint8_t likelihood);
void swapEffects();
void pickRandomSizeParametersForEffects();
void pickRandomGlobalBrightnessControlModes();
void pickRandomTransitionTime();
void switchTransitionDirection();
void pickRandomAudioLevelThresholdForMoreIntenseEffect();
void handleBeatDetected();
void randomizeEffectsBasedOnElapsedTime();

Effect currentPrimaryEffectA;
Effect currentPrimaryEffectB;
Effect currentSecondaryEffectA;
Effect currentSecondaryEffectB;
EffectSettings effectSettingsStationary;
EffectSettings effectSettingsMoving;

Color ledColorMap[TOTAL_LEDS];

// Transition variables.
uint16_t percentOfEffectBToShow;
uint8_t percentOfEffectBToShow8Bit;
uint8_t percentOfSecondaryEffectToShow;
uint16_t currentTransitionIncrement;
bool transitionDirection;

int effectsRandomizedCount = 0;
int beatCount = 0;
int frameNumber = 0;
int frameTimeDelta;
unsigned long currentTime;
int lastPeakDetectorValue;
int millisecondsLeftInTransitionFromSecondaryToPrimaryEffect;
int millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax = 1;

unsigned long lastTimeEffectChangedDueToTimer;
unsigned long lastTimeFrameIncremented;

Color getLedColorForFrame(int ledIndex)
{
    if (!isMusicDetected())
    {
        uint8_t currentScreen = (*currentScreenMap)[ledIndex];
        if (currentScreen == 4) return {0, 0, 0};
        else if (currentScreen & 1) return currentPrimaryEffectA.effectFunctionHighlight(ledIndex);
        
        Color color1 = currentPrimaryEffectA.effectFunction(ledIndex);
        Color color2 = currentPrimaryEffectB.effectFunction(ledIndex);
        Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2, percentOfEffectBToShow8Bit);
        if (millisecondsLeftInTransitionFromSecondaryToPrimaryEffect)
        {
            Color color3 = currentSecondaryEffectA.effectFunction(ledIndex);
            Color color4 = currentSecondaryEffectB.effectFunction(ledIndex);
            Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4, percentOfEffectBToShow8Bit);
            resultColor1 = blendColorsUsingMixing(resultColor1, resultColor2, percentOfSecondaryEffectToShow);
            ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps);
        } 
        else ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps);
        return ledColorMap[ledIndex];
    }
    uint8_t currentScreen = (*currentScreenMap)[ledIndex];
    if (currentScreen == 4) return {0, 0, 0};
    else if (currentScreen & 1) getEffectWithAudioDrivenIntensity(&currentPrimaryEffectA, ledIndex);
    
    Color color1 = getEffectWithAudioDrivenIntensity(&currentPrimaryEffectA, ledIndex);
    Color color2 = getEffectWithAudioDrivenIntensity(&currentPrimaryEffectB, ledIndex);
    Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2, percentOfEffectBToShow8Bit);
    if (millisecondsLeftInTransitionFromSecondaryToPrimaryEffect)
    {
        Color color3 = getEffectWithAudioDrivenIntensity(&currentSecondaryEffectA, ledIndex);
        Color color4 = getEffectWithAudioDrivenIntensity(&currentSecondaryEffectB, ledIndex);
        Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4, percentOfEffectBToShow8Bit);
        resultColor1 = blendColorsUsingMixing(resultColor1, resultColor2, percentOfSecondaryEffectToShow);
        ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps);
    } 
    else ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps);
    return ledColorMap[ledIndex];
}

inline Color getEffectWithAudioDrivenIntensity(Effect *effect, int index)
{
    return currentAudioIntensityLevel < effect->thresholdToShowMoreIntenseEffect ? effect->effectFunction(index) : effect->effectFunctionHighlight(index);
}

//#include <time.h>
//clock_t start;
void incrementEffectFrame()
{
    incrementBrightnessModeLevels();
    //start = clock(); // Record the start time
    incrementTime();
    //D_emitFloatMetric(METRIC_NAME_ID_RENDER_TIME, ((double)(clock() - start)) / CLOCKS_PER_SEC * 1000);
    incrementTransition();
    incrementMixingModeBlend(frameTimeDelta);
    incrementTransitionFromSecondaryToPrimaryEffect();
    incrementColorPalettesTowardsTargets();
    setAudioIntensityLevel(getAudioIntensityRatio());
    detectBeat();
    randomizeEffectsBasedOnElapsedTime();
    incrementEffects();
    D_emitIntegerMetric(METRIC_NAME_ID_TOTAL_FRAME_TIME, currentTime - lastTimeFrameIncremented);
    lastTimeFrameIncremented = currentTime;
}

void setupEffects()
{
    initializeEffects();
    initializeEffect(&effectA1, &frameTimeDelta, [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectA1); }, 0);
    initializeEffect(&effectB1, &frameTimeDelta, [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectB1); }, 1);
    initializeEffect(&effectA2, &frameTimeDelta, [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectA2); }, 2);
    initializeEffect(&effectB2, &frameTimeDelta, [](int index) { return gradientWaveEffectWithMaxGlobalBrightness(index, &effectB2); }, 3);
    swapEffects();
    setupMixingModes();
    setupTimeModes();
    subscribeToStepDetectedEvent(handleStepDetected);
    subscribeToMovementDetectedEvent(handleMovementDetected);
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
    // Probably doing this in an event is a bad idea.
    MovementType movementType = getCurrentMovementType();
    if (movementType == Stationary) effectSettings = effectSettingsStationary;
    else effectSettings = effectSettingsMoving;

    pickRandomTimeModesForEffect(&effectA1, true, isMusicDetected(), movementType != Stationary, effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes);
    pickRandomTimeModesForEffect(&effectB1, true, isMusicDetected(), movementType != Stationary, effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes);
    pickRandomTimeModesForEffect(&effectA2, true, isMusicDetected(), movementType != Stationary, effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes);
    pickRandomTimeModesForEffect(&effectB2, true, isMusicDetected(), movementType != Stationary, effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes);
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
    uint32_t incrementAmount = currentTransitionIncrement * frameTimeDelta;
    if (transitionDirection) percentOfEffectBToShow = (UINT16_MAX - percentOfEffectBToShow) > incrementAmount ? percentOfEffectBToShow + incrementAmount : UINT16_MAX;
    else percentOfEffectBToShow = (percentOfEffectBToShow > incrementAmount) ? percentOfEffectBToShow - incrementAmount : 0; 
    percentOfEffectBToShow8Bit = percentOfEffectBToShow >> 8;
    D_emitIntegerMetric(METRIC_NAME_ID_PERCENT_OF_EFFECT_B_TO_SHOW, percentOfEffectBToShow8Bit);
}

inline void incrementTransitionFromSecondaryToPrimaryEffect()
{
    if (millisecondsLeftInTransitionFromSecondaryToPrimaryEffect - frameTimeDelta > 0) millisecondsLeftInTransitionFromSecondaryToPrimaryEffect -= frameTimeDelta;
    else millisecondsLeftInTransitionFromSecondaryToPrimaryEffect = 0;
    if (millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax != 0) percentOfSecondaryEffectToShow = (millisecondsLeftInTransitionFromSecondaryToPrimaryEffect / millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax) * 255;
    else percentOfSecondaryEffectToShow = 0;
    D_emitIntegerMetric(METRIC_NAME_ID_MILLISECONDS_TO_TRANSITION_FROM_SECONDARY_TO_PRIMARY_EFFECT, millisecondsLeftInTransitionFromSecondaryToPrimaryEffect);
    D_emitIntegerMetric(METRIC_NAME_ID_PERCENT_SECONDARY_EFFECT_TO_SHOW, percentOfSecondaryEffectToShow);
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
    else return;
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_PALETTE_OFFSET, effect->effectId, effect->currentPaletteOffset);

}

void detectBeat()
{
    const int PositivePeak = 1;
    if (!isMusicDetected()) return;
    int nextPeak = getCurrentPeakDetectorValue();
    if (nextPeak == lastPeakDetectorValue) return;
    D_emitIntegerMetric(METRIC_NAME_ID_PEAK_DETECTOR, nextPeak);
    lastPeakDetectorValue = nextPeak;
    if (nextPeak == PositivePeak) handleBeatDetected();
    else if (!nextPeak && fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero) randomizeEffectsNaturally();
    else if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenAntiBeatDetected) randomizeEffectsNaturally();
}

void randomizeEffectsBasedOnElapsedTime()
{
    if (!effectSettings.RandomizeEffectsAutomaticallyOverTime) return;
    if (currentTime - lastTimeEffectChangedDueToTimer > effectSettings.MillisecondsBetweenTimeBasedAutomaticEffectRandomization)
    {
        lastTimeEffectChangedDueToTimer = currentTime;
        randomizeEffectsNaturally();
    }
}

void handleBeatDetected()
{
    D_emitIntegerMetric(METRIC_NAME_ID_BEAT_COUNT, beatCount++);
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
    if (fastRandomByte() < effectSettings.LikelihoodTimeModesRandomizeWhenRandomizingEffect) pickRandomTimeModesForAllEffects(&effectA1, &effectB1, &effectA2, &effectB2, effectSettings.LikelihoodMovementBasedTimeModeIsPicked, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes, getCurrentMovementType() != Stationary, isMusicDetected());
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenRandomizingEffect) pickRandomEffects();
    if (fastRandomByte() < effectSettings.LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect) pickRandomSizeParametersForEffects();
    if (fastRandomByte() < effectSettings.LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect) pickRandomGlobalBrightnessControlModes();
    if (fastRandomByte() < effectSettings.LikelihoodTransitionDirectionChangesWhenRandomizingEffect) switchTransitionDirection();
    if (fastRandomByte() < effectSettings.LikelihoodBackgroundTransitionTimeChangesWhenRandomizingEffect) pickRandomTransitionTime();
    if (fastRandomByte() < effectSettings.LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect) pickRandomAudioLevelThresholdForMoreIntenseEffect();
    if (fastRandomByte() < effectSettings.LikelihoodScreenMapChangesWhenRandomizingEffect) pickRandomScreenMap();
    D_emitIntegerMetric(METRIC_NAME_ID_RANDOMIZATION_COUNT, effectsRandomizedCount++);
}

void pickRandomScreenMap()
{
    effectSettings.CurrentScreenMapIndex = fastRandomInteger(getScreenMapCount());
    setCurrentScreenMapFromSettings();
    D_emitIntegerMetric(METRIC_NAME_ID_CURRENT_SCREEN_MAP, effectSettings.CurrentScreenMapIndex);
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
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_CURRENT_PALETTE_OFFSET_TARGET, effect->effectId, effect->currentPaletteOffsetTarget);
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
        effectB1.currentPaletteOffset = effectA1.currentPaletteOffset;
        effectA2.currentPaletteOffset = effectA1.currentPaletteOffset;
        effectB2.currentPaletteOffset = effectA1.currentPaletteOffset;
        D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_PALETTE, effectB1.effectId, effectB1.currentPalette);
        D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_PALETTE, effectA2.effectId, effectA2.currentPalette);
        D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_PALETTE, effectB2.effectId, effectB2.currentPalette);
        D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_PALETTE_OFFSET, effectB1.effectId, effectB1.currentPaletteOffset);
        D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_PALETTE_OFFSET, effectA2.effectId, effectA2.currentPaletteOffset);
        D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_PALETTE_OFFSET, effectB2.effectId, effectB2.currentPaletteOffset);
    }
}

void pickRandomPaletteForEffect(Effect *effect)
{
    uint8_t numberOfPossiblePalettesToSwitchTo = effectSettings.AllowedPalettes[effect->currentPalette][0];
    uint8_t indexOfNextPalette = 1 + fastRandomInteger(numberOfPossiblePalettesToSwitchTo);
    uint8_t nextPalette = effectSettings.AllowedPalettes[effect->currentPalette][indexOfNextPalette];
    effect->currentPalette = nextPalette;
    effect->currentPaletteOffset = pickRandomSubPaletteFromPalette(effect->currentPalette);
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_PALETTE, effect->effectId, effect->currentPalette);
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_PALETTE_OFFSET, effect->effectId, effect->currentPaletteOffset);
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

void pickRandomTransformMaps(Effect *effect, uint8_t likelihood)
{
    if (fastRandomByte() < likelihood)
    {
        effect->transformMap1Index = fastRandomInteger(getNormalTransformMapCount());
        effect->isTransformMap1Mirrored = false;
        emitIntegerMetric(METRIC_NAME_ID_USING_MIRRORED_TRANSFORM_MAPS_1, effect->effectId, 0);
        setTransformMap1FromSettings(effect);
    }
    if (fastRandomByte() < likelihood)
    {
        effect->transformMap2Index = fastRandomInteger(getNormalTransformMapCount());
        effect->isTransformMap2Mirrored = false;
        emitIntegerMetric(METRIC_NAME_ID_USING_MIRRORED_TRANSFORM_MAPS_2, effect->effectId, 0);
        setTransformMap2FromSettings(effect);
    }
}

void pickRandomMirroredTransformMaps(Effect *effect, uint8_t likelihood)
{
    if (fastRandomByte() < likelihood)
    {
        effect->transformMap1Index = fastRandomInteger(getMirroredTransformMapCount());
        effect->isTransformMap1Mirrored = true;
        emitIntegerMetric(METRIC_NAME_ID_USING_MIRRORED_TRANSFORM_MAPS_1, effect->effectId, 1);
        setTransformMap1FromSettings(effect);
    }
    if (fastRandomByte() < likelihood)
    { 
        effect->transformMap2Index = fastRandomInteger(getMirroredTransformMapCount());
        effect->isTransformMap2Mirrored = true;
        emitIntegerMetric(METRIC_NAME_ID_USING_MIRRORED_TRANSFORM_MAPS_2, effect->effectId, 1);
        setTransformMap2FromSettings(effect);
    }
}

void swapEffects()
{
    effectSettings.PrimaryEffectToggle = !effectSettings.PrimaryEffectToggle;
    D_emitMetricString(METRIC_NAME_ID_PRIMARY_EFFECT_TOGGLE, effectSettings.PrimaryEffectToggle ? "true" : "false");
    if (effectSettings.PrimaryEffectToggle)
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
        if (effectSettings.PrimaryEffectToggle)
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
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_SIZE, effectA1.effectId, effectA1.size);
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_SIZE, effectB1.effectId, effectB1.size);
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_SIZE, effectA2.effectId, effectA2.size);
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_SIZE, effectB2.effectId, effectB2.size);
}

void pickRandomGlobalBrightnessControlModes()
{
    pickRandomGlobalBrightnessControlModesForEffect(&effectA1, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
    pickRandomGlobalBrightnessControlModesForEffect(&effectB1, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
    pickRandomGlobalBrightnessControlModesForEffect(&effectA2, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
    pickRandomGlobalBrightnessControlModesForEffect(&effectB2, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_BRIGHTNESS_MODE, effectA1.effectId, *effectA1.globalBrightnessPointer);
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_BRIGHTNESS_MODE, effectB1.effectId, *effectB1.globalBrightnessPointer);
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_BRIGHTNESS_MODE, effectA2.effectId, *effectA2.globalBrightnessPointer);
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_BRIGHTNESS_MODE, effectB2.effectId, *effectB2.globalBrightnessPointer);
}

void pickRandomTransitionTime()
{
    int desiredMillisecondTransitionDuration = fastRandomInteger(effectSettings.MillisecondsForBlendingModeTransitionsMinimum, effectSettings.MillisecondsForBlendingModeTransitionsMaximum) + 1; 
    currentTransitionIncrement = UINT16_MAX / desiredMillisecondTransitionDuration;
    D_emitIntegerMetric(METRIC_NAME_ID_DESIRED_MILLISECOND_TRANSITION_DIRECTION, desiredMillisecondTransitionDuration);
}

void switchTransitionDirection()
{
    transitionDirection = !transitionDirection;
    D_emitIntegerMetric(METRIC_NAME_ID_TRANSITION_DIRECTION, transitionDirection);
}

void pickRandomAudioLevelThresholdForMoreIntenseEffect()
{
    effectA1.thresholdToShowMoreIntenseEffect = (float)fastRandomInteger(effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMinimum, effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMaximum) / (float)UINT8_MAX;
    effectA2.thresholdToShowMoreIntenseEffect = effectA1.thresholdToShowMoreIntenseEffect;
    effectB1.thresholdToShowMoreIntenseEffect = (float)fastRandomInteger(effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMinimum, effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMaximum) / (float)UINT8_MAX;
    effectB2.thresholdToShowMoreIntenseEffect = effectB1.thresholdToShowMoreIntenseEffect;
    D_emitFloatMetric(METRIC_NAME_ID_AUDIO_THRESHOLD_TO_SHOW_INTENSE_EFFECT_A1, effectA1.thresholdToShowMoreIntenseEffect);
    D_emitFloatMetric(METRIC_NAME_ID_AUDIO_THRESHOLD_TO_SHOW_INTENSE_EFFECT_B1, effectB1.thresholdToShowMoreIntenseEffect);
}

void enableRandomEffectChangeBasedOnElapsedTime()
{
    effectSettings.RandomizeEffectsAutomaticallyOverTime = true;
}

void disableRandomEffectChangeBasedOnElapsedTime()
{
    effectSettings.RandomizeEffectsAutomaticallyOverTime = false;
}