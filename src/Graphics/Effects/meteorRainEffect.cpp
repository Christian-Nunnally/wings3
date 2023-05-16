
#include "../../Graphics/Effects/meteorRainEffect.h"
#include "../../Graphics/palettes.h"
#include "../../Graphics/colorMixing.h"
#include "../../Utility/fastRandom.h"
#include "../../settings.h"

Color currentLedColorMap[TOTAL_LEDS];

Color meteorRainEffect(int pixelIndex, Effect *effect, float meteorTrailDecay)
{
  uint32_t projectionMap1FrameWrapping = effect->time1 % (256 + effect->size * 10);
  if (fastRandomInteger(90) < *(effect->frameTimeDelta))
  {
    currentLedColorMap[pixelIndex] = fadeColorToBlack(currentLedColorMap[pixelIndex], meteorTrailDecay );        
  }
  if ((*effect->transformMap1)[pixelIndex] + effect->size > projectionMap1FrameWrapping && (*effect->transformMap1)[pixelIndex] + effect->size < projectionMap1FrameWrapping + effect->size)
  {
    uint8_t colorFrame8bit = (effect->time2 / 30);
    uint8_t colorStartingPoint = (*effect->transformMap1)[pixelIndex];
    uint8_t colorFramePreOffset = colorStartingPoint + colorFrame8bit;
    Color result = colorFromPalette(effect->currentPaletteOffset + colorFramePreOffset, (*effect->globalBrightnessPointer));
    currentLedColorMap[pixelIndex] = result;
  }
  return currentLedColorMap[pixelIndex];
}

Color meteorRainEffect2(int pixelIndex, Effect *effect, float meteorTrailDecay)
{
  uint32_t projectionMap1FrameWrapping = effect->time1 % (256 + effect->size * 10);
  uint32_t projectionMap2FrameWrapping = effect->time2 % (256 + effect->size * 10);
  if (fastRandomInteger(90) < *(effect->frameTimeDelta))
  {
    currentLedColorMap[pixelIndex] = fadeColorToBlack(currentLedColorMap[pixelIndex], meteorTrailDecay );        
  }
  if ((*effect->transformMap1)[pixelIndex] + effect->size > projectionMap1FrameWrapping && (*effect->transformMap2)[pixelIndex] + effect->size < projectionMap1FrameWrapping + effect->size)
  {
    if ((*effect->transformMap2)[pixelIndex] + effect->size > projectionMap2FrameWrapping && (*effect->transformMap2)[pixelIndex] + effect->size < projectionMap2FrameWrapping + effect->size)
    {
      uint8_t colorFrame8bit = (effect->time1 / 30);
      uint8_t colorStartingPoint = (*effect->transformMap1)[pixelIndex];
      uint8_t colorFramePreOffset = colorStartingPoint + colorFrame8bit;
      Color result = colorFromPalette(effect->currentPaletteOffset + colorFramePreOffset, (*effect->globalBrightnessPointer));
      currentLedColorMap[pixelIndex] = result;
    }
  }
  return currentLedColorMap[pixelIndex];
}



