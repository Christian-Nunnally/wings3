
#include "../Graphics/palettes.h"

Color colorFromPalette(int index, uint16_t brightness)
{
  uint8_t red = palette[index * 3 + 0];
  uint8_t green = palette[index * 3 + 1];
  uint8_t blue = palette[index * 3 + 2];
  return { 
    (uint8_t)((red * brightness) >> 16), 
    (uint8_t)((green * brightness) >> 16),
    (uint8_t)((blue * brightness) >> 16)
  };
}