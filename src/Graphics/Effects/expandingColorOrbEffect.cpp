
#include "../../Graphics/Effects/expandingColorOrbEffect.h"
#include "../../Graphics/palettes.h"
#include "../../Graphics/transformMaps.h"
#include "../../Utility/fastRandom.h"

const byte MaxNumberOfColorOrbs = 10;
const int MaxColorOrbRadius = 100;
const int MaxColorOrbFlashSpeed = 100;
const int MinColorOrbFlashSpeed = 12;
int colorOrbs[MaxNumberOfColorOrbs] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
float colorOrbFlashSpeed[MaxNumberOfColorOrbs];
float colorOrbRadii[MaxNumberOfColorOrbs];
int colorOrbColor[MaxNumberOfColorOrbs];
Color expandingColorOrbEffect(int frameDelta, int pixelIndex, int paletteOffset, uint16_t globalBrightnessModifier)
{
  if (fastRandomInteger(1000) < frameDelta * 2)
  {
    for (int i = 0; i < MaxNumberOfColorOrbs; i++)
    {
      if (colorOrbs[i] == -1)
      {
        colorOrbs[i] = pixelIndex;
        colorOrbFlashSpeed[i] = 0.001 * (MinColorOrbFlashSpeed + fastRandomInteger(MaxColorOrbFlashSpeed - MinColorOrbFlashSpeed));
        colorOrbRadii[i] = 0;
        colorOrbColor[i] = paletteOffset + fastRandomByte();
      }
    }
  }

  int closestOrb = -1;
  int closestDistance = 300;
  for (int i = 0; i < MaxNumberOfColorOrbs; i++)
  {
    if (colorOrbs[i] == -1) continue;
    if (colorOrbs[i] == pixelIndex)
    {
      colorOrbRadii[i] += colorOrbFlashSpeed[i] * frameDelta;
      if (colorOrbRadii[i] > MaxColorOrbRadius)
      {
          colorOrbRadii[i] = 0;
          colorOrbs[i] = -1;
          continue;
      }
    }

    if (pixelRadiusTransformMaps[colorOrbs[i]][pixelIndex] < colorOrbRadii[i])
    {
      if (pixelRadiusTransformMaps[colorOrbs[i]][pixelIndex] < closestDistance)
      {
        closestDistance = pixelRadiusTransformMaps[colorOrbs[i]][pixelIndex];
        closestOrb = i;
      }
    }
  }
  if (closestOrb != -1) return colorFromPalette(colorOrbColor[closestOrb], globalBrightnessModifier);
  return {0, 0, 0};
}