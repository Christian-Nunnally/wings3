
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
void incrementStarFieldEffect(Effect *effect, int fadeAmount, int16_t newStarLikelihood)
{
  if (fastRandomInteger(10000) < newStarLikelihood * *(effect->frameTimeDelta))
  {
    int chosenLed = fastRandomInteger(TOTAL_LEDS);
    starShimmerCurrentTargetValueMap[chosenLed] = fastRandomInteger((*effect->globalBrightnessPointer));
    starShimmerSpeedMap[chosenLed] = 2 * fastRandomInteger(1, fadeAmount);
    starShimmerColorMap[chosenLed] = fastRandomInteger(PALETTE_LENGTH) + effect->currentPaletteOffset;
    starShimmerTwinkleMap[chosenLed] = 0;
    starShimmerTwinklePersistanceMap[chosenLed] = 0;
    starShimmerTwinkleDirectionMap[chosenLed] = 0;
  }
}


Color starFieldEffect(int pixelIndex, Effect *effect, int fadeAmount) 
{
  if (starShimmerCurrentTargetValueMap[pixelIndex] > 0)
  {
    if(starShimmerMap[pixelIndex] < starShimmerCurrentTargetValueMap[pixelIndex])
    {
      if (starShimmerMap[pixelIndex] + starShimmerSpeedMap[pixelIndex] * *(effect->frameTimeDelta) < starShimmerCurrentTargetValueMap[pixelIndex])
      {
        starShimmerMap[pixelIndex] += starShimmerSpeedMap[pixelIndex] * *(effect->frameTimeDelta);
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
      if (starShimmerMap[pixelIndex] > starShimmerSpeedMap[pixelIndex] * *(effect->frameTimeDelta))
      {
        starShimmerMap[pixelIndex] -= starShimmerSpeedMap[pixelIndex] * *(effect->frameTimeDelta);
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
  return colorFromPalette(starShimmerColorMap[pixelIndex], starShimmerMap[pixelIndex]);
}
