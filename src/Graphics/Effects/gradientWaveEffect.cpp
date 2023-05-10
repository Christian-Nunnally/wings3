
#include "../../Graphics/Effects/gradientWaveEffect.h"
#include "../../Graphics/palettes.h"

Color gradientWaveEffect(int pixelIndex, Effect *effect) 
{
    uint8_t brightnessFrame8bit = effect->time1 / 4;
    uint8_t colorFrame8bit = effect->time2 / 4;
    uint8_t brightnessStartingPoint = (*effect->transformMap1)[pixelIndex];
    uint8_t colorStartingPoint = (*effect->transformMap2)[pixelIndex];
    uint16_t brightness = ((uint8_t)(brightnessStartingPoint - brightnessFrame8bit) << 8);
    brightness = (brightness * (*effect->globalBrightnessPointer)) >> 16;
    uint8_t colorFramePreOffset = colorStartingPoint + colorFrame8bit;
    return colorFromPalette(effect->currentPaletteOffset + colorFramePreOffset, brightness);
}

Color gradientWaveEffectWithMaxGlobalBrightness(int pixelIndex, Effect *effect) 
{
    uint8_t brightnessFrame8bit = effect->time1 / 4;
    uint8_t colorFrame8bit = effect->time2 / 4;
    uint8_t brightnessStartingPoint = (*effect->transformMap1)[pixelIndex];
    uint8_t colorStartingPoint = (*effect->transformMap2)[pixelIndex];
    uint16_t brightness = ((uint8_t)(brightnessStartingPoint - brightnessFrame8bit) << 8);
    uint8_t colorFramePreOffset = colorStartingPoint + colorFrame8bit;
    return colorFromPalette(effect->currentPaletteOffset + colorFramePreOffset, brightness);
}