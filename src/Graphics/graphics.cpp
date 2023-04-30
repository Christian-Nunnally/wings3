#include <Adafruit_NeoPXL8.h>
#include <pgmspace.h>
#include "../Peripherals/microphone.h"
#include "../Graphics/graphics.h"
#include "../Graphics/transformMaps.h"
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
    uint8_t colorFrame8bit = (colorFrame / 15);
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
      if (fastRandomInteger(10000) < frameDelta * 2)
      {
        isRainDropAtYPosition[projectionMap2[pixelIndex]] = true;
        rainDropXPositions[projectionMap2[pixelIndex]] = -rainLength;
        rainDropXVelocities[projectionMap2[pixelIndex]] = 0;
      }
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
        return colorFromPalette(paletteOffset, 65535);
      }
    }
    else 
    {
      int distanceFromUpperBound = projectionMap[pixelIndex] - rainDropXPositions[projectionMap2[pixelIndex]];
      bool isDropInLowerBound =  projectionMap[pixelIndex] > rainDropXPositions[projectionMap2[pixelIndex]] ;//+ rainAnimationFrame2;
      bool isDropInUpperBound = projectionMap[pixelIndex] < rainDropXPositions[projectionMap2[pixelIndex]] + rainLength ;//+ rainAnimationFrame2;
      if (isDropInLowerBound && isDropInUpperBound)
      {
        return colorFromPalette(paletteOffset, ((65535 / rainLength) * distanceFromUpperBound));
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
    return colorFromPalette(paletteOffset, 65535);
  }
  return {0,0,0};
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

Color ledTest(uint32_t ledTestAnimationFrame, int pixelIndex, uint32_t colorAnimationFrame, int paletteOffset, int mod, uint16_t globalBrightnessModifier)
{
  uint8_t colorFrame8bit = colorAnimationFrame;
  if ((ledTestAnimationFrame >> 9) % 5 == pixelIndex % mod) return colorFromPalette(paletteOffset + colorFrame8bit, globalBrightnessModifier);
  return {0,0,0};
}

Color blendColorsUsingMixing(Color color1, Color color2, uint16_t blendFactor)
{
  float blendFactorFloat = blendFactor / 65535.0;
  int redResult = ((1 - blendFactorFloat) * color1.red * color1.red) + (blendFactorFloat * color2.red * color2.red);
  int greenResult = ((1 - blendFactorFloat) * color1.green * color1.green) + (blendFactorFloat * color2.green * color2.green);
  int blueResult = ((1 - blendFactorFloat) * color1.blue * color1.blue) + (blendFactorFloat * color2.blue * color2.blue);
  uint16_t red = squareRoot32Bit(redResult);
  uint16_t green = squareRoot32Bit(greenResult);
  uint16_t blue = squareRoot32Bit(blueResult);
  return {red, green, blue};
}

inline uint16_t squareRoot32Bit(int input)
{
  uint16_t index = input >> 18;
  return pgm_read_word(squareRootMapFor32BitInputs + index);
}

Color blendColorsUsingMixing2(Color color1, Color color2, uint16_t blendFactor)
{
  float blendFactorFloat = blendFactor / 65535.0;
  uint16_t red = sqrt(((1 - blendFactorFloat) * color1.red * color1.red) + (blendFactorFloat * color2.red * color2.red));
  uint16_t green = sqrt(((1 - blendFactorFloat) * color1.green * color1.green) + (blendFactorFloat * color2.green * color2.green));
  uint16_t blue = sqrt(((1 - blendFactorFloat) * color1.blue * color1.blue) + (blendFactorFloat * color2.blue * color2.blue));
  return {red, green, blue};
}

Color blendColorsUsingMixingGlitched(Color color1, Color color2, uint16_t blendFactor)
{
  uint16_t brightnessAdjustedRed1 = ((65535 - blendFactor) * color1.red) >> 16;
  uint16_t color1FactoredRed = (brightnessAdjustedRed1 * color1.red) >> 16;
  uint16_t brightnessAdjustedRed2 = (blendFactor * color2.red) >> 16;
  uint16_t color2FactoredRed = (brightnessAdjustedRed2 * color2.red) >> 16;

  uint16_t brightnessAdjustedGreen1 = ((65535 - blendFactor) * color1.green) >> 16;
  uint16_t color1FactoredGreen = (brightnessAdjustedGreen1 * color1.green) >> 16;
  uint16_t brightnessAdjustedGreen2 = (blendFactor * color2.green) >> 16;
  uint16_t color2FactoredGreen = (brightnessAdjustedGreen2 * color2.green) >> 16;

  uint16_t brightnessAdjustedBlue1 = ((65535 - blendFactor) * color1.blue) >> 16;
  uint16_t color1FactoredBlue = (brightnessAdjustedBlue1 * color1.blue) >> 16;
  uint16_t brightnessAdjustedBlue2 = (blendFactor * color2.blue) >> 16;
  uint16_t color2FactoredBlue = (brightnessAdjustedBlue2 * color2.blue) >> 16;

  uint16_t red = squareRoot32Bit((brightnessAdjustedRed1 * color1.red) + (brightnessAdjustedRed2 * color2.red));
  uint16_t green = squareRoot32Bit((brightnessAdjustedGreen1 * color1.green) + (brightnessAdjustedGreen2 * color2.green));
  uint16_t blue = squareRoot32Bit((brightnessAdjustedBlue1 * color1.blue) + (brightnessAdjustedBlue2 * color2.blue));

  if (red > 20000) green = 0;
  return {red, green, blue};
}

Color blendColorsUsingAdd(Color color1, Color color2, uint16_t blendFactor)
{
  uint16_t color1Red = color1.red;
  uint16_t color2Red = color2.red;
  if (blendFactor > 65535 / 2) color1Red = ((65535 - ((blendFactor - 32768) * 2)) * color1.red) >> 16;
  else color2Red = (blendFactor * 2 * color2.red) >> 16;

  uint16_t color1Green = color1.green;
  uint16_t color2Green = color2.green;
  if (blendFactor > 65535 / 2) color1Green = ((65535 - ((blendFactor - 32768) * 2)) * color1.green) >> 16;
  else color2Green = (blendFactor * 2 * color2.green) >> 16;

  uint16_t color1Blue = color1.blue;
  uint16_t color2Blue = color2.blue;
  if (blendFactor > 65535 / 2) color1Blue = ((65535 - ((blendFactor - 32768) * 2)) * color1.blue) >> 16;
  else color2Blue = (blendFactor * 2 * color2.blue) >> 16;

  uint16_t r = (color1Red + color2Red) & 0xFFFF;
  uint16_t g =(color1Green + color2Green) & 0xFFFF;
  uint16_t b = (color1Blue + color2Blue) & 0xFFFF;
  return {r, g, b};
}

Color blendColorsUsingSubtract(Color color1, Color color2, uint16_t blendFactor)
{
  uint16_t color1Red = color1.red;
  uint16_t color2Red = color2.red;
  if (blendFactor > 65535 / 2) color1Red = ((65535 - ((blendFactor - 32768) * 2)) * color1.red) >> 16;
  else color2Red = (blendFactor * 2 * color2.red) >> 16;

  uint16_t color1Green = color1.green;
  uint16_t color2Green = color2.green;
  if (blendFactor > 65535 / 2) color1Green = ((65535 - ((blendFactor - 32768) * 2)) * color1.green) >> 16;
  else color2Green = (blendFactor * 2 * color2.green) >> 16;

  uint16_t color1Blue = color1.blue;
  uint16_t color2Blue = color2.blue;
  if (blendFactor > 65535 / 2) color1Blue = ((65535 - ((blendFactor - 32768) * 2)) * color1.blue) >> 16;
  else color2Blue = (blendFactor * 2 * color2.blue) >> 16;

  uint16_t r = (65535 - color1Red - color2Red) & 0xFFFF;
  uint16_t g =(65535 - color1Green - color2Green) & 0xFFFF;
  uint16_t b = (65535 - color1Blue - color2Blue) & 0xFFFF;
  return {r, g, b};
}

Color blendColorsUsingOverlay(Color color1, Color color2, uint16_t blendFactor)
{
  uint16_t color1Red = color1.red;
  uint16_t color2Red = color2.red;
  if (blendFactor > (65535 >> 1)) color1Red = ((65535 - ((blendFactor - 32768) * 2)) * color1.red) >> 16;
  else color2Red = (blendFactor * 2 * color2.red) >> 16;

  uint16_t color1Green = color1.green;
  uint16_t color2Green = color2.green;
  if (blendFactor > (65535 >> 1)) color1Green = ((65535 - ((blendFactor - 32768) * 2)) * color1.green) >> 16;
  else color2Green = (blendFactor * 2 * color2.green) >> 16;

  uint16_t color1Blue = color1.blue;
  uint16_t color2Blue = color2.blue;
  if (blendFactor > (65535 >> 1)) color1Blue = ((65535 - ((blendFactor - 32768) * 2)) * color1.blue) >> 16;
  else color2Blue = (blendFactor * 2 * color2.blue) >> 16;

  uint16_t r;
  if (color1Red < 128) {
      r = (2 * color1Red * color2Red) / 255;
  } else {
      r = 255 - ((2 * (255 - color1Red) * (255 - color2Red)) / 255);
  }
  uint16_t g;
  if (color1Green < 128) {
      g = (2 * color1Green * color2Green) / 255;
  } else {
      g = 255 - ((2 * (255 - color1Green) * (255 - color2Green)) / 255);
  }
  uint16_t b;
  if (color1Blue < 128) {
      b = (2 * color1Blue * color2Blue) / 255;
  } else {
      b = 255 - ((2 * (255 - color1Blue) * (255 - color2Blue)) / 255);
  }
  return {r, g, b};
}

Color blendColorsUsingScreen(Color color1, Color color2, uint16_t blendFactor)
{
  uint16_t color1Red = color1.red;
  uint16_t color2Red = color2.red;
  if (blendFactor > 65535 / 2) color1Red = ((65535 - ((blendFactor - 32768) * 2)) * color1.red) >> 16;
  else color2Red = (blendFactor * 2 * color2.red) >> 16;

  uint16_t color1Green = color1.green;
  uint16_t color2Green = color2.green;
  if (blendFactor > 65535 / 2) color1Green = ((65535 - ((blendFactor - 32768) * 2)) * color1.green) >> 16;
  else color2Green = (blendFactor * 2 * color2.green) >> 16;

  uint16_t color1Blue = color1.blue;
  uint16_t color2Blue = color2.blue;
  if (blendFactor > 65535 / 2) color1Blue = ((65535 - ((blendFactor - 32768) * 2)) * color1.blue) >> 16;
  else color2Blue = (blendFactor * 2 * color2.blue) >> 16;

  uint16_t r = (65535 - (65535 - color1Red) * (65535 - color2Red)) >> 16;
  uint16_t g = (65535 - (65535 - color1Green) * (65535 - color2Green)) >> 16;
  uint16_t b =  (65535 - (65535 - color1Blue) * (65535 - color2Blue)) >> 16;
  return {r, g, b};
}

Color blendColorsUsingAverage(Color color1, Color color2, uint16_t blendFactor)
{
  uint16_t color1Red = color1.red;
  uint16_t color2Red = color2.red;
  if (blendFactor > 65535 / 2) color1Red = ((65535 - ((blendFactor - 32768) * 2)) * color1.red) >> 16;
  else color2Red = (blendFactor * 2 * color2.red) >> 16;

  uint16_t color1Green = color1.green;
  uint16_t color2Green = color2.green;
  if (blendFactor > 65535 / 2) color1Green = ((65535 - ((blendFactor - 32768) * 2)) * color1.green) >> 16;
  else color2Green = (blendFactor * 2 * color2.green) >> 16;

  uint16_t color1Blue = color1.blue;
  uint16_t color2Blue = color2.blue;
  if (blendFactor > 65535 / 2) color1Blue = ((65535 - ((blendFactor - 32768) * 2)) * color1.blue) >> 16;
  else color2Blue = (blendFactor * 2 * color2.blue) >> 16;

  uint16_t r = (color1Red + color2Red) / 2;
  uint16_t g = (color1Green + color2Green) / 2;
  uint16_t b =  (color1Blue + color2Blue) / 2;
  return {r, g, b};
}

Color blendColorsUsingShimmer(Color color1, Color color2, uint16_t blendFactor)
{
  uint16_t index = blendFactor >> 4;
  if (shimmerMap[index]) return color2;
  return color1;
}

inline Color fadeToBlack(Color color, float fadeValue) {
    uint16_t r, g, b;
    r = (color.red <= 10) ? 0 : color.red - (color.red * fadeValue);
    g = (color.green <= 10) ? 0 : color.green - (color.green * fadeValue);
    b = (color.blue <= 10) ? 0 : color.blue - (color.blue * fadeValue);
    return {r,g,b};
}