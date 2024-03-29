
#include "../../Graphics/Effects/rainShowerEffect.h"
#include "../../Graphics/palettes.h"
#include "../../Graphics/directionMaps.h"
#include "../../Utility/fastRandom.h"
#include "../../settings.h"

int rainDropChanceBoost = 500;
bool isRainDropAtYPosition[256];
float rainDropXPositions[256];
float rainDropXVelocities[256];
bool doesLedHaveWater[TOTAL_LEDS];
const float rainDropGravity = .00017;
Color rainShowerEffect(int pixelIndex, Effect *effect)
{
  if ((*effect->transformMap1)[pixelIndex] == 0)
  {
    if (!isRainDropAtYPosition[(*effect->transformMap2)[pixelIndex]])
    {
      if (fastRandomInteger(5000) < *(effect->frameTimeDelta) + (rainDropChanceBoost / 4))
      {
        isRainDropAtYPosition[(*effect->transformMap2)[pixelIndex]] = true;
        rainDropXPositions[(*effect->transformMap2)[pixelIndex]] = -effect->size;
        rainDropXVelocities[(*effect->transformMap2)[pixelIndex]] = 0;
      }
      if (rainDropChanceBoost != 0) rainDropChanceBoost -= *(effect->frameTimeDelta);
      if (rainDropChanceBoost < 0) rainDropChanceBoost = 0;
    }
    else 
    {
      rainDropXVelocities[(*effect->transformMap2)[pixelIndex]] += rainDropGravity * *(effect->frameTimeDelta);
      rainDropXPositions[(*effect->transformMap2)[pixelIndex]] += rainDropXVelocities[(*effect->transformMap2)[pixelIndex]] * *(effect->frameTimeDelta);
      if (rainDropXPositions[(*effect->transformMap2)[pixelIndex]] > 300)
      {
        isRainDropAtYPosition[(*effect->transformMap2)[pixelIndex]] = false;
      }
    }
  }

  if (isRainDropAtYPosition[(*effect->transformMap2)[pixelIndex]])
  {
    if (isBottomLed[pixelIndex] != -1) 
    {
      bool isDropInLowerBound =  (*effect->transformMap1)[pixelIndex] > rainDropXPositions[(*effect->transformMap2)[pixelIndex]] ;
      bool isDropInUpperBound = (*effect->transformMap1)[pixelIndex] < rainDropXPositions[(*effect->transformMap2)[pixelIndex]] + effect->size ;
      if (isDropInLowerBound && isDropInUpperBound)
      {
        doesLedHaveWater[pixelIndex] = true;
        return colorFromPalette(effect->currentPaletteOffset, (*effect->globalBrightnessPointer));
      }
    }
    else 
    {
      int distanceFromUpperBound = (*effect->transformMap1)[pixelIndex] - rainDropXPositions[(*effect->transformMap2)[pixelIndex]];
      bool isDropInLowerBound =  (*effect->transformMap1)[pixelIndex] > rainDropXPositions[(*effect->transformMap2)[pixelIndex]] ;
      bool isDropInUpperBound = (*effect->transformMap1)[pixelIndex] < rainDropXPositions[(*effect->transformMap2)[pixelIndex]] + effect->size ;
      if (isDropInLowerBound && isDropInUpperBound)
      {
        return colorFromPalette(effect->currentPaletteOffset, ((((*effect->globalBrightnessPointer) + 1) / (effect->size + 1)) * distanceFromUpperBound));
      }
    }
  }
  if (doesLedHaveWater[pixelIndex]) 
  {
    if (fastRandomInteger(150) < *(effect->frameTimeDelta))
    {
      doesLedHaveWater[pixelIndex] = false;
      if (fastRandomInteger(6) > 1)
      {
      int drainagePixel = isBottomLed[pixelIndex];
      if (drainagePixel != -1)
      {
        doesLedHaveWater[drainagePixel] = true;
      }
      }
    }
    return colorFromPalette(effect->currentPaletteOffset, (*effect->globalBrightnessPointer));
  }
  return {0,0,0};
}

void boostRainChance()
{
  rainDropChanceBoost = 500;
}