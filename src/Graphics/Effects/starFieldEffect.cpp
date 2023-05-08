
#include "../../Graphics/Effects/starFieldEffect.h"
#include "../../Graphics/palettes.h"
#include "../../Utility/fastRandom.h"
#include "../../settings.h"

uint16_t starShimmerMap[TOTAL_LEDS];
uint16_t starShimmerCurrentTargetValueMap[TOTAL_LEDS];
uint16_t starShimmerSpeedMap[TOTAL_LEDS];
uint16_t starShimmerColorMap[TOTAL_LEDS];
uint16_t starShimmerTwinkleMap[TOTAL_LEDS];
uint16_t starShimmerTwinklePersistanceMap[TOTAL_LEDS];
bool starShimmerTwinkleDirectionMap[TOTAL_LEDS];
Color starFieldEffect(int frameDelta, int fadeAmount, uint16_t newStarLikelihood, int pixelIndex, int paletteOffset, uint16_t globalBrightnessModifier) 
{
  if (pixelIndex == 0)
  {
    if (fastRandomInteger(10000) < newStarLikelihood * frameDelta)
    {
      int chosenLed = fastRandomInteger(TOTAL_LEDS);
      starShimmerCurrentTargetValueMap[chosenLed] = fastRandomInteger(globalBrightnessModifier);
      starShimmerSpeedMap[chosenLed] = 2 * fastRandomInteger(1, fadeAmount);
      starShimmerColorMap[chosenLed] = fastRandomInteger(PALETTE_LENGTH) + paletteOffset;
      starShimmerTwinkleMap[chosenLed] = 0;
      starShimmerTwinklePersistanceMap[chosenLed] = 0;
      starShimmerTwinkleDirectionMap[chosenLed] = 0;
    }
  }

  if (starShimmerCurrentTargetValueMap[pixelIndex] > 0)
  {
    if(starShimmerMap[pixelIndex] < starShimmerCurrentTargetValueMap[pixelIndex])
    {
      if (starShimmerMap[pixelIndex] + starShimmerSpeedMap[pixelIndex] * frameDelta < starShimmerCurrentTargetValueMap[pixelIndex])
      {
        starShimmerMap[pixelIndex] += starShimmerSpeedMap[pixelIndex] * frameDelta;
      }
      else 
      {
        starShimmerMap[pixelIndex] = starShimmerCurrentTargetValueMap[pixelIndex];
        starShimmerCurrentTargetValueMap[pixelIndex] = 1;
        starShimmerSpeedMap[pixelIndex] = fastRandomInteger(1, fadeAmount);
        starShimmerTwinkleMap[pixelIndex] = fastRandomInteger(2000);
        starShimmerTwinklePersistanceMap[pixelIndex] = fastRandomInteger(800);
      }
    }
    else 
    {
      if (starShimmerMap[pixelIndex] > starShimmerSpeedMap[pixelIndex] * frameDelta)
      {
        starShimmerMap[pixelIndex] -= starShimmerSpeedMap[pixelIndex] * frameDelta;
      }
      else 
      {
        starShimmerMap[pixelIndex] = 0;
        starShimmerCurrentTargetValueMap[pixelIndex] = 0;
      }
    }
  }
  if (starShimmerMap[pixelIndex] == 0) return {0,0,0};

  int shimmerAmount = fastRandomInteger(starShimmerTwinklePersistanceMap[pixelIndex]) - (starShimmerTwinklePersistanceMap[pixelIndex] / 2);
  if (starShimmerTwinkleDirectionMap[pixelIndex])
  {
    starShimmerTwinkleDirectionMap[pixelIndex] = false;
    shimmerAmount += starShimmerTwinkleMap[pixelIndex];
  }
  else 
  {
    starShimmerTwinkleDirectionMap[pixelIndex] = true;
    shimmerAmount -= starShimmerTwinkleMap[pixelIndex];
  }
  if (starShimmerMap[pixelIndex] + shimmerAmount > 0 && starShimmerMap[pixelIndex] + shimmerAmount <= 65535)
  {
    starShimmerMap[pixelIndex] += shimmerAmount;
  }
  return colorFromPalette(paletteOffset + starShimmerColorMap[pixelIndex], starShimmerMap[pixelIndex]);
}
