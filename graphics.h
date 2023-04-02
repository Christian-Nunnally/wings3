#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Adafruit_NeoPXL8.h>

typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
}
Color;

typedef struct
{
  uint16_t red;
  uint16_t green;
  uint16_t blue;
}
Color16;

Color16 wavePulse(uint32_t now, int pixelNum, uint8_t brightnessProjectionMap[], uint8_t colorProjectionMap[], Color color_palette[], double globalBrightnessModifier);

Color16 blendColorsUsingMixing(Color16 color1, Color16 color2, float blendFactor);

#endif