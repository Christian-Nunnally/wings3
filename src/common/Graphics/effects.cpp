#include "../settings.h"
#include "../IO/tracing.h"
#include <stdio.h>
#include <string.h>
#include "../Graphics/brightnessControlModes.h"
#include "../Graphics/effects.h"
#include "../Graphics/Effects/solidColorFillEffect.h"
#include "../Graphics/Effects/lightChaseEffect.h"
#include "../Graphics/Effects/fireworksEffect.h"
#include "../Graphics/Effects/fireEffect.h"
#include "../Graphics/Effects/expandingColorOrbEffect.h"
#include "../Graphics/Effects/lightningBugEffect.h"
#include "../Graphics/Effects/rainShowerEffect.h"
#include "../Graphics/Effects/meteorRainEffect.h"
#include "../Graphics/Effects/gradientWaveEffect.h"
#include "../Graphics/Effects/starFieldEffect.h"
#include "../Graphics/savedEffectsSettings.h"
#include "../Graphics/normalTransformMaps.h"
#include "../Graphics/mirroredTransformMaps.h"  
#include "../Graphics/screenMaps.h"
#include "../Utility/fastRandom.h"

int getRandomEffectNumberFromAllowedEffects();
void setEffectA(uint8_t effectIndex);
void setEffectB(uint8_t effectIndex);
void saveSingleEffect(Effect* fromEffect, SavedEffect* savedEffect);
void loadSingleEffect(SavedEffect* savedEffect, Effect* toEffect);

Effect effectA1;
Effect effectB1;
Effect effectA2;
Effect effectB2;

EffectSettings effectSettings;

float currentAudioIntensityLevel;

uint8_t *currentScreenMap[TOTAL_LEDS];

void initializeEffects()
{
    *currentScreenMap = normalScreenMap;
}

void setAudioIntensityLevel(float level)
{
    currentAudioIntensityLevel = level;
}

int getRandomEffectNumberFromAllowedEffects()
{
    int effectRandom = fastRandomInteger(NumberOfAllowedEffectsToPickBetween);
    return effectSettings.AllowedEffects[effectRandom];
}

void syncEffects(Effect *syncFrom, Effect *syncTo)
{
    syncTo->transformMap1Index = syncFrom->transformMap1Index;
    syncTo->transformMap2Index = syncFrom->transformMap2Index;
    *syncTo->transformMap1 = *syncFrom->transformMap1;
    *syncTo->transformMap2 = *syncFrom->transformMap2;
    *syncTo->globalBrightnessPointer = *syncFrom->globalBrightnessPointer;
    *syncTo->frameTimeDelta = *syncFrom->frameTimeDelta;
    syncTo->time1 = syncFrom->time1;
    syncTo->time2 = syncFrom->time2;
    syncTo->timeMode1 = syncFrom->timeMode1;
    syncTo->timeMode2 = syncFrom->timeMode2;
    syncTo->size = syncFrom->size;
    syncTo->currentPalette = syncFrom->currentPalette;
    syncTo->currentPaletteOffset = syncFrom->currentPaletteOffset;
    syncTo->currentPaletteOffsetTarget = syncFrom->currentPaletteOffsetTarget;
}

void incrementEffects()
{
    effectA1.effectFunctionIncrement();
    if (effectA1.effectFunctionIncrementUniqueId != effectB1.effectFunctionIncrementUniqueId) effectB1.effectFunctionIncrement();
}

/// ---------------------------------------------------------------------------------------------------------------

void setEffectAToMeteorRainEffect()
{
    effectA1.effectFunction = [](int index) { return meteorRainEffect(index, &effectA1, .12); };
    effectA2.effectFunction = [](int index) { return meteorRainEffect(index, &effectA2, .12); };
    effectA1.effectFunctionIncrement = []() { };
    effectA2.effectFunctionIncrement = []() { };
    effectA1.effectFunctionIncrementUniqueId = METEOR_RAIN_EFFECT_INDEX;
    effectA2.effectFunctionIncrementUniqueId = METEOR_RAIN_EFFECT_INDEX;
}

void setEffectAToMeteorRainEffect2()
{
    effectA1.effectFunction = [](int index) { return meteorRainEffect2(index, &effectA1, .12); };
    effectA2.effectFunction = [](int index) { return meteorRainEffect2(index, &effectA2, .12); };
    effectA1.effectFunctionIncrement = []() { };
    effectA2.effectFunctionIncrement = []() { };
    effectA1.effectFunctionIncrementUniqueId = METEOR_RAIN_EFFECT_2_INDEX;
    effectA2.effectFunctionIncrementUniqueId = METEOR_RAIN_EFFECT_2_INDEX;
}

void setEffectAToStarFieldEffect()
{
    effectA1.effectFunction = [](int index) { return starFieldEffect(index, &effectA1, 20); };
    effectA2.effectFunction = [](int index) { return starFieldEffect(index, &effectA2, 20); };
    effectA1.effectFunctionIncrement = []() { incrementStarFieldEffect(&effectA1, 20, 500); };
    effectA2.effectFunctionIncrement = []() { incrementStarFieldEffect(&effectA1, 20, 500); };
    effectA1.effectFunctionIncrementUniqueId = STAR_FIELD_EFFECT_INDEX;
    effectA2.effectFunctionIncrementUniqueId = STAR_FIELD_EFFECT_INDEX;
}

void setEffectAToGradientWaveEffect()
{
    effectA1.effectFunction = [](int index) { return gradientWaveEffect(index, &effectA1); };
    effectA2.effectFunction = [](int index) { return gradientWaveEffect(index, &effectA2); };
    effectA1.effectFunctionIncrement = []() { };
    effectA2.effectFunctionIncrement = []() { };
    effectA1.effectFunctionIncrementUniqueId = GRADIENT_WAVE_EFFECT_INDEX;
    effectA2.effectFunctionIncrementUniqueId = GRADIENT_WAVE_EFFECT_INDEX;
}

void setEffectAToRainShowerEffect()
{
    effectA1.effectFunction = [](int index) { return rainShowerEffect(index, &effectA1); };
    effectA2.effectFunction = [](int index) { return rainShowerEffect(index, &effectA2); };
    effectA1.effectFunctionIncrement = []() { };
    effectA2.effectFunctionIncrement = []() { };
    effectA1.effectFunctionIncrementUniqueId = RAIN_SHOWER_EFFECT_INDEX;
    effectA2.effectFunctionIncrementUniqueId = RAIN_SHOWER_EFFECT_INDEX;
}

void setEffectAToExpandingColorOrbEffect()
{
    effectA1.effectFunction = [](int index) { return expandingColorOrbEffect(index, &effectA1); };
    effectA2.effectFunction = [](int index) { return expandingColorOrbEffect(index, &effectA2); };
    effectA1.effectFunctionIncrement = []() { incrementExpandingColorOrbEffect(&effectA1); };
    effectA2.effectFunctionIncrement = []() { incrementExpandingColorOrbEffect(&effectA1); };
    effectA1.effectFunctionIncrementUniqueId = EXPANDING_COLOR_ORB_EFFECT_INDEX;
    effectA2.effectFunctionIncrementUniqueId = EXPANDING_COLOR_ORB_EFFECT_INDEX;
}

void setEffectAToLightningBugEffect()
{
    effectA1.effectFunction = [](int index) { return lightningBugEffect(index, &effectA1); };
    effectA2.effectFunction = [](int index) { return lightningBugEffect(index, &effectA2); };
    effectA1.effectFunctionIncrement = []() { incrementLightningBugEffect(&effectA1); };
    effectA2.effectFunctionIncrement = []() { incrementLightningBugEffect(&effectA1); };
    effectA1.effectFunctionIncrementUniqueId = LIGHTNING_BUG_EFFECT_INDEX;
    effectA2.effectFunctionIncrementUniqueId = LIGHTNING_BUG_EFFECT_INDEX;
}

void setEffectAToFireEffect()
{
    effectA1.effectFunction = [](int index) { return fireEffect(index, &effectA1, 100 - (currentAudioIntensityLevel * 100)); };
    effectA2.effectFunction = [](int index) { return fireEffect(index, &effectA2, 100 - (currentAudioIntensityLevel * 100)); };
    effectA1.effectFunctionIncrement = []() { incrementFireEffect(&effectA1, 400 * currentAudioIntensityLevel * currentAudioIntensityLevel); };
    effectA2.effectFunctionIncrement = []() { incrementFireEffect(&effectA1, 400 * currentAudioIntensityLevel * currentAudioIntensityLevel); };
    effectA1.effectFunctionIncrementUniqueId = FIRE_EFFECT_INDEX;
    effectA2.effectFunctionIncrementUniqueId = FIRE_EFFECT_INDEX;
}

void setEffectAToSimpleSolidColorFillEffect()
{
    effectA1.effectFunction = [](int index) { return simpleSolidColorFillEffect(index, &effectA1); };
    effectA2.effectFunction = [](int index) { return simpleSolidColorFillEffect(index, &effectA2); };
    effectA1.effectFunctionIncrement = []() { };
    effectA2.effectFunctionIncrement = []() { };
    effectA1.effectFunctionIncrementUniqueId = SIMPLE_SOLID_COLOR_FILL_EFFECT_INDEX;
    effectA2.effectFunctionIncrementUniqueId = SIMPLE_SOLID_COLOR_FILL_EFFECT_INDEX;
}

void setEffectAToFireworksEffect()
{
    effectA1.effectFunction = [](int index) { return fireworksEffect(index, &effectA1); };
    effectA2.effectFunction = [](int index) { return fireworksEffect(index, &effectA2); };
    effectA1.effectFunctionIncrement = []() { incrementFireworksEffect(&effectA1, 40); };
    effectA2.effectFunctionIncrement = []() { incrementFireworksEffect(&effectA2, 40); };
    effectA1.effectFunctionIncrementUniqueId = FIREWORKS_EFFECT_INDEX;
    effectA2.effectFunctionIncrementUniqueId = FIREWORKS_EFFECT_INDEX;
    effectA1.effectFunctionBonusTrigger = triggerFirework;
    effectA2.effectFunctionBonusTrigger = triggerFirework;
}

void setEffectAToLightChaseEffect()
{
    effectA1.effectFunction = [](int index) { return lightChaseEffect(index, &effectA1, 12); };
    effectA2.effectFunction = [](int index) { return lightChaseEffect(index, &effectA2, 7); };
    effectA1.effectFunctionIncrement = []() { };
    effectA2.effectFunctionIncrement = []() { };
    effectA1.effectFunctionIncrementUniqueId = LIGHT_CHASE_EFFECT_INDEX;
    effectA2.effectFunctionIncrementUniqueId = LIGHT_CHASE_EFFECT_INDEX;
}

/// ---------------------------------------------------------------------------------------------------------------

void setEffectBToMeteorRainEffect()
{
    effectB1.effectFunction = [](int index) { return meteorRainEffect(index, &effectB1, .12); };
    effectB2.effectFunction = [](int index) { return meteorRainEffect(index, &effectB2, .12); };
    effectB1.effectFunctionIncrement = []() { };
    effectB2.effectFunctionIncrement = []() { };
    effectB1.effectFunctionIncrementUniqueId = METEOR_RAIN_EFFECT_INDEX;
    effectB2.effectFunctionIncrementUniqueId = METEOR_RAIN_EFFECT_INDEX;
}

void setEffectBToMeteorRainEffect2()
{
    effectB1.effectFunction = [](int index) { return meteorRainEffect2(index, &effectB1, .12); };
    effectB2.effectFunction = [](int index) { return meteorRainEffect2(index, &effectB2, .12); };
    effectB1.effectFunctionIncrement = []() { };
    effectB2.effectFunctionIncrement = []() { };
    effectB1.effectFunctionIncrementUniqueId = METEOR_RAIN_EFFECT_2_INDEX;
    effectB2.effectFunctionIncrementUniqueId = METEOR_RAIN_EFFECT_2_INDEX;
}

void setEffectBToStarFieldEffect()
{
    effectB1.effectFunction = [](int index) { return starFieldEffect(index, &effectB1, 15); };
    effectB2.effectFunction = [](int index) { return starFieldEffect(index, &effectB2, 20); };
    effectB1.effectFunctionIncrement = []() { incrementStarFieldEffect(&effectB1, 15, 400); };
    effectB2.effectFunctionIncrement = []() { incrementStarFieldEffect(&effectB1, 20, 300); };
    effectB1.effectFunctionIncrementUniqueId = STAR_FIELD_EFFECT_INDEX;
    effectB2.effectFunctionIncrementUniqueId = STAR_FIELD_EFFECT_INDEX;
}

void setEffectBToGradientWaveEffect()
{
    effectB1.effectFunction = [](int index) { return gradientWaveEffect(index, &effectB1); };
    effectB2.effectFunction = [](int index) { return gradientWaveEffect(index, &effectB2); };
    effectB1.effectFunctionIncrement = []() { };
    effectB2.effectFunctionIncrement = []() { };
    effectB1.effectFunctionIncrementUniqueId = GRADIENT_WAVE_EFFECT_INDEX;
    effectB2.effectFunctionIncrementUniqueId = GRADIENT_WAVE_EFFECT_INDEX;
}

void setEffectBToRainShowerEffect()
{
    effectB1.effectFunction = [](int index) { return rainShowerEffect(index, &effectB1); };
    effectB2.effectFunction = [](int index) { return rainShowerEffect(index, &effectB2); };
    effectB1.effectFunctionIncrement = []() { };
    effectB2.effectFunctionIncrement = []() { };
    effectB1.effectFunctionIncrementUniqueId = RAIN_SHOWER_EFFECT_INDEX;
    effectB2.effectFunctionIncrementUniqueId = RAIN_SHOWER_EFFECT_INDEX;
}

void setEffectBToExpandingColorOrbEffect()
{
    effectB1.effectFunction = [](int index) { return expandingColorOrbEffect(index, &effectB1); };
    effectB2.effectFunction = [](int index) { return expandingColorOrbEffect(index, &effectB2); };
    effectB1.effectFunctionIncrement = []() { incrementExpandingColorOrbEffect(&effectB1); };
    effectB2.effectFunctionIncrement = []() { incrementExpandingColorOrbEffect(&effectB1); };
    effectB1.effectFunctionIncrementUniqueId = EXPANDING_COLOR_ORB_EFFECT_INDEX;
    effectB2.effectFunctionIncrementUniqueId = EXPANDING_COLOR_ORB_EFFECT_INDEX;
}

void setEffectBToLightningBugEffect()
{
    effectB1.effectFunction = [](int index) { return lightningBugEffect(index, &effectB1); };
    effectB2.effectFunction = [](int index) { return lightningBugEffect(index, &effectB2); };
    effectB1.effectFunctionIncrement = []() { incrementLightningBugEffect(&effectB1); };
    effectB2.effectFunctionIncrement = []() { incrementLightningBugEffect(&effectB1); };
    effectB1.effectFunctionIncrementUniqueId = LIGHTNING_BUG_EFFECT_INDEX;
    effectB2.effectFunctionIncrementUniqueId = LIGHTNING_BUG_EFFECT_INDEX;
}

void setEffectBToFireEffect()
{
    effectB1.effectFunction = [](int index) { return fireEffect(index, &effectB1, 100 - (currentAudioIntensityLevel * 100)); };
    effectB2.effectFunction = [](int index) { return fireEffect(index, &effectB2, 100 - (currentAudioIntensityLevel * 100)); };
    effectB1.effectFunctionIncrement = []() { incrementFireEffect(&effectB1, 500 * currentAudioIntensityLevel * currentAudioIntensityLevel); };
    effectB2.effectFunctionIncrement = []() { incrementFireEffect(&effectB1, 500 * currentAudioIntensityLevel * currentAudioIntensityLevel); };
    effectB1.effectFunctionIncrementUniqueId = FIRE_EFFECT_INDEX;
    effectB2.effectFunctionIncrementUniqueId = FIRE_EFFECT_INDEX;
}

void setEffectBToSimpleSolidColorFillEffect()
{
    effectB1.effectFunction = [](int index) { return simpleSolidColorFillEffect(index, &effectB1); };
    effectB2.effectFunction = [](int index) { return simpleSolidColorFillEffect(index, &effectB2); };
    effectB1.effectFunctionIncrement = []() { };
    effectB2.effectFunctionIncrement = []() { };
    effectB1.effectFunctionIncrementUniqueId = SIMPLE_SOLID_COLOR_FILL_EFFECT_INDEX;
    effectB2.effectFunctionIncrementUniqueId = SIMPLE_SOLID_COLOR_FILL_EFFECT_INDEX;
}

void setEffectBToFireworksEffect()
{
    effectB1.effectFunction = [](int index) { return fireworksEffect(index, &effectB1); };
    effectB2.effectFunction = [](int index) { return fireworksEffect(index, &effectB2); };
    effectB1.effectFunctionIncrement = []() { incrementFireworksEffect(&effectB1, 40); };
    effectB2.effectFunctionIncrement = []() { incrementFireworksEffect(&effectB2, 45); };
    effectB1.effectFunctionIncrementUniqueId = FIREWORKS_EFFECT_INDEX;
    effectB2.effectFunctionIncrementUniqueId = FIREWORKS_EFFECT_INDEX;
    effectB1.effectFunctionBonusTrigger = triggerFirework;
    effectB2.effectFunctionBonusTrigger = triggerFirework;
}

void setEffectBToLightChaseEffect()
{
    effectB1.effectFunction = [](int index) { return lightChaseEffect(index, &effectB1, (effectB1.time1 >> 10) % 72); };
    effectB2.effectFunction = [](int index) { return lightChaseEffect(index, &effectB2, 18); };
    effectB1.effectFunctionIncrement = []() { };
    effectB2.effectFunctionIncrement = []() { };
    effectB1.effectFunctionIncrementUniqueId = LIGHT_CHASE_EFFECT_INDEX;
    effectB2.effectFunctionIncrementUniqueId = LIGHT_CHASE_EFFECT_INDEX;
}

/// ---------------------------------------------------------------------------------------------------------------

void pickRandomEffectA()
{
    int effectSelection = getRandomEffectNumberFromAllowedEffects();
    setEffectA(effectSelection);
}

void setEffectA(uint8_t effectIndex)
{
    if (effectIndex == METEOR_RAIN_EFFECT_INDEX) setEffectAToMeteorRainEffect();
    else if (effectIndex == METEOR_RAIN_EFFECT_2_INDEX) setEffectAToMeteorRainEffect2();
    else if (effectIndex == STAR_FIELD_EFFECT_INDEX) setEffectAToStarFieldEffect();
    else if (effectIndex == GRADIENT_WAVE_EFFECT_INDEX) setEffectAToGradientWaveEffect();
    else if (effectIndex == RAIN_SHOWER_EFFECT_INDEX) setEffectAToRainShowerEffect();
    else if (effectIndex == EXPANDING_COLOR_ORB_EFFECT_INDEX) setEffectAToExpandingColorOrbEffect();
    else if (effectIndex == LIGHTNING_BUG_EFFECT_INDEX) setEffectAToLightningBugEffect();
    else if (effectIndex == FIRE_EFFECT_INDEX) setEffectAToFireEffect();
    else if (effectIndex == SIMPLE_SOLID_COLOR_FILL_EFFECT_INDEX) setEffectAToSimpleSolidColorFillEffect();
    else if (effectIndex == FIREWORKS_EFFECT_INDEX) setEffectAToFireworksEffect();
    else if (effectIndex == LIGHT_CHASE_EFFECT_INDEX) setEffectAToLightChaseEffect();
}

void pickRandomEffectB()
{
    int effectSelection = getRandomEffectNumberFromAllowedEffects();
    setEffectB(effectSelection);
}

void setEffectB(uint8_t effectIndex)
{
    if (effectIndex == METEOR_RAIN_EFFECT_INDEX) setEffectBToMeteorRainEffect();
    else if (effectIndex == METEOR_RAIN_EFFECT_2_INDEX) setEffectBToMeteorRainEffect2();
    else if (effectIndex == STAR_FIELD_EFFECT_INDEX) setEffectBToStarFieldEffect();
    else if (effectIndex == GRADIENT_WAVE_EFFECT_INDEX) setEffectBToGradientWaveEffect();
    else if (effectIndex == RAIN_SHOWER_EFFECT_INDEX) setEffectBToRainShowerEffect();
    else if (effectIndex == EXPANDING_COLOR_ORB_EFFECT_INDEX) setEffectBToExpandingColorOrbEffect();
    else if (effectIndex == LIGHTNING_BUG_EFFECT_INDEX) setEffectBToLightningBugEffect();
    else if (effectIndex == FIRE_EFFECT_INDEX) setEffectBToFireEffect();
    else if (effectIndex == SIMPLE_SOLID_COLOR_FILL_EFFECT_INDEX) setEffectBToSimpleSolidColorFillEffect();
    else if (effectIndex == FIREWORKS_EFFECT_INDEX) setEffectBToFireworksEffect();
    else if (effectIndex == LIGHT_CHASE_EFFECT_INDEX) setEffectBToLightChaseEffect();
}

void pickRandomEffects()
{
    pickRandomEffectA();
    pickRandomEffectB();
}

void saveCurrentEffectsState(SavedEffectSettings *savedEffectSettings)
{
    copyEffectSettings(&effectSettings, &(savedEffectSettings->effectSettings));
    saveSingleEffect(&effectA1, &savedEffectSettings->savedEffectA1);
    saveSingleEffect(&effectB1, &savedEffectSettings->savedEffectB1);
    saveSingleEffect(&effectA2, &savedEffectSettings->savedEffectA2);
    saveSingleEffect(&effectB2, &savedEffectSettings->savedEffectB2);
}

void loadCurrentEffectsState(SavedEffectSettings *savedEffectSettings)
{
    copyEffectSettings(&(savedEffectSettings->effectSettings), &effectSettings);
    loadSingleEffect(&savedEffectSettings->savedEffectA1, &effectA1);
    loadSingleEffect(&savedEffectSettings->savedEffectB1, &effectB1);
    loadSingleEffect(&savedEffectSettings->savedEffectA2, &effectA2);
    loadSingleEffect(&savedEffectSettings->savedEffectB2, &effectB2);
    setEffectA(effectA1.effectFunctionIncrementUniqueId);
    setEffectB(effectB1.effectFunctionIncrementUniqueId);

    setTransformMap1FromSettings(&effectA1);
    setTransformMap1FromSettings(&effectB1);
    setTransformMap1FromSettings(&effectA2);
    setTransformMap1FromSettings(&effectB2);
    setTransformMap2FromSettings(&effectA1);
    setTransformMap2FromSettings(&effectB1);
    setTransformMap2FromSettings(&effectA2);
    setTransformMap2FromSettings(&effectB2);

    setBrightnessPointerFromIndexForEffect(&effectA1);
    setBrightnessPointerFromIndexForEffect(&effectB1);
    setBrightnessPointerFromIndexForEffect(&effectA2);
    setBrightnessPointerFromIndexForEffect(&effectB2);

    //setCurrentScreenMapFromSettings();
}

void saveSingleEffect(Effect* fromEffect, SavedEffect* savedEffect)
{
    savedEffect->effectFunctionIncrementUniqueId = fromEffect->effectFunctionIncrementUniqueId;
    savedEffect->transformMap1Index = fromEffect->transformMap1Index;
    savedEffect->transformMap2Index = fromEffect->transformMap2Index;
    savedEffect->brightnessControlMode = fromEffect->brightnessControlMode;
    savedEffect->brightnessControlIndex = fromEffect->brightnessControlIndex;
    savedEffect->time1 = fromEffect->time1;
    savedEffect->time2 = fromEffect->time2;
    savedEffect->timeMode1 = fromEffect->timeMode1;
    savedEffect->timeMode2 = fromEffect->timeMode2;
    savedEffect->size = fromEffect->size;
    savedEffect->currentPalette = fromEffect->currentPalette;
    savedEffect->currentPaletteOffset = fromEffect->currentPaletteOffset;
    savedEffect->currentPaletteOffsetTarget = fromEffect->currentPaletteOffsetTarget;
    savedEffect->thresholdToShowMoreIntenseEffect = fromEffect->thresholdToShowMoreIntenseEffect;
}

void loadSingleEffect(SavedEffect* savedEffect, Effect* toEffect)
{
    toEffect->effectFunctionIncrementUniqueId = savedEffect->effectFunctionIncrementUniqueId;
    toEffect->transformMap1Index = savedEffect->transformMap1Index;
    toEffect->transformMap2Index = savedEffect->transformMap2Index;
    toEffect->brightnessControlMode = savedEffect->brightnessControlMode;
    toEffect->brightnessControlIndex = savedEffect->brightnessControlIndex;
    toEffect->time1 = savedEffect->time1;
    toEffect->time2 = savedEffect->time2;
    toEffect->timeMode1 = savedEffect->timeMode1;
    toEffect->timeMode2 = savedEffect->timeMode2;
    toEffect->size = savedEffect->size;
    toEffect->currentPalette = savedEffect->currentPalette;
    toEffect->currentPaletteOffset = savedEffect->currentPaletteOffset;
    toEffect->currentPaletteOffsetTarget = savedEffect->currentPaletteOffsetTarget;
    toEffect->thresholdToShowMoreIntenseEffect = savedEffect->thresholdToShowMoreIntenseEffect;
}

void copyEffectSettings(EffectSettings* fromEffectSettings, EffectSettings* toEffectSettings)
{
    toEffectSettings->LikelihoodEffectsAreRandomizedWhenBeatDetected = fromEffectSettings->LikelihoodEffectsAreRandomizedWhenBeatDetected;
    toEffectSettings->LikelihoodEffectsAreRandomizedWhenAntiBeatDetected = fromEffectSettings->LikelihoodEffectsAreRandomizedWhenAntiBeatDetected;
    toEffectSettings->LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero = fromEffectSettings->LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero;
    toEffectSettings->LikelihoodEffectsAreRandomizedWhenStepIsDetected = fromEffectSettings->LikelihoodEffectsAreRandomizedWhenStepIsDetected;

    toEffectSettings->LikelihoodNothingChangesWhenRandomizingEffect = fromEffectSettings->LikelihoodNothingChangesWhenRandomizingEffect;
    toEffectSettings->LikelihoodBlendingModeChangesWhenRandomizingEffect = fromEffectSettings->LikelihoodBlendingModeChangesWhenRandomizingEffect;
    toEffectSettings->LikelihoodSubPaletteChangesWhenRandomizingEffect = fromEffectSettings->LikelihoodSubPaletteChangesWhenRandomizingEffect;
    toEffectSettings->LikelihoodPaletteChangesWhenRandomizingEffect = fromEffectSettings->LikelihoodPaletteChangesWhenRandomizingEffect;
    toEffectSettings->LikelihoodTwoPalettesAreUsedWhenPaletteChanges = fromEffectSettings->LikelihoodTwoPalettesAreUsedWhenPaletteChanges;

    toEffectSettings->LikelihoodBackgroundTransitionTimeChangesWhenRandomizingEffect = fromEffectSettings->LikelihoodBackgroundTransitionTimeChangesWhenRandomizingEffect;
    toEffectSettings->LikelihoodScreenMapChangesWhenRandomizingEffect = fromEffectSettings->LikelihoodScreenMapChangesWhenRandomizingEffect;
    toEffectSettings->LikelihoodEffectsAreSwappedWhenRandomizingEffect = fromEffectSettings->LikelihoodEffectsAreSwappedWhenRandomizingEffect;
    toEffectSettings->LikelihoodTransformMapsAreSwitchedWhenEffectsAreSwapped = fromEffectSettings->LikelihoodTransformMapsAreSwitchedWhenEffectsAreSwapped;
    toEffectSettings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched = fromEffectSettings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched;
    toEffectSettings->LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect = fromEffectSettings->LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect;
    toEffectSettings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized = fromEffectSettings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized;

    toEffectSettings->LikelihoodTimeModesRandomizeWhenRandomizingEffect = fromEffectSettings->LikelihoodTimeModesRandomizeWhenRandomizingEffect;
    toEffectSettings->LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes = fromEffectSettings->LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes;

    toEffectSettings->LikelihoodEffectsAreRandomizedWhenRandomizingEffect = fromEffectSettings->LikelihoodEffectsAreRandomizedWhenRandomizingEffect;

    toEffectSettings->LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect = fromEffectSettings->LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect;

    toEffectSettings->LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect = fromEffectSettings->LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect;
    toEffectSettings->LikelihoodIndividualGlobalBrightnessModesChange = fromEffectSettings->LikelihoodIndividualGlobalBrightnessModesChange;

    toEffectSettings->LikelihoodTransitionDirectionChangesWhenRandomizingEffect = fromEffectSettings->LikelihoodTransitionDirectionChangesWhenRandomizingEffect;

    toEffectSettings->LikelihoodMovementBasedBrightnessModeIsPicked = fromEffectSettings->LikelihoodMovementBasedBrightnessModeIsPicked;
    toEffectSettings->LikelihoodMusicBasedBrightnessModeIsPicked = fromEffectSettings->LikelihoodMusicBasedBrightnessModeIsPicked;
    toEffectSettings->LikelihoodMovementBasedTimeModeIsPicked = fromEffectSettings->LikelihoodMovementBasedTimeModeIsPicked;

    toEffectSettings->MillisecondToMoveToNextPaletteFrame = fromEffectSettings->MillisecondToMoveToNextPaletteFrame;
    toEffectSettings->MillisecondsForEffectTransitionsMinimum = fromEffectSettings->MillisecondsForEffectTransitionsMinimum;
    toEffectSettings->MillisecondsForEffectTransitionsMaximum = fromEffectSettings->MillisecondsForEffectTransitionsMaximum;
    toEffectSettings->MillisecondsForBlendingModeTransitionsMinimum = fromEffectSettings->MillisecondsForBlendingModeTransitionsMinimum;
    toEffectSettings->MillisecondsForBlendingModeTransitionsMaximum = fromEffectSettings->MillisecondsForBlendingModeTransitionsMaximum;

    toEffectSettings->GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps = fromEffectSettings->GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps;
    toEffectSettings->GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps = fromEffectSettings->GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps;

    toEffectSettings->LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect = fromEffectSettings->LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect;
    toEffectSettings->AudioLevelThresholdToShowMoreIntenseEffectMinimum = fromEffectSettings->AudioLevelThresholdToShowMoreIntenseEffectMinimum;
    toEffectSettings->AudioLevelThresholdToShowMoreIntenseEffectMaximum = fromEffectSettings->AudioLevelThresholdToShowMoreIntenseEffectMaximum;

    for (int i = 0; i < NumberOfAllowedEffectsToPickBetween; i++)
    {
        toEffectSettings->AllowedEffects[i] = fromEffectSettings->AllowedEffects[i];
    }
    for (int i = 0; i < NumberOfAllowedEffectsToPickBetween; i++)
    {
        for (int j = 0; j < NumberOfAllowedEffectsToPickBetween + 1; j++)
        {
            toEffectSettings->AllowedPalettes[i][j] = fromEffectSettings->AllowedPalettes[i][j];
        }
    }

    toEffectSettings->MinimumEffectSize = fromEffectSettings->MinimumEffectSize;
    toEffectSettings->MaximumEffectSize = fromEffectSettings->MaximumEffectSize;

    toEffectSettings->LikelihoodWingsAreMirroredWhenTransformMapsAreRandomized = fromEffectSettings->LikelihoodWingsAreMirroredWhenTransformMapsAreRandomized;

    toEffectSettings->CurrentScreenMapIndex = fromEffectSettings->CurrentScreenMapIndex;
    toEffectSettings->PrimaryEffectToggle = fromEffectSettings->PrimaryEffectToggle;
    toEffectSettings->RandomizeEffectsAutomaticallyOverTime = fromEffectSettings->RandomizeEffectsAutomaticallyOverTime;
    toEffectSettings->MillisecondsBetweenTimeBasedAutomaticEffectRandomization = fromEffectSettings->MillisecondsBetweenTimeBasedAutomaticEffectRandomization;
}

void setTransformMap1FromSettings(Effect* effect)
{
    if (effect->isTransformMap1Mirrored)
    {
        (*effect->transformMap1) = mirroredTransformMaps[effect->transformMap1Index];
    }
    else 
    {
        (*effect->transformMap1) = transformMaps[effect->transformMap1Index];
    }
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_TRANSFORM_MAP_1, effect->effectId, effect->transformMap1Index);
}

void setTransformMap2FromSettings(Effect* effect)
{
    if (effect->isTransformMap2Mirrored)
    {
        (*effect->transformMap2) = mirroredTransformMaps[effect->transformMap2Index];
    }
    else 
    {
        (*effect->transformMap2) = transformMaps[effect->transformMap2Index];
    }
    D_emitIntegerMetric(METRIC_NAME_ID_EFFECT_TRANSFORM_MAP_2, effect->effectId, effect->transformMap2Index);
}

void setCurrentScreenMapFromSettings()
{
    *currentScreenMap = screenMaps[effectSettings.CurrentScreenMapIndex];
}

int getScreenMapCount()
{
    return screenMapsCount;
}