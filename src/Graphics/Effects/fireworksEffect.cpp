
#include "../../Graphics/Effects/fireworksEffect.h"
#include "../../Graphics/palettes.h"
#include "../../Graphics/transformMaps.h"
#include "../../Utility/fastRandom.h"
#include "../../settings.h"

const int MaxNumberOfFireworks = 10;
int currentNumberOfFireworks;
float fireworkSizes[MaxNumberOfFireworks];
int fireworkTypes[MaxNumberOfFireworks];
float fireworkVelocities[MaxNumberOfFireworks];
float fireworkStarSizes[MaxNumberOfFireworks];
bool fireworkIsActive[MaxNumberOfFireworks];
uint16_t fireworkCenterPixel[MaxNumberOfFireworks];

Color fireworksEffect(int frameDelta, uint32_t colorFrame, int pixelIndex, int fireworkProbability, int paletteOffset, uint16_t globalBrightnessModifier)
{
  if (pixelIndex == 0)
  {
    for (int firework = 0; firework < MaxNumberOfFireworks; firework++)
    {
      if (!fireworkIsActive[firework]) continue;
      fireworkSizes[firework] += (fireworkVelocities[firework] / 5000.0) * frameDelta;
      if (fireworkVelocities[firework] > frameDelta) fireworkVelocities[firework] -= frameDelta;
      else 
      {
        fireworkVelocities[firework] = 0;
        fireworkIsActive[firework] = false;
        currentNumberOfFireworks--;
      }
    }

    if (currentNumberOfFireworks < MaxNumberOfFireworks && fastRandomInteger(30000) < frameDelta * fireworkProbability)
    {
        int emptyIndex;
        for (int firework = 0; firework < MaxNumberOfFireworks; firework++)
        {
          if (!fireworkIsActive[firework])
          {
            emptyIndex = firework;
            break;
          }
        }

        fireworkIsActive[emptyIndex] = true;
        fireworkSizes[emptyIndex] = 0;
        fireworkTypes[emptyIndex] = 20;
        fireworkCenterPixel[emptyIndex] = fastRandomInteger(TOTAL_LEDS);
        fireworkVelocities[emptyIndex] = 1000;
        fireworkStarSizes[emptyIndex] = fastRandomInteger(25) + 5;
        currentNumberOfFireworks++;
    }
  }

  for (int firework = 0; firework < MaxNumberOfFireworks; firework++)
  {
    if (!fireworkIsActive[firework]) continue;
    uint16_t centerPixel = fireworkCenterPixel[firework];
    uint8_t distance = pixelRadiusTransformMaps[centerPixel][pixelIndex];
    uint8_t angle = pixelAngleTransformMaps[centerPixel][pixelIndex];
    float size = fireworkSizes[firework];
    // if (angle > 128) angle -= distance / 8;
    // else angle += distance / 8;

    uint8_t color8Bit = angle + (colorFrame >> 1);

    if ((distance < size) && ((distance + fireworkStarSizes[firework]) > size))
    {
      if (angle % (fireworkTypes[firework]) < (fireworkTypes[firework] >> 1))
      {
        return colorFromPalette(paletteOffset + color8Bit, globalBrightnessModifier);
        return {65535, 0, 0};
      }
    }
    if (size > 80)
    {
      if ((distance < (size / 2)) && ((distance + fireworkStarSizes[firework]) > (size / 2)))
      {
        if (angle % (fireworkTypes[firework]) < (fireworkTypes[firework] >> 1))
        {
          return colorFromPalette(paletteOffset + color8Bit, globalBrightnessModifier);
          return {65535, 0, 0};
        }
      }
    }
  }
  return {0, 0, 0};
}