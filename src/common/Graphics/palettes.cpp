
#include "../Graphics/palettes.h"

Color colorFromPalette(int index, uint8_t brightness)
{
  index = index * 3;
  return { 
    (uint8_t)((palette[index + 0] * brightness) >> 8), 
    (uint8_t)((palette[index + 1] * brightness) >> 8),
    (uint8_t)((palette[index + 2] * brightness) >> 8)
  };
}