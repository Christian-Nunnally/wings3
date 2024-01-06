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
void setAppropriatePrimaryAndSecondaryEffect();

Effect currentPrimaryEffectA;
Effect currentPrimaryEffectB;
Effect currentSecondaryEffectA;
Effect currentSecondaryEffectB;
Color ledColorMap[TOTAL_LEDS];
unsigned long currentTime;

int lastPeakDetectorValue;
uint8_t percentOfEffectBToShow8Bit;

unsigned long lastTimeEffectChangedDueToTimer;
unsigned long lastTimeFrameIncremented;

int effectsRandomizedCount = 0;
int beatCount = 0;
int frameNumber = 0;
int frameTimeDelta;

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
        if (effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffect)
        {
            Color color3 = currentSecondaryEffectA.effectFunction(ledIndex);
            Color color4 = currentSecondaryEffectB.effectFunction(ledIndex);
            Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4, percentOfEffectBToShow8Bit);
            resultColor1 = blendColorsUsingMixing(resultColor1, resultColor2, effectSettings.percentOfSecondaryEffectToShow);
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
    if (effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffect)
    {
        Color color3 = getEffectWithAudioDrivenIntensity(&currentSecondaryEffectA, ledIndex);
        Color color4 = getEffectWithAudioDrivenIntensity(&currentSecondaryEffectB, ledIndex);
        Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4, percentOfEffectBToShow8Bit);
        resultColor1 = blendColorsUsingMixing(resultColor1, resultColor2, effectSettings.percentOfSecondaryEffectToShow);
        ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps);
    } 
    else ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps * currentAudioIntensityLevel);
    return ledColorMap[ledIndex];
}

inline Color getEffectWithAudioDrivenIntensity(Effect *effect, int index)
{
    return currentAudioIntensityLevel < effect->thresholdToShowMoreIntenseEffect ? effect->effectFunction(index) : effect->effectFunctionHighlight(index);
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
    randomizeEffectsBasedOnElapsedTime();
    incrementEffects();
    if (doEffectsNeedToRefresh())
    {
        setAppropriatePrimaryAndSecondaryEffect();
    }
    D_emitMetric(METRIC_NAME_ID_TOTAL_FRAME_TIME, (int)(currentTime - lastTimeFrameIncremented));
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
    setupNormalMood(&effectSettings);
}

void handleStepDetected()
{
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenStepIsDetected) randomizeEffectsNaturally();
}

void handleMovementDetected()
{
    MovementType movementType = getCurrentMovementType();

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
    uint32_t incrementAmount = effectSettings.currentTransitionIncrement * frameTimeDelta;
    if (effectSettings.transitionDirection) effectSettings.percentOfEffectBToShow = (UINT16_MAX - effectSettings.percentOfEffectBToShow) > incrementAmount ? effectSettings.percentOfEffectBToShow + incrementAmount : UINT16_MAX;
    else effectSettings.percentOfEffectBToShow = (effectSettings.percentOfEffectBToShow > incrementAmount) ? effectSettings.percentOfEffectBToShow - incrementAmount : 0; 
    percentOfEffectBToShow8Bit = effectSettings.percentOfEffectBToShow >> 8;
    D_emitMetric(METRIC_NAME_ID_PERCENT_OF_EFFECT_B_TO_SHOW, percentOfEffectBToShow8Bit);
}

inline void incrementTransitionFromSecondaryToPrimaryEffect()
{
    if (effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffect - frameTimeDelta > 0) effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffect -= frameTimeDelta;
    else effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffect = 0;
    if (effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax != 0) effectSettings.percentOfSecondaryEffectToShow = (effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffect / effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax) * 255;
    else effectSettings.percentOfSecondaryEffectToShow = 0;
    D_emitMetric(METRIC_NAME_ID_MILLISECONDS_TO_TRANSITION_FROM_SECONDARY_TO_PRIMARY_EFFECT, effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffect);
    D_emitMetric(METRIC_NAME_ID_PERCENT_SECONDARY_EFFECT_TO_SHOW, effectSettings.percentOfSecondaryEffectToShow);
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
    D_emitMetric(METRIC_NAME_ID_EFFECT_PALETTE_OFFSET, effect->currentPaletteOffset);
}

void detectBeat()
{
    const int PositivePeak = 1;
    if (!isMusicDetected()) return;
    int nextPeak = getCurrentPeakDetectorValue();
    if (nextPeak == lastPeakDetectorValue) return;
    D_emitMetric(METRIC_NAME_ID_PEAK_DETECTOR, nextPeak);
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
    D_emitMetric(METRIC_NAME_ID_BEAT_COUNT, beatCount++);
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
    D_emitMetric(METRIC_NAME_ID_RANDOMIZATION_COUNT, effectsRandomizedCount++);
}

void pickRandomScreenMap()
{
    effectSettings.CurrentScreenMapIndex = fastRandomInteger(getScreenMapCount());
    setCurrentScreenMapFromSettings();
    D_emitMetric(METRIC_NAME_ID_CURRENT_SCREEN_MAP, effectSettings.CurrentScreenMapIndex);
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
    D_emitMetric(METRIC_NAME_ID_EFFECT_CURRENT_PALETTE_OFFSET_TARGET, effect->currentPaletteOffsetTarget);
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
        D_emitMetric(METRIC_NAME_ID_EFFECT_PALETTE, effectB1.currentPalette);
        D_emitMetric(METRIC_NAME_ID_EFFECT_PALETTE, effectA2.currentPalette);
        D_emitMetric(METRIC_NAME_ID_EFFECT_PALETTE, effectB2.currentPalette);
        D_emitMetric(METRIC_NAME_ID_EFFECT_PALETTE_OFFSET, effectB1.currentPaletteOffset);
        D_emitMetric(METRIC_NAME_ID_EFFECT_PALETTE_OFFSET, effectA2.currentPaletteOffset);
        D_emitMetric(METRIC_NAME_ID_EFFECT_PALETTE_OFFSET, effectB2.currentPaletteOffset);
    }
}

void pickRandomPaletteForEffect(Effect *effect)
{
    uint8_t numberOfPossiblePalettesToSwitchTo = effectSettings.AllowedPalettes[effect->currentPalette][0];
    uint8_t indexOfNextPalette = 1 + fastRandomInteger(numberOfPossiblePalettesToSwitchTo);
    uint8_t nextPalette = effectSettings.AllowedPalettes[effect->currentPalette][indexOfNextPalette];
    effect->currentPalette = nextPalette;
    effect->currentPaletteOffset = pickRandomSubPaletteFromPalette(effect->currentPalette);
    D_emitMetric(METRIC_NAME_ID_EFFECT_PALETTE, effect->currentPalette);
    D_emitMetric(METRIC_NAME_ID_EFFECT_PALETTE_OFFSET, effect->currentPaletteOffset);
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
        D_emitMetric(METRIC_NAME_ID_USING_MIRRORED_TRANSFORM_MAPS_1, 0);
        setTransformMap1FromSettings(effect);
    }
    if (fastRandomByte() < likelihood)
    {
        effect->transformMap2Index = fastRandomInteger(getNormalTransformMapCount());
        effect->isTransformMap2Mirrored = false;
        D_emitMetric(METRIC_NAME_ID_USING_MIRRORED_TRANSFORM_MAPS_2, 0);
        setTransformMap2FromSettings(effect);
    }
}

void pickRandomMirroredTransformMaps(Effect *effect, uint8_t likelihood)
{
    if (fastRandomByte() < likelihood)
    {
        effect->transformMap1Index = fastRandomInteger(getMirroredTransformMapCount());
        effect->isTransformMap1Mirrored = true;
        D_emitMetric(METRIC_NAME_ID_USING_MIRRORED_TRANSFORM_MAPS_1, 1);
        setTransformMap1FromSettings(effect);
    }
    if (fastRandomByte() < likelihood)
    { 
        effect->transformMap2Index = fastRandomInteger(getMirroredTransformMapCount());
        effect->isTransformMap2Mirrored = true;
        D_emitMetric(METRIC_NAME_ID_USING_MIRRORED_TRANSFORM_MAPS_2, 1);
        setTransformMap2FromSettings(effect);
    }
}

void swapEffects()
{
    effectSettings.PrimaryEffectToggle = !effectSettings.PrimaryEffectToggle;
    setAppropriatePrimaryAndSecondaryEffect();
    D_emitMetric(METRIC_NAME_ID_PRIMARY_EFFECT_TOGGLE, effectSettings.PrimaryEffectToggle);

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
    effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax = fastRandomInteger(effectSettings.MillisecondsForEffectTransitionsMinimum, effectSettings.MillisecondsForEffectTransitionsMaximum);
    effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffect = effectSettings.millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax;
}

void setAppropriatePrimaryAndSecondaryEffect()
{
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
}

void pickRandomSizeParametersForEffects()
{
    effectA1.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
    effectB1.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
    effectA2.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
    effectB2.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
    D_emitMetric(METRIC_NAME_ID_EFFECT_SIZE, effectA1.size);
    D_emitMetric(METRIC_NAME_ID_EFFECT_SIZE, effectB1.size);
    D_emitMetric(METRIC_NAME_ID_EFFECT_SIZE, effectA2.size);
    D_emitMetric(METRIC_NAME_ID_EFFECT_SIZE, effectB2.size);
}

void pickRandomGlobalBrightnessControlModes()
{
    pickRandomGlobalBrightnessControlModesForEffect(&effectA1, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
    pickRandomGlobalBrightnessControlModesForEffect(&effectB1, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
    pickRandomGlobalBrightnessControlModesForEffect(&effectA2, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
    pickRandomGlobalBrightnessControlModesForEffect(&effectB2, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange, effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked, effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked);
    D_emitMetric(METRIC_NAME_ID_EFFECT_BRIGHTNESS_MODE, *effectA1.globalBrightnessPointer);
    D_emitMetric(METRIC_NAME_ID_EFFECT_BRIGHTNESS_MODE, *effectB1.globalBrightnessPointer);
    D_emitMetric(METRIC_NAME_ID_EFFECT_BRIGHTNESS_MODE, *effectA2.globalBrightnessPointer);
    D_emitMetric(METRIC_NAME_ID_EFFECT_BRIGHTNESS_MODE, *effectB2.globalBrightnessPointer);
}

void pickRandomTransitionTime()
{
    int desiredMillisecondTransitionDuration = fastRandomInteger(effectSettings.MillisecondsForBlendingModeTransitionsMinimum, effectSettings.MillisecondsForBlendingModeTransitionsMaximum) + 1; 
    effectSettings.currentTransitionIncrement = UINT16_MAX / desiredMillisecondTransitionDuration;
    D_emitMetric(METRIC_NAME_ID_DESIRED_MILLISECOND_TRANSITION_DIRECTION, desiredMillisecondTransitionDuration);
}

void switchTransitionDirection()
{
    effectSettings.transitionDirection = !effectSettings.transitionDirection;
    D_emitMetric(METRIC_NAME_ID_TRANSITION_DIRECTION, effectSettings.transitionDirection);
}

void pickRandomAudioLevelThresholdForMoreIntenseEffect()
{
    effectA1.thresholdToShowMoreIntenseEffect = (float)fastRandomInteger(effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMinimum, effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMaximum) / (float)UINT8_MAX;
    effectA2.thresholdToShowMoreIntenseEffect = effectA1.thresholdToShowMoreIntenseEffect;
    effectB1.thresholdToShowMoreIntenseEffect = (float)fastRandomInteger(effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMinimum, effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMaximum) / (float)UINT8_MAX;
    effectB2.thresholdToShowMoreIntenseEffect = effectB1.thresholdToShowMoreIntenseEffect;
    D_emitMetric(METRIC_NAME_ID_AUDIO_THRESHOLD_TO_SHOW_INTENSE_EFFECT_A1, effectA1.thresholdToShowMoreIntenseEffect);
    D_emitMetric(METRIC_NAME_ID_AUDIO_THRESHOLD_TO_SHOW_INTENSE_EFFECT_B1, effectB1.thresholdToShowMoreIntenseEffect);
}