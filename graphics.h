#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Adafruit_NeoPXL8.h>

typedef struct
{
  uint16_t red;
  uint16_t green;
  uint16_t blue;
}
Color;

Color wavePulse(uint32_t now, int pixelNum, uint8_t brightnessProjectionMap[], uint8_t colorProjectionMap[], uint16_t color_palette[], uint16_t globalBrightnessModifier);

Color blendColorsUsingMixing(Color color1, Color color2, uint16_t blendFactor);
Color blendColorsUsingMixingGlitched(Color color1, Color color2, uint16_t blendFactor);
Color blendColorsUsingMultiply(Color color1, Color color2, uint16_t blendFactor);
Color blendColorsUsingAdd(Color color1, Color color2, uint16_t blendFactor);
Color blendColorsUsingSubtract(Color color1, Color color2, uint16_t blendFactor);
Color blendColorsUsingOverlay(Color color1, Color color2, uint16_t blendFactor);
Color blendColorsUsingScreen(Color color1, Color color2, uint16_t blendFactor);
Color blendColorsUsingAverage(Color color1, Color color2, uint16_t blendFactor);

#endif