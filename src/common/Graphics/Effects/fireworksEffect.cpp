
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
uint8_t fireworkColorType[MaxNumberOfFireworks];
uint8_t fireworkTargetSize[MaxNumberOfFireworks];
uint16_t fireworkCenterPixel[MaxNumberOfFireworks];

void incrementFireworksEffect(Effect *effect, int fireworkProbability)
{
  for (int firework = 0; firework < MaxNumberOfFireworks; firework++)
  {
    if (!fireworkIsActive[firework]) continue;
    fireworkSizes[firework] += (fireworkVelocities[firework] / 7000.0) * *(effect->frameTimeDelta);
    if (fireworkVelocities[firework] > *(effect->frameTimeDelta)) fireworkVelocities[firework] -= *(effect->frameTimeDelta);
    else 
    {
      fireworkVelocities[firework] = 0;
      fireworkIsActive[firework] = false;
      currentNumberOfFireworks--;
    }
    if (fireworkSizes[firework] > fireworkTargetSize[firework])
    {
      fireworkVelocities[firework] = 0;
      fireworkIsActive[firework] = false;
      currentNumberOfFireworks--;
    }
  }

  if (fastRandomInteger(60000) < *(effect->frameTimeDelta) * fireworkProbability)
  {
    triggerFirework();
  }
}

Color fireworksEffect(int pixelIndex, Effect *effect)
{
  for (int firework = 0; firework < MaxNumberOfFireworks; firework++)
  {
    if (!fireworkIsActive[firework]) continue;
    uint16_t centerPixel = fireworkCenterPixel[firework];
    uint8_t distance = pixelRadiusTransformMaps[centerPixel][pixelIndex];
    uint8_t angle = pixelAngleTransformMaps[centerPixel][pixelIndex];
    uint8_t colorType = fireworkColorType[firework];
    float size = fireworkSizes[firework];

    uint8_t color8Bit;
    if (colorType == 0) color8Bit = (effect->time1 >> 8);
    else if (colorType == 1) color8Bit = angle + (effect->time1 >> 8);
    else if (colorType == 2) color8Bit = distance + (effect->time1 >> 8);
    else if (colorType == 3) color8Bit = angle + (effect->time1 >> 5);
    else if (colorType == 4) color8Bit = distance + (effect->time1 >> 5);
    else if (colorType == 5) color8Bit = angle + (effect->time1 >> 4);
    else if (colorType == 6) color8Bit = distance + (effect->time1 >> 4);
    else if (colorType == 7) color8Bit = angle + (effect->time1 >> 2);
    else if (colorType == 8) color8Bit = distance + (effect->time1 >> 2);
    else if (colorType == 9) color8Bit = (*effect->transformMap1)[pixelIndex];

    if ((distance < size) && ((distance + fireworkStarSizes[firework]) > size))
    {
      if (angle % (fireworkTypes[firework]) < (fireworkTypes[firework] >> 1))
      {
        return colorFromPalette(effect->currentPaletteOffset + color8Bit, (*effect->globalBrightnessPointer));
      }
    }
    if (size > 80)
    {
      if ((distance < (size / 2)) && ((distance + fireworkStarSizes[firework]) > (size / 2)))
      {
        if (angle % (fireworkTypes[firework]) < (fireworkTypes[firework] >> 1))
        {
          return colorFromPalette(effect->currentPaletteOffset + color8Bit, (*effect->globalBrightnessPointer));
        }
      }
    }
  }
  return {0, 0, 0};
}

void triggerFirework()
{
  if (currentNumberOfFireworks >= MaxNumberOfFireworks) return;

  int index = 0;
  for (; index < MaxNumberOfFireworks; index++)
  {
    if (!fireworkIsActive[index]) break;
  }

  fireworkIsActive[index] = true;
  fireworkSizes[index] = 0;
  fireworkTypes[index] = fastRandomInteger(15, 40);
  fireworkCenterPixel[index] = fastRandomInteger(TOTAL_LEDS);
  fireworkVelocities[index] = fastRandomInteger(700, 1300);
  fireworkStarSizes[index] = fastRandomInteger(35) + 1;
  fireworkColorType[index] = fastRandomInteger(10);
  fireworkTargetSize[index] = fastRandomInteger(50, 200);
  currentNumberOfFireworks++;
}