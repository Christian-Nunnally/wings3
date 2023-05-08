
#include "../../Graphics/Effects/meteorRainEffect.h"
#include "../../Graphics/palettes.h"
#include "../../Graphics/colorMixing.h"
#include "../../Utility/fastRandom.h"
#include "../../settings.h"

Color currentLedColorMap[TOTAL_LEDS];

Color meteorRainEffect(int frameDelta, uint32_t animationFrame, uint32_t colorFrame, int pixelIndex, byte meteorSize, float meteorTrailDecay, const uint8_t projectionMap[], int paletteOffset, uint16_t globalBrightnessModifier)
{
  uint32_t projectionMap1FrameWrapping = animationFrame % (256 + meteorSize * 10);
  if (fastRandomInteger(90) < frameDelta)
  {
    currentLedColorMap[pixelIndex] = fadeColorToBlack(currentLedColorMap[pixelIndex], meteorTrailDecay );        
  }
  if (projectionMap[pixelIndex] + meteorSize > projectionMap1FrameWrapping && projectionMap[pixelIndex] + meteorSize < projectionMap1FrameWrapping + meteorSize)
  {
    uint8_t colorFrame8bit = (colorFrame / 15);
    uint8_t colorStartingPoint = projectionMap[pixelIndex];
    uint8_t colorFramePreOffset = colorStartingPoint + colorFrame8bit;
    Color result = colorFromPalette(paletteOffset + colorFramePreOffset, globalBrightnessModifier);
    currentLedColorMap[pixelIndex] = result;
  }
  return currentLedColorMap[pixelIndex];
}

Color meteorRainEffect2(int frameDelta, uint32_t projectionMap1Frame, uint32_t projectionMap2Frame, uint32_t colorFrame, int pixelIndex, byte meteorSize, float meteorTrailDecay, const uint8_t projectionMap[], const uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier)
{
  uint32_t projectionMap1FrameWrapping = projectionMap1Frame % (256 + meteorSize * 10);
  uint32_t projectionMap2FrameWrapping = projectionMap2Frame % (256 + meteorSize * 10);
  if (fastRandomInteger(90) < frameDelta)
  {
    currentLedColorMap[pixelIndex] = fadeColorToBlack(currentLedColorMap[pixelIndex], meteorTrailDecay );        
  }
  if (projectionMap[pixelIndex] + meteorSize > projectionMap1FrameWrapping && projectionMap[pixelIndex] + meteorSize < projectionMap1FrameWrapping + meteorSize)
  {
    if (projectionMap2[pixelIndex] + meteorSize > projectionMap2FrameWrapping && projectionMap2[pixelIndex] + meteorSize < projectionMap2FrameWrapping + meteorSize)
    {
      uint8_t colorFrame8bit = (colorFrame / 15);
      uint8_t colorStartingPoint = projectionMap[pixelIndex];
      uint8_t colorFramePreOffset = colorStartingPoint + colorFrame8bit;
      Color result = colorFromPalette(paletteOffset + colorFramePreOffset, globalBrightnessModifier);
      currentLedColorMap[pixelIndex] = result;
    }
  }
  return currentLedColorMap[pixelIndex];
}



