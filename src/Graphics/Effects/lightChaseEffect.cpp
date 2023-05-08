
#include "../../Graphics/Effects/lightChaseEffect.h"
#include "../../Graphics/palettes.h"

Color lightChaseEffect(uint32_t ledTestAnimationFrame, int pixelIndex, uint32_t colorAnimationFrame, int paletteOffset, int mod, uint16_t globalBrightnessModifier)
{
  uint8_t colorFrame8bit = colorAnimationFrame;
  if ((ledTestAnimationFrame >> 8) % 5 == pixelIndex % mod) return colorFromPalette(paletteOffset + colorFrame8bit, globalBrightnessModifier);
  return {0,0,0};
}