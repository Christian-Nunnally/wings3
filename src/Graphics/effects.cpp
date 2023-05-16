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

Effect effect1;
Effect effect2;
Effect effect3;
Effect effect4;

EffectSettings effectSettings;

float currentAudioIntensityLevel;

void setAudioIntensityLevel(float level)
{
    currentAudioIntensityLevel = level;
}

void pickRandomEffects()
{
    int effectSelection = getRandomEffectNumberFromAllowedEffects();
    if (effectSelection == 0) effect1.effectFunction = [](int index) { return meteorRainEffect2(index, &effect1, .12); };
    else if (effectSelection == 1) effect1.effectFunction = [](int index) { return meteorRainEffect(index, &effect1, .12); };
    else if (effectSelection == 2) effect1.effectFunction = [](int index) { return meteorRainEffect2(index, &effect1, .12); };
    else if (effectSelection == 3) effect1.effectFunction = [](int index) { return starFieldEffect(index, &effect1, 20, 200); };
    else if (effectSelection == 4) effect1.effectFunction = [](int index) { return gradientWaveEffect(index, &effect1); };
    else if (effectSelection == 5) effect1.effectFunction = [](int index) { return rainShowerEffect(index, &effect1); };
    else if (effectSelection == 6) effect1.effectFunction = [](int index) { return expandingColorOrbEffect(index, &effect1); };
    else if (effectSelection == 7) effect1.effectFunction = [](int index) { return lightningBugEffect(index, &effect1); };
    else if (effectSelection == 8) effect1.effectFunction = [](int index) { return fireEffect(index, &effect1, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel); };
    else if (effectSelection == 9) effect1.effectFunction = [](int index) { return simpleSolidColorFillEffect(index, &effect1); };
    else if (effectSelection == 10) effect1.effectFunction = [](int index) { return fireworksEffect(index, &effect1, 40); };
    else if (effectSelection == 11) effect1.effectFunction = [](int index) { return lightChaseEffect(index, &effect1, 5); };
    else effect1.effectFunction = [](int index) { return gradientWaveEffect(index, &effect1); };
    if (effectSelection == 0) effect3.effectFunction = [](int index) { return meteorRainEffect2(index, &effect3, .12); };
    else if (effectSelection == 1) effect3.effectFunction = [](int index) { return meteorRainEffect(index, &effect3, .12); };
    else if (effectSelection == 2) effect3.effectFunction = [](int index) { return meteorRainEffect2(index, &effect3, .12); };
    else if (effectSelection == 3) effect3.effectFunction = [](int index) { return starFieldEffect(index, &effect3, 16, 120); };
    else if (effectSelection == 4) effect3.effectFunction = [](int index) { return gradientWaveEffect(index, &effect3); };
    else if (effectSelection == 5) effect3.effectFunction = [](int index) { return rainShowerEffect(index, &effect3); };
    else if (effectSelection == 6) effect3.effectFunction = [](int index) { return expandingColorOrbEffect(index, &effect3); };
    else if (effectSelection == 7) effect3.effectFunction = [](int index) { return lightningBugEffect(index, &effect3); };
    else if (effectSelection == 8) effect3.effectFunction = [](int index) { return fireEffect(index, &effect3, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel); };
    else if (effectSelection == 9) effect3.effectFunction = [](int index) { return simpleSolidColorFillEffect(index, &effect3); };
    else if (effectSelection == 10) effect3.effectFunction = [](int index) { return fireworksEffect(index, &effect3, 40); };
    else if (effectSelection == 11) effect3.effectFunction = [](int index) { return lightChaseEffect(index, &effect3, 4); };
    else effect3.effectFunction = [](int index) { return gradientWaveEffect(index, &effect3); };
    effectSelection = getRandomEffectNumberFromAllowedEffects();
    if (effectSelection == 0) effect2.effectFunction = [](int index) { return meteorRainEffect2(index, &effect2, .12); };
    else if (effectSelection == 1) effect2.effectFunction = [](int index) { return meteorRainEffect(index, &effect2, .12); };
    else if (effectSelection == 2) effect2.effectFunction = [](int index) { return meteorRainEffect2(index, &effect2, .12); };
    else if (effectSelection == 3) effect2.effectFunction = [](int index) { return starFieldEffect(index, &effect2, 16, 180); };
    else if (effectSelection == 4) effect2.effectFunction = [](int index) { return gradientWaveEffect(index, &effect2); };
    else if (effectSelection == 5) effect2.effectFunction = [](int index) { return rainShowerEffect(index, &effect2); };
    else if (effectSelection == 6) effect2.effectFunction = [](int index) { return expandingColorOrbEffect(index, &effect2); };
    else if (effectSelection == 7) effect2.effectFunction = [](int index) { return lightningBugEffect(index, &effect2); };
    else if (effectSelection == 8) effect2.effectFunction = [](int index) { return fireEffect(index, &effect2, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel); };
    else if (effectSelection == 9) effect2.effectFunction = [](int index) { return simpleSolidColorFillEffect(index, &effect2); };
    else if (effectSelection == 10) effect2.effectFunction = [](int index) { return fireworksEffect(index, &effect2, 40); };
    else if (effectSelection == 11) effect2.effectFunction = [](int index) { return lightChaseEffect(index, &effect2, 3); };
    else effect2.effectFunction = [](int index) { return gradientWaveEffect(index, &effect2); };
    if (effectSelection == 0) effect4.effectFunction = [](int index) { return meteorRainEffect2(index, &effect4, .12); };
    else if (effectSelection == 1) effect4.effectFunction = [](int index) { return meteorRainEffect(index, &effect4, .12); };
    else if (effectSelection == 2) effect4.effectFunction = [](int index) { return meteorRainEffect2(index, &effect4, .12); };
    else if (effectSelection == 3) effect4.effectFunction = [](int index) { return starFieldEffect(index, &effect4, 20, 400); };
    else if (effectSelection == 4) effect4.effectFunction = [](int index) { return gradientWaveEffect(index, &effect4); };
    else if (effectSelection == 5) effect4.effectFunction = [](int index) { return rainShowerEffect(index, &effect4); };
    else if (effectSelection == 6) effect4.effectFunction = [](int index) { return expandingColorOrbEffect(index, &effect4); };
    else if (effectSelection == 7) effect4.effectFunction = [](int index) { return lightningBugEffect(index, &effect4); };
    else if (effectSelection == 8) effect4.effectFunction = [](int index) { return fireEffect(index, &effect4, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel); };
    else if (effectSelection == 9) effect4.effectFunction = [](int index) { return simpleSolidColorFillEffect(index, &effect4); };
    else if (effectSelection == 10) effect4.effectFunction = [](int index) { return fireworksEffect(index, &effect4, 40); };
    else if (effectSelection == 11) effect4.effectFunction = [](int index) { return lightChaseEffect(index, &effect4, 7); };
    else effect4.effectFunction = [](int index) { return gradientWaveEffect(index, &effect4); };
}

int getRandomEffectNumberFromAllowedEffects()
{
    int effectRandom = fastRandomInteger(NumberOfAllowedEffectsToPickBetween);
    return effectSettings.AllowedEffects[effectRandom];
}