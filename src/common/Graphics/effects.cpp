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
#include "../Utility/fastRandom.h"

int getRandomEffectNumberFromAllowedEffects();

Effect effectA1;
Effect effectB1;
Effect effectA2;
Effect effectB2;

EffectSettings effectSettings;

float currentAudioIntensityLevel;

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
    if (effectSelection == METEOR_RAIN_EFFECT_INDEX) setEffectAToMeteorRainEffect();
    else if (effectSelection == METEOR_RAIN_EFFECT_2_INDEX) setEffectAToMeteorRainEffect2();
    else if (effectSelection == STAR_FIELD_EFFECT_INDEX) setEffectAToStarFieldEffect();
    else if (effectSelection == GRADIENT_WAVE_EFFECT_INDEX) setEffectAToGradientWaveEffect();
    else if (effectSelection == RAIN_SHOWER_EFFECT_INDEX) setEffectAToRainShowerEffect();
    else if (effectSelection == EXPANDING_COLOR_ORB_EFFECT_INDEX) setEffectAToExpandingColorOrbEffect();
    else if (effectSelection == LIGHTNING_BUG_EFFECT_INDEX) setEffectAToLightningBugEffect();
    else if (effectSelection == FIRE_EFFECT_INDEX) setEffectAToFireEffect();
    else if (effectSelection == SIMPLE_SOLID_COLOR_FILL_EFFECT_INDEX) setEffectAToSimpleSolidColorFillEffect();
    else if (effectSelection == FIREWORKS_EFFECT_INDEX) setEffectAToFireworksEffect();
    else if (effectSelection == LIGHT_CHASE_EFFECT_INDEX) setEffectAToLightChaseEffect();
}

void pickRandomEffectB()
{
    int effectSelection = getRandomEffectNumberFromAllowedEffects();
    if (effectSelection == METEOR_RAIN_EFFECT_INDEX) setEffectBToMeteorRainEffect();
    else if (effectSelection == METEOR_RAIN_EFFECT_2_INDEX) setEffectBToMeteorRainEffect2();
    else if (effectSelection == STAR_FIELD_EFFECT_INDEX) setEffectBToStarFieldEffect();
    else if (effectSelection == GRADIENT_WAVE_EFFECT_INDEX) setEffectBToGradientWaveEffect();
    else if (effectSelection == RAIN_SHOWER_EFFECT_INDEX) setEffectBToRainShowerEffect();
    else if (effectSelection == EXPANDING_COLOR_ORB_EFFECT_INDEX) setEffectBToExpandingColorOrbEffect();
    else if (effectSelection == LIGHTNING_BUG_EFFECT_INDEX) setEffectBToLightningBugEffect();
    else if (effectSelection == FIRE_EFFECT_INDEX) setEffectBToFireEffect();
    else if (effectSelection == SIMPLE_SOLID_COLOR_FILL_EFFECT_INDEX) setEffectBToSimpleSolidColorFillEffect();
    else if (effectSelection == FIREWORKS_EFFECT_INDEX) setEffectBToFireworksEffect();
    else if (effectSelection == LIGHT_CHASE_EFFECT_INDEX) setEffectBToLightChaseEffect();
}

void pickRandomEffects()
{
    pickRandomEffectA();
    pickRandomEffectB();
}