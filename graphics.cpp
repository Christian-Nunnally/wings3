#include <Adafruit_NeoPXL8.h>
#include "microphone.h"
#include "graphics.h"

Color16 colorFromPalette(Color palette[40], uint8_t index, uint16_t brightness)
{
  return { 
    (uint16_t)((palette[index].red * brightness) >> 8), 
    (uint16_t)((palette[index].green * brightness) >> 8),
    (uint16_t)((palette[index].blue * brightness) >> 8)
  };
}

Color16 wavePulse(uint32_t now, int pixelNum, uint8_t brightnessProjectionMap[], uint8_t colorProjectionMap[], Color color_palette[], double globalBrightnessModifier) 
{
    const int colorPaletteLength = 40; 
    uint8_t brightnessFrame = (now / 4);
    uint8_t colorFrame = (now / 15);;
    uint8_t brightnessStartingPoint = brightnessProjectionMap[pixelNum];
    uint8_t colorStartingPoint = colorProjectionMap[pixelNum];
    uint16_t brightness = ((uint8_t)(brightnessStartingPoint - brightnessFrame) << 8) * globalBrightnessModifier;
    return colorFromPalette(color_palette, ((colorStartingPoint + colorFrame) * colorPaletteLength/256) % colorPaletteLength, brightness);
}

Color16 blendColorsUsingMixing(Color16 color1, Color16 color2, float blendFactor)
{
  uint16_t r = sqrt((1 - blendFactor) * color1.red * color1.red + blendFactor * color2.red * color2.red);
  uint16_t g = sqrt((1 - blendFactor) * color1.green * color1.green + blendFactor * color2.green * color2.green);
  uint16_t b = sqrt((1 - blendFactor) * color1.blue * color1.blue + blendFactor * color2.blue * color2.blue);
  return {r, g, b};
}