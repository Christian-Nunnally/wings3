
#include "../../Graphics/Effects/lightChaseEffect.h"
#include "../../Graphics/palettes.h"

Color lightChaseEffect(int pixelIndex, Effect *effect, int mod)
{
  uint8_t colorFrame8bit = effect->time1;
  if ((effect->time2 >> 8) % 5 == pixelIndex % mod) return colorFromPalette(effect->currentPaletteOffset + colorFrame8bit, (*effect->globalBrightnessPointer));
  return {0,0,0};
}