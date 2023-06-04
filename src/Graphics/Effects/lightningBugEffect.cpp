
#include "../../Graphics/Effects/lightningBugEffect.h"
#include "../../Graphics/palettes.h"
#include "../../Graphics/transformMaps.h"
#include "../../Utility/fastRandom.h"

const byte MaxNumberOfLightningBugs = 10;
const int MaxLightningBugRadius = 30;
const int MinLightningBugRadius = 15;
const int MaxLightningBugBrightnessTarget = 65535;
const int MinLightningBugBrightnessTarget = 40535;
const int MaxLightningBugFlashSpeed = 12;
const int MinLightningBugFlashSpeed = 4;
int lightningBugs[MaxNumberOfLightningBugs] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
uint16_t lightningBugsBrightness[MaxNumberOfLightningBugs];
uint16_t lightningBugsBrightnessTarget[MaxNumberOfLightningBugs];
float lightningBugFlashSpeed[MaxNumberOfLightningBugs];
float lightningBugRadii[MaxNumberOfLightningBugs];
float lightningBugRadiiTarget[MaxNumberOfLightningBugs];
void incrementLightningBugEffect(Effect *effect)
{
  if (fastRandomInteger(120) < *(effect->frameTimeDelta))
  {
    for (int i = 0; i < MaxNumberOfLightningBugs; i++)
    {
      if (lightningBugs[i] == -1)
      {
        lightningBugs[i] = fastRandomInteger(TOTAL_LEDS);
        lightningBugsBrightnessTarget[i] = MinLightningBugBrightnessTarget + fastRandomInteger(MaxLightningBugBrightnessTarget - MinLightningBugBrightnessTarget);
        lightningBugRadiiTarget[i] = MinLightningBugRadius + fastRandomInteger(MaxLightningBugRadius - MinLightningBugRadius);
        lightningBugFlashSpeed[i] = 0.001 * (MinLightningBugFlashSpeed + fastRandomInteger(MaxLightningBugFlashSpeed - MinLightningBugFlashSpeed));
        lightningBugRadii[i] = fastRandomInteger(lightningBugRadiiTarget[i] / 2);
      }
    }
  }  
}

Color lightningBugEffect(int pixelIndex, Effect *effect)
{
  for (int i = 0; i < MaxNumberOfLightningBugs; i++)
  {
    if (lightningBugs[i] == -1) continue;
    if (lightningBugs[i] == pixelIndex)
    {
      lightningBugsBrightness[i] += (lightningBugsBrightnessTarget[i] - lightningBugsBrightness[i]) * lightningBugFlashSpeed[i] * *(effect->frameTimeDelta);
      lightningBugRadii[i] += (lightningBugRadiiTarget[i] - lightningBugRadii[i]) * lightningBugFlashSpeed[i] * *(effect->frameTimeDelta);
      if (abs(lightningBugsBrightnessTarget[i] - lightningBugsBrightness[i]) < 3000)
      {
          if (lightningBugsBrightnessTarget[i] == 0)
          {
            lightningBugsBrightness[i] = 0;
            lightningBugRadii[i] = 1.0f;
            lightningBugs[i] = -1;
            continue;
          }
          if (fastRandomByte() < 128) lightningBugsBrightnessTarget[i] = fastRandomInteger(30000);
          else lightningBugsBrightnessTarget[i] = 0;
      }
    }

    if (pixelRadiusTransformMaps[lightningBugs[i]][pixelIndex] < lightningBugRadii[i])
    {
      uint16_t brightness = (float)((lightningBugRadii[i] - pixelRadiusTransformMaps[lightningBugs[i]][pixelIndex]) / lightningBugRadii[i]) * (lightningBugsBrightness[i] >> 8);
      return colorFromPalette(effect->currentPaletteOffset, brightness);
    }
  }
  return {0, 0, 0};
}