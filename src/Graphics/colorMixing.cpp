#include "../Graphics/colorMixing.h"
#include "../Utility/fastMath.h"
#include <Arduino.h>

Color blendColorsUsingMixing(Color color1, Color color2, uint8_t blendFactor)
{
  uint8_t inverseBlendFactor = UINT8_MAX - blendFactor;
  uint16_t redResult = ((inverseBlendFactor * color1.red * color1.red) + (blendFactor * color2.red * color2.red)) >> 10;
  uint16_t greenResult = ((inverseBlendFactor * color1.green * color1.green) + (blendFactor * color2.green * color2.green)) >> 10;
  uint16_t blueResult = ((inverseBlendFactor * color1.blue * color1.blue) + (blendFactor * color2.blue * color2.blue)) >> 10;
  uint8_t red = fastSquareRootOnlyProvide14MsbOf16BitInput(redResult);
  uint8_t green = fastSquareRootOnlyProvide14MsbOf16BitInput(greenResult);
  uint8_t blue = fastSquareRootOnlyProvide14MsbOf16BitInput(blueResult);
  return {red, green, blue};
}

Color blendColorsUsingMixingGlitched(Color color1, Color color2, uint8_t blendFactor)
{
  uint8_t brightnessAdjustedRed1 = ((UINT8_MAX - blendFactor) * color1.red) >> 8;
  uint8_t color1FactoredRed = (brightnessAdjustedRed1 * color1.red) >> 8;
  uint8_t brightnessAdjustedRed2 = (blendFactor * color2.red) >> 8;
  uint8_t color2FactoredRed = (brightnessAdjustedRed2 * color2.red) >> 8;

  uint8_t brightnessAdjustedGreen1 = ((UINT8_MAX - blendFactor) * color1.green) >> 8;
  uint8_t color1FactoredGreen = (brightnessAdjustedGreen1 * color1.green) >> 8;
  uint8_t brightnessAdjustedGreen2 = (blendFactor * color2.green) >> 8;
  uint8_t color2FactoredGreen = (brightnessAdjustedGreen2 * color2.green) >> 8;

  uint8_t brightnessAdjustedBlue1 = ((UINT8_MAX - blendFactor) * color1.blue) >> 8;
  uint8_t color1FactoredBlue = (brightnessAdjustedBlue1 * color1.blue) >> 8;
  uint8_t brightnessAdjustedBlue2 = (blendFactor * color2.blue) >> 8;
  uint8_t color2FactoredBlue = (brightnessAdjustedBlue2 * color2.blue) >> 8;

  uint8_t red = fastSquareRoot16BitInput((brightnessAdjustedRed1 * color1.red) + (brightnessAdjustedRed2 * color2.red));
  uint8_t green = fastSquareRoot16BitInput((brightnessAdjustedGreen1 * color1.green) + (brightnessAdjustedGreen2 * color2.green));
  uint8_t blue = fastSquareRoot16BitInput((brightnessAdjustedBlue1 * color1.blue) + (brightnessAdjustedBlue2 * color2.blue));

  if (red > 20000) green = 0;
  return {red, green, blue};
}

Color blendColorsUsingAdd(Color color1, Color color2, uint8_t blendFactor)
{
  const uint8_t half8BitMax = UINT8_MAX / 2;
  uint8_t color1Red = color1.red;
  uint8_t color2Red = color2.red;
  if (blendFactor > half8BitMax) color1Red = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.red) >> 8;
  else color2Red = (blendFactor * 2 * color2.red) >> 8;

  uint8_t color1Green = color1.green;
  uint8_t color2Green = color2.green;
  if (blendFactor > half8BitMax) color1Green = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.green) >> 8;
  else color2Green = (blendFactor * 2 * color2.green) >> 8;

  uint8_t color1Blue = color1.blue;
  uint8_t color2Blue = color2.blue;
  if (blendFactor > half8BitMax) color1Blue = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.blue) >> 8;
  else color2Blue = (blendFactor * 2 * color2.blue) >> 8;

  uint8_t r = (color1Red + color2Red) & 0xFF;
  uint8_t g =(color1Green + color2Green) & 0xFF;
  uint8_t b = (color1Blue + color2Blue) & 0xFF;
  return {r, g, b};
}

Color blendColorsUsingSubtract(Color color1, Color color2, uint8_t blendFactor)
{
  const uint8_t half8BitMax = UINT8_MAX / 2;
  uint8_t color1Red = color1.red;
  uint8_t color2Red = color2.red;
  if (blendFactor > half8BitMax) color1Red = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.red) >> 8;
  else color2Red = (blendFactor * 2 * color2.red) >> 8;

  uint8_t color1Green = color1.green;
  uint8_t color2Green = color2.green;
  if (blendFactor > half8BitMax) color1Green = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.green) >> 8;
  else color2Green = (blendFactor * 2 * color2.green) >> 8;

  uint8_t color1Blue = color1.blue;
  uint8_t color2Blue = color2.blue;
  if (blendFactor > half8BitMax) color1Blue = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.blue) >> 8;
  else color2Blue = (blendFactor * 2 * color2.blue) >> 8;

  uint8_t r = (UINT8_MAX - color1Red - color2Red) & 0xFF;
  uint8_t g =(UINT8_MAX - color1Green - color2Green) & 0xFF;
  uint8_t b = (UINT8_MAX - color1Blue - color2Blue) & 0xFF;
  return {r, g, b};
}

Color blendColorsUsingOverlay(Color color1, Color color2, uint8_t blendFactor)
{
  const uint8_t half8BitMax = UINT8_MAX / 2;
  uint8_t color1Red = color1.red;
  uint8_t color2Red = color2.red;
  if (blendFactor > half8BitMax) color1Red = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.red) >> 8;
  else color2Red = (blendFactor * 2 * color2.red) >> 8;

  uint8_t color1Green = color1.green;
  uint8_t color2Green = color2.green;
  if (blendFactor > half8BitMax) color1Green = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.green) >> 8;
  else color2Green = (blendFactor * 2 * color2.green) >> 8;

  uint8_t color1Blue = color1.blue;
  uint8_t color2Blue = color2.blue;
  if (blendFactor > half8BitMax) color1Blue = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.blue) >> 8;
  else color2Blue = (blendFactor * 2 * color2.blue) >> 8;

  uint8_t r;
  if (color1Red < 128) {
      r = (2 * color1Red * color2Red) / 255;
  } else {
      r = 255 - ((2 * (255 - color1Red) * (255 - color2Red)) / 255);
  }
  uint8_t g;
  if (color1Green < 128) {
      g = (2 * color1Green * color2Green) / 255;
  } else {
      g = 255 - ((2 * (255 - color1Green) * (255 - color2Green)) / 255);
  }
  uint8_t b;
  if (color1Blue < 128) {
      b = (2 * color1Blue * color2Blue) / 255;
  } else {
      b = 255 - ((2 * (255 - color1Blue) * (255 - color2Blue)) / 255);
  }
  return {r, g, b};
}

Color blendColorsUsingScreen(Color color1, Color color2, uint8_t blendFactor)
{
  const uint8_t half8BitMax = UINT8_MAX / 2;
  uint8_t color1Red = color1.red;
  uint8_t color2Red = color2.red;
  if (blendFactor > half8BitMax) color1Red = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.red) >> 8;
  else color2Red = (blendFactor * 2 * color2.red) >> 8;

  uint8_t color1Green = color1.green;
  uint8_t color2Green = color2.green;
  if (blendFactor > half8BitMax) color1Green = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.green) >> 8;
  else color2Green = (blendFactor * 2 * color2.green) >> 8;

  uint8_t color1Blue = color1.blue;
  uint8_t color2Blue = color2.blue;
  if (blendFactor > half8BitMax) color1Blue = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.blue) >> 8;
  else color2Blue = (blendFactor * 2 * color2.blue) >> 8;

  uint8_t r = (UINT8_MAX - (UINT8_MAX - color1Red) * (UINT8_MAX - color2Red)) >> 8;
  uint8_t g = (UINT8_MAX - (UINT8_MAX - color1Green) * (UINT8_MAX - color2Green)) >> 8;
  uint8_t b =  (UINT8_MAX - (UINT8_MAX - color1Blue) * (UINT8_MAX - color2Blue)) >> 8;
  return {r, g, b};
}

Color blendColorsUsingAverage(Color color1, Color color2, uint8_t blendFactor)
{
  const uint8_t half8BitMax = UINT8_MAX / 2;
  uint8_t color1Red = color1.red;
  uint8_t color2Red = color2.red;
  if (blendFactor > half8BitMax) color1Red = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.red) >> 8;
  else color2Red = (blendFactor * 2 * color2.red) >> 8;

  uint8_t color1Green = color1.green;
  uint8_t color2Green = color2.green;
  if (blendFactor > half8BitMax) color1Green = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.green) >> 8;
  else color2Green = (blendFactor * 2 * color2.green) >> 8;

  uint8_t color1Blue = color1.blue;
  uint8_t color2Blue = color2.blue;
  if (blendFactor > half8BitMax) color1Blue = ((UINT8_MAX - ((blendFactor - half8BitMax) * 2)) * color1.blue) >> 8;
  else color2Blue = (blendFactor * 2 * color2.blue) >> 8;

  uint8_t r = (color1Red + color2Red) / 2;
  uint8_t g = (color1Green + color2Green) / 2;
  uint8_t b =  (color1Blue + color2Blue) / 2;
  return {r, g, b};
}

Color blendColorsUsingShimmer(Color color1, Color color2, uint8_t blendFactor)
{
  if (shimmerMap[blendFactor]) return color2;
  return color1;
}

Color fadeColorToBlack(Color color, float fadeValue) 
{
    uint8_t red = (color.red <= 10) ? 0 : color.red - (color.red * fadeValue);
    uint8_t green = (color.green <= 10) ? 0 : color.green - (color.green * fadeValue);
    uint8_t blue = (color.blue <= 10) ? 0 : color.blue - (color.blue * fadeValue);
    return {red, green, blue};
}