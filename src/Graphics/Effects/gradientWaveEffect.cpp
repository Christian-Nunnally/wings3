
#include "../../Graphics/Effects/gradientWaveEffect.h"
#include "../../Graphics/palettes.h"

Color gradientWaveEffect(uint32_t brightnessFrame, uint32_t colorFrame, int pixelIndex, const uint8_t brightnessProjectionMap[], const uint8_t colorProjectionMap[], int paletteOffset, uint16_t globalBrightnessModifier) 
{
    uint8_t brightnessFrame8bit = (brightnessFrame / 4);
    uint8_t colorFrame8bit = (colorFrame / 10);
    uint8_t brightnessStartingPoint = brightnessProjectionMap[pixelIndex];
    uint8_t colorStartingPoint = colorProjectionMap[pixelIndex];
    uint16_t brightness = ((uint8_t)(brightnessStartingPoint - brightnessFrame8bit) << 8);
    brightness = (brightness * globalBrightnessModifier) >> 16;
    uint8_t colorFramePreOffset = colorStartingPoint + colorFrame8bit;
    return colorFromPalette(paletteOffset + colorFramePreOffset, brightness);
}