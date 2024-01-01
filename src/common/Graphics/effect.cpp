
#include "../Graphics/effect.h"
#include "../Graphics/normalTransformMaps.h"
#include "../Graphics/mirroredTransformMaps.h"

#ifdef RP2040
#else
#include <iostream>
#endif

uint8_t defaultGlobalBrightness = 255;

void initializeEffect(Effect *effect, int *frameTimeDeltaPointer, Color (*defaultEffect)(int), uint8_t effectId)
{
    effect->effectFunction = defaultEffect;
    effect->effectFunctionHighlight = defaultEffect;
    effect->effectFunctionIncrement = +[]{};
    effect->effectFunctionBonusTrigger = +[]{};
    effect->effectFunctionIncrementUniqueId = -1;
    effect->isTransformMap1Mirrored = 0;
    effect->isTransformMap2Mirrored = 0;
    effect->transformMap1Index = 0;
    effect->transformMap2Index = 0;
    *effect->transformMap1 = transformMaps[0];
    *effect->transformMap2 = mirroredTransformMaps[0];
    effect->globalBrightnessPointer = &defaultGlobalBrightness;
    effect->brightnessControlIndex = 0;
    effect->brightnessControlMode = 0;
    effect->frameTimeDelta = frameTimeDeltaPointer;
    effect->time1 = 0;
    effect->time2 = 0;
    effect->timeMode1 = 0;
    effect->timeMode2 = 0;
    effect->size = 0;
    effect->currentPalette = 0;
    effect->currentPaletteOffset = 0;
    effect->currentPaletteOffsetTarget = 0;
    effect->effectId = effectId;
}

int getNormalTransformMapCount()
{
    return transformMapsCount;
}

int getMirroredTransformMapCount()
{
    return mirroredTransformMapsCount;
}