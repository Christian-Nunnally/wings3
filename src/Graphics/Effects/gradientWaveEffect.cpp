
#include "../../Graphics/Effects/gradientWaveEffect.h"
#include "../../Graphics/palettes.h"

#ifdef RP2040
#else
#include <iostream>
#endif

Color gradientWaveEffect(int pixelIndex, Effect *effect) 
{
    uint8_t brightnessFrame8bit = effect->time1 / 4;
    uint8_t colorFrame8bit = effect->time2 / 4;
    uint8_t brightnessStartingPoint = (*effect->transformMap1)[pixelIndex];
    uint8_t colorStartingPoint = (*effect->transformMap2)[pixelIndex];
    uint8_t brightness = brightnessStartingPoint - brightnessFrame8bit;
    brightness = (brightness * (*effect->globalBrightnessPointer)) >> 8;
    uint8_t colorFramePreOffset = colorStartingPoint + colorFrame8bit;
    return colorFromPalette(effect->currentPaletteOffset + colorFramePreOffset, brightness);
}

Color gradientWaveEffectWithMaxGlobalBrightness(int pixelIndex, Effect *effect) 
{
    uint8_t brightnessFrame8bit = effect->time1 / 4;
    uint8_t colorFrame8bit = effect->time2 / 4;
    uint8_t brightnessStartingPoint = (*effect->transformMap1)[pixelIndex];
    uint8_t colorStartingPoint = (*effect->transformMap2)[pixelIndex];
    uint8_t brightness = brightnessStartingPoint - brightnessFrame8bit;
    uint8_t colorFramePreOffset = colorStartingPoint + colorFrame8bit;
    return colorFromPalette(effect->currentPaletteOffset + colorFramePreOffset, brightness);
}