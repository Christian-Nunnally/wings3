#include <Adafruit_NeoPXL8.h>
#include "microphone.h"
#include "settings.h"
#include "graphics.h"
#include <pgmspace.h>

//#include "palettes.h"
#include "palettesMinimal.h"

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

Color wavePulse(uint32_t brightnessFrame, uint32_t colorFrame, int pixelIndex, uint8_t brightnessProjectionMap[], uint8_t colorProjectionMap[], int paletteOffset, uint16_t globalBrightnessModifier) 
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

Color starShimmer(int fadeAmount, int newStarLikelihood, int pixelIndex, int paletteOffset, uint16_t globalBrightnessModifier) 
{
  if (pixelIndex == 0)
  {
    if (random(10000) < newStarLikelihood)
    {
      int chosenLed = random(TOTAL_LEDS);
      starShimmerCurrentTargetValueMap[chosenLed] = random(globalBrightnessModifier);
      starShimmerSpeedMap[chosenLed] = 2 * random(fadeAmount) + 1;
      starShimmerColorMap[chosenLed] = random(PALETTE_LENGTH) + paletteOffset;
      starShimmerTwinkleMap[chosenLed] = 0;
      starShimmerTwinklePersistanceMap[chosenLed] = 0;
      starShimmerTwinkleDirectionMap[chosenLed] = 0;
    }
  }

  if (starShimmerCurrentTargetValueMap[pixelIndex] > 0)
  {
    if(starShimmerMap[pixelIndex] < starShimmerCurrentTargetValueMap[pixelIndex])
    {
      if (starShimmerMap[pixelIndex] + starShimmerSpeedMap[pixelIndex] < starShimmerCurrentTargetValueMap[pixelIndex])
      {
        starShimmerMap[pixelIndex] += starShimmerSpeedMap[pixelIndex];
      }
      else 
      {
        starShimmerMap[pixelIndex] = starShimmerCurrentTargetValueMap[pixelIndex];
        starShimmerCurrentTargetValueMap[pixelIndex] = 1;
        starShimmerSpeedMap[pixelIndex] = random(fadeAmount) + 1;
        starShimmerTwinkleMap[pixelIndex] = random(2000);
        starShimmerTwinklePersistanceMap[pixelIndex] = random(800);
      }
    }
    else 
    {
      if (starShimmerMap[pixelIndex] > starShimmerSpeedMap[pixelIndex])
      {
        starShimmerMap[pixelIndex] -= starShimmerSpeedMap[pixelIndex];
      }
      else 
      {
        starShimmerMap[pixelIndex] = 0;
        starShimmerCurrentTargetValueMap[pixelIndex] = 0;
      }
    }
  }
  if (starShimmerMap[pixelIndex] == 0) return {0,0,0};

  int shimmerAmount = random(starShimmerTwinklePersistanceMap[pixelIndex]) - (starShimmerTwinklePersistanceMap[pixelIndex] / 2);
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

Color meteorRain(uint32_t animationFrame, uint32_t colorFrame, int pixelIndex, byte meteorSize, float meteorTrailDecay, uint8_t projectionMap[], int paletteOffset, uint16_t globalBrightnessModifier)
{
  uint32_t animationFrame9bit = animationFrame & 0x1FF;
  if (random(100) > 90)
  {
    currentLedColorMap[pixelIndex] = fadeToBlack(currentLedColorMap[pixelIndex], meteorTrailDecay );        
  }
  if (projectionMap[pixelIndex] + meteorSize > animationFrame9bit && projectionMap[pixelIndex] + meteorSize < animationFrame9bit + meteorSize)
  {
    uint8_t colorFrame8bit = (colorFrame / 15);
    uint8_t colorStartingPoint = projectionMap[pixelIndex];
    uint8_t colorFramePreOffset = colorStartingPoint + colorFrame8bit;
    Color result = colorFromPalette(paletteOffset + colorFramePreOffset, globalBrightnessModifier);
    currentLedColorMap[pixelIndex] = result;
  }
  return currentLedColorMap[pixelIndex];
}

Color meteorRain2(uint32_t projectionMap1Frame, uint32_t projectionMap2Frame, uint32_t colorFrame, int pixelIndex, byte meteorSize, float meteorTrailDecay, uint8_t projectionMap[], uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier)
{
  uint32_t projectionMap1FrameWrapping = projectionMap1Frame % (256 + meteorSize * 2);
  uint32_t projectionMap2FrameWrapping = projectionMap2Frame % (256 + meteorSize * 2);
  if (random(100) > 98)
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

  uint16_t red = sqrt((brightnessAdjustedRed1 * color1.red) + (brightnessAdjustedRed2 * color2.red));
  uint16_t green = sqrt((brightnessAdjustedGreen1 * color1.green) + (brightnessAdjustedGreen2 * color2.green));
  uint16_t blue = sqrt((brightnessAdjustedBlue1 * color1.blue) + (brightnessAdjustedBlue2 * color2.blue));
  return {red, green, blue};
}

Color blendColorsUsingMultiply(Color color1, Color color2, uint16_t blendFactor)
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

  uint16_t r = (color1Red * color2Red) >> 16;
  uint16_t g =(color1Green * color2Green) >> 16;
  uint16_t b = (color1Blue * color2Blue) >> 16;
  return {r, g, b};
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