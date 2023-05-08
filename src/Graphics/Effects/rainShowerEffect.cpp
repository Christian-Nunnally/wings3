
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
const float rainDropGravity = .0002;
Color rainShowerEffect(int frameDelta, uint32_t rainAnimationFrame, int pixelIndex, byte rainLength, const uint8_t projectionMap[], const uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier)
{
  int rainAnimationFrame2 = rainAnimationFrame % 512;
  if (projectionMap[pixelIndex] == 0)
  {
    if (!isRainDropAtYPosition[projectionMap2[pixelIndex]])
    {
      if (fastRandomInteger(5000) < frameDelta + (rainDropChanceBoost / 4))
      {
        isRainDropAtYPosition[projectionMap2[pixelIndex]] = true;
        rainDropXPositions[projectionMap2[pixelIndex]] = -rainLength;
        rainDropXVelocities[projectionMap2[pixelIndex]] = 0;
      }
      if (rainDropChanceBoost != 0) rainDropChanceBoost -= frameDelta;
      if (rainDropChanceBoost < 0) rainDropChanceBoost = 0;
    }
    else 
    {
      rainDropXVelocities[projectionMap2[pixelIndex]] += rainDropGravity * frameDelta;
      rainDropXPositions[projectionMap2[pixelIndex]] += rainDropXVelocities[projectionMap2[pixelIndex]] * frameDelta;
      if (rainDropXPositions[projectionMap2[pixelIndex]] > 300)
      {
        isRainDropAtYPosition[projectionMap2[pixelIndex]] = false;
      }
    }
  }

  if (isRainDropAtYPosition[projectionMap2[pixelIndex]])
  {
    if (isBottomLed[pixelIndex] != -1) 
    {
      bool isDropInLowerBound =  projectionMap[pixelIndex] > rainDropXPositions[projectionMap2[pixelIndex]] ;//+ rainAnimationFrame2;
      bool isDropInUpperBound = projectionMap[pixelIndex] < rainDropXPositions[projectionMap2[pixelIndex]] + rainLength ;//+ rainAnimationFrame2;
      if (isDropInLowerBound && isDropInUpperBound)
      {
        doesLedHaveWater[pixelIndex] = true;
        return colorFromPalette(paletteOffset, globalBrightnessModifier);
      }
    }
    else 
    {
      int distanceFromUpperBound = projectionMap[pixelIndex] - rainDropXPositions[projectionMap2[pixelIndex]];
      bool isDropInLowerBound =  projectionMap[pixelIndex] > rainDropXPositions[projectionMap2[pixelIndex]] ;//+ rainAnimationFrame2;
      bool isDropInUpperBound = projectionMap[pixelIndex] < rainDropXPositions[projectionMap2[pixelIndex]] + rainLength ;//+ rainAnimationFrame2;
      if (isDropInLowerBound && isDropInUpperBound)
      {
        return colorFromPalette(paletteOffset, ((globalBrightnessModifier / rainLength) * distanceFromUpperBound));
      }
    }
  }
  if (doesLedHaveWater[pixelIndex]) 
  {
    if (fastRandomInteger(150) < frameDelta)
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
    return colorFromPalette(paletteOffset, globalBrightnessModifier);
  }
  return {0,0,0};
}

void boostRainChance()
{
  rainDropChanceBoost = 500;
}