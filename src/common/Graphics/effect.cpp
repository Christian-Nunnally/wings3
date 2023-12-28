
#include "../Graphics/effect.h"
#include "../Graphics/transformMaps.h"

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
    *effect->transformMap1 = normalTransformMapX;
    *effect->transformMap2 = normalTransformMapY;
    effect->globalBrightnessPointer = &defaultGlobalBrightness;
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