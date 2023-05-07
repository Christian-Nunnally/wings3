#include <Arduino.h>
#include <pgmspace.h>
#include "../Peripherals/microphone.h"
#include "../Graphics/graphics.h"
#include "../Graphics/transformMaps.h"
#include "../Graphics/directionMaps.h"
#include "../Utility/fastRandom.h"
#include "../settings.h"

#include "palettes.h"
//#include "palettesMinimal.h"

uint16_t squareRoot32Bit(int input);
inline Color fadeToBlack(Color color, float fadeValue);

Color currentLedColorMap[TOTAL_LEDS];

Color colorFromPalette(int index, uint16_t brightness)
{
  uint8_t red = palette[index * 3 + 0];
  uint8_t green = palette[index * 3 + 1];
  uint8_t blue = palette[index * 3 + 2];
  return { 
    (uint16_t)((red * brightness) >> 8), 
    (uint16_t)((green * brightness) >> 8),
    (uint16_t)((blue * brightness) >> 8)
  };
}

Color wavePulse(uint32_t brightnessFrame, uint32_t colorFrame, int pixelIndex, const uint8_t brightnessProjectionMap[], const uint8_t colorProjectionMap[], int paletteOffset, uint16_t globalBrightnessModifier) 
{
    uint8_t brightnessFrame8bit = (brightnessFrame / 4);
    uint8_t colorFrame8bit = (colorFrame / 10);
    uint8_t brightnessStartingPoint = brightnessProjectionMap[pixelIndex];
    uint8_t colorStartingPoint = colorProjectionMap[pixelIndex];
    uint16_t brightness = ((uint8_t)(brightnessStartingPoint - brightnessFrame8bit) << 8);
    brightness = (brightness * globalBrightnessModifier) >> 16;
    uint8_t colorFramePreOffset = colorStartingPoint + colorFrame8bit;
    return colorFromPalette(paletteOffset + colorFramePreOffset, brightness);
}

uint16_t starShimmerMap[TOTAL_LEDS];
uint16_t starShimmerCurrentTargetValueMap[TOTAL_LEDS];
uint16_t starShimmerSpeedMap[TOTAL_LEDS];
uint16_t starShimmerColorMap[TOTAL_LEDS];
uint16_t starShimmerTwinkleMap[TOTAL_LEDS];
uint16_t starShimmerTwinklePersistanceMap[TOTAL_LEDS];
bool starShimmerTwinkleDirectionMap[TOTAL_LEDS];
Color starShimmer(int frameDelta, int fadeAmount, uint16_t newStarLikelihood, int pixelIndex, int paletteOffset, uint16_t globalBrightnessModifier) 
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

Color meteorRain(int frameDelta, uint32_t animationFrame, uint32_t colorFrame, int pixelIndex, byte meteorSize, float meteorTrailDecay, const uint8_t projectionMap[], int paletteOffset, uint16_t globalBrightnessModifier)
{
  uint32_t projectionMap1FrameWrapping = animationFrame % (256 + meteorSize * 10);
  if (fastRandomInteger(90) < frameDelta)
  {
    currentLedColorMap[pixelIndex] = fadeToBlack(currentLedColorMap[pixelIndex], meteorTrailDecay );        
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

Color meteorRain2(int frameDelta, uint32_t projectionMap1Frame, uint32_t projectionMap2Frame, uint32_t colorFrame, int pixelIndex, byte meteorSize, float meteorTrailDecay, const uint8_t projectionMap[], const uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier)
{
  uint32_t projectionMap1FrameWrapping = projectionMap1Frame % (256 + meteorSize * 10);
  uint32_t projectionMap2FrameWrapping = projectionMap2Frame % (256 + meteorSize * 10);
  if (fastRandomInteger(90) < frameDelta)
  {
    currentLedColorMap[pixelIndex] = fadeToBlack(currentLedColorMap[pixelIndex], meteorTrailDecay );        
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

int rainDropChanceBoost = 500;
bool isRainDropAtYPosition[256];
float rainDropXPositions[256];
float rainDropXVelocities[256];
bool doesLedHaveWater[TOTAL_LEDS];
const float rainDropGravity = .0002;
const uint16_t bottomLeds[TOTAL_LEDS] = {12,14,17,19,21,23,24,26,27,29,30,32,33,34,36,37,38,39,40,42,43,179,178,176,175,174,173,172,170,169,168,166,165,163,162,160,159,157,155,153,150,148};
const int isBottomLed[TOTAL_LEDS] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 14, -1, 17, -1, -1, 19, -1, 21, -1, 23, -1, 24, 26, -1, 27, 29, -1, 30, 32, -1, 33, 34, 36, -1, 37, 38, 39, 40, 42, -1, 43, 179, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 150, -1, 153, -1, -1, 155, -1, 157, -1, 159, -1, 160, 162, -1, 163, 165, -1, 166, 168, -1, 169, 170, 172, -1, 173, 174, 175, 176, 178, -1, 179, 43, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
Color rainShower(int frameDelta, uint32_t rainAnimationFrame, int pixelIndex, byte rainLength, const uint8_t projectionMap[], const uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier)
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
Color lightningBug(int frameDelta, int pixelIndex, int paletteOffset)
{
  if (fastRandomInteger(30000) < frameDelta * 2)
  {
    for (int i = 0; i < MaxNumberOfLightningBugs; i++)
    {
      if (lightningBugs[i] == -1)
      {
        lightningBugs[i] = pixelIndex;
        lightningBugsBrightnessTarget[i] = MinLightningBugBrightnessTarget + fastRandomInteger(MaxLightningBugBrightnessTarget - MinLightningBugBrightnessTarget);
        lightningBugRadiiTarget[i] = MinLightningBugRadius + fastRandomInteger(MaxLightningBugRadius - MinLightningBugRadius);
        lightningBugFlashSpeed[i] = 0.001 * (MinLightningBugFlashSpeed + fastRandomInteger(MaxLightningBugFlashSpeed - MinLightningBugFlashSpeed));
        lightningBugRadii[i] = fastRandomInteger(lightningBugRadiiTarget[i] / 2);
      }
    }
  }

  for (int i = 0; i < MaxNumberOfLightningBugs; i++)
  {
    if (lightningBugs[i] == -1) continue;
    if (lightningBugs[i] == pixelIndex)
    {
      lightningBugsBrightness[i] += (lightningBugsBrightnessTarget[i] - lightningBugsBrightness[i]) * lightningBugFlashSpeed[i] * frameDelta;
      lightningBugRadii[i] += (lightningBugRadiiTarget[i] - lightningBugRadii[i]) * lightningBugFlashSpeed[i] * frameDelta;
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
      uint16_t brightness = (float)((lightningBugRadii[i] - pixelRadiusTransformMaps[lightningBugs[i]][pixelIndex]) / lightningBugRadii[i]) * lightningBugsBrightness[i];
      return colorFromPalette(paletteOffset, brightness);
    }
  }
  return {0, 0, 0};
}

const byte MaxNumberOfColorOrbs = 10;
const int MaxColorOrbRadius = 100;
const int MaxColorOrbFlashSpeed = 100;
const int MinColorOrbFlashSpeed = 12;
int colorOrbs[MaxNumberOfColorOrbs] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
float colorOrbFlashSpeed[MaxNumberOfColorOrbs];
float colorOrbRadii[MaxNumberOfColorOrbs];
int colorOrbColor[MaxNumberOfColorOrbs];
Color colorOrb(int frameDelta, int pixelIndex, int paletteOffset, uint16_t globalBrightnessModifier)
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

const uint32_t gamma_lut[256] = {
     0,1355,2201,2923,3576,4180,4749,5290,5808,6308,6790,7259,7715,8159,8594,9019,
  9436,9845,10247,10642,11031,11414,11792,12165,12533,12896,13255,13610,13961,14308,14651,14992,
  15329,15662,15993,16321,16646,16968,17288,17605,17920,18232,18543,18851,19156,19460,19762,20062,
  20359,20655,20950,21242,21533,21822,22109,22395,22679,22962,23243,23523,23801,24078,24354,24628,
  24901,25173,25443,25713,25981,26248,26513,26778,27041,27304,27565,27825,28084,28343,28600,28856,
  29111,29365,29619,29871,30122,30373,30623,30872,31120,31367,31613,31858,32103,32347,32590,32832,
  33074,33315,33555,33794,34033,34270,34508,34744,34980,35215,35449,35683,35916,36149,36381,36612,
  36842,37072,37302,37530,37759,37986,38213,38440,38665,38891,39115,39339,39563,39786,40009,40231,
  40452,40673,40894,41114,41333,41552,41770,41988,42206,42423,42639,42855,43071,43286,43501,43715,
  43929,44142,44355,44567,44779,44991,45202,45413,45623,45833,46043,46252,46460,46669,46876,47084,
  47291,47498,47704,47910,48116,48321,48526,48730,48934,49138,49341,49544,49747,49949,50151,50353,
  50554,50755,50955,51155,51355,51555,51754,51953,52152,52350,52548,52745,52943,53140,53336,53533,
  53729,53924,54120,54315,54510,54704,54899,55092,55286,55479,55673,55865,56058,56250,56442,56634,
  56825,57016,57207,57397,57588,57778,57968,58157,58346,58535,58724,58912,59100,59288,59476,59663,
  59851,60038,60224,60411,60597,60783,60968,61154,61339,61524,61709,61893,62077,62261,62445,62629,
  62812,62995,63178,63361,63543,63725,63907,64089,64270,64452,64633,64814,64994,65175,65355,65535,
  };

int lastTimeFireMovedUp = 0;
const int FireMovementFrameSpeed = 50;
Color fireAnimation(int frameDelta, int pixelIndex, int flameDecay, int sparks, const uint8_t projectionMap[], const uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier)
{
    static uint16_t heat1[TOTAL_LEDS];
    static uint16_t heat2[TOTAL_LEDS];
    static uint16_t* heatAPointer[TOTAL_LEDS];
    static uint16_t* heatBPointer[TOTAL_LEDS];
    static int cooldown;
    static bool shouldFireMoveUpThisFrame;
    static bool currentHeatBackBuffer;

    if (pixelIndex == 0)
    {
      if (shouldFireMoveUpThisFrame) 
      {
        shouldFireMoveUpThisFrame = false;
        if (currentHeatBackBuffer)
        {
          *heatAPointer = heat1;
          *heatBPointer = heat2;
        }
        else 
        {
          *heatAPointer = heat2;
          *heatBPointer = heat1;
        }
        currentHeatBackBuffer = !currentHeatBackBuffer;
      }
      cooldown = fastRandomInteger(flameDecay * frameDelta);
      lastTimeFireMovedUp += frameDelta;
      if (lastTimeFireMovedUp > FireMovementFrameSpeed)
      {
        lastTimeFireMovedUp = 0;
        shouldFireMoveUpThisFrame = true;
      }
    }

    if(cooldown > (*heatAPointer)[pixelIndex]) (*heatAPointer)[pixelIndex] = 0;
    else (*heatAPointer)[pixelIndex] -= cooldown;

    if ((isBottomLed[pixelIndex] != -1) && fastRandomInteger(30000) < sparks * frameDelta) 
    {
      (*heatAPointer)[pixelIndex] = (uint16_t)fastRandomInteger(30000) + 35535;
    }

    if (shouldFireMoveUpThisFrame)
    {
      uint16_t southernLed1 = southernLedMap[pixelIndex];
      if (southernLed1 != TOTAL_LEDS && fastRandomByte() < 100)
      {
        uint16_t southernLed2 = southernLedMap[southernLed1];
        if (southernLed2 != TOTAL_LEDS) (*heatBPointer)[pixelIndex] = (((*heatAPointer)[southernLed1] / 2) + ((*heatAPointer)[southernLed2] / 2));// + ((*heatAPointer)[southernLed2] / 3));
        else (*heatBPointer)[pixelIndex] = (*heatAPointer)[southernLed1];
      }
      else (*heatBPointer)[pixelIndex] = (*heatAPointer)[pixelIndex];
    }
    uint16_t brightness = gamma_lut[(*heatAPointer)[pixelIndex] >> 8];
    return colorFromPalette(((*heatAPointer)[pixelIndex] >> 10) + paletteOffset, brightness);
}

Color simpleColor(int colorFrame, int paletteOffset, uint16_t globalBrightnessModifier)
{
  uint8_t colorFrame8bit = (colorFrame >> 4);
  return colorFromPalette(colorFrame8bit + paletteOffset, globalBrightnessModifier);
}

const int MaxNumberOfFireworks = 10;
int currentNumberOfFireworks;
float fireworkSizes[MaxNumberOfFireworks];
int fireworkTypes[MaxNumberOfFireworks];
float fireworkVelocities[MaxNumberOfFireworks];
float fireworkStarSizes[MaxNumberOfFireworks];
bool fireworkIsActive[MaxNumberOfFireworks];
uint16_t fireworkCenterPixel[MaxNumberOfFireworks];

Color fireworks(int frameDelta, uint32_t colorFrame, int pixelIndex, int fireworkProbability, int paletteOffset, uint16_t globalBrightnessModifier)
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

Color lightChase(uint32_t ledTestAnimationFrame, int pixelIndex, uint32_t colorAnimationFrame, int paletteOffset, int mod, uint16_t globalBrightnessModifier)
{
  uint8_t colorFrame8bit = colorAnimationFrame;
  if ((ledTestAnimationFrame >> 8) % 5 == pixelIndex % mod) return colorFromPalette(paletteOffset + colorFrame8bit, globalBrightnessModifier);
  return {0,0,0};
}

inline Color fadeToBlack(Color color, float fadeValue) {
    uint16_t r, g, b;
    r = (color.red <= 10) ? 0 : color.red - (color.red * fadeValue);
    g = (color.green <= 10) ? 0 : color.green - (color.green * fadeValue);
    b = (color.blue <= 10) ? 0 : color.blue - (color.blue * fadeValue);
    return {r,g,b};
}