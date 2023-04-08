#include <Adafruit_NeoPXL8.h>
#include "microphone.h"
#include "graphics.h"
#include <pgmspace.h>

inline uint8_t sqrt16(uint16_t x);
inline uint16_t squareRoot32Bit(int input);

Color colorFromPalette(uint16_t palette[768], uint8_t index, uint16_t brightness)
{
  uint16_t red = pgm_read_word(palette + index * 3 + 0);
  uint16_t green = pgm_read_word(palette + index * 3 + 1);
  uint16_t blue = pgm_read_word(palette + index * 3 + 2);
  return { 
    (uint16_t)((red * brightness) >> 16), 
    (uint16_t)((green * brightness) >> 16),
    (uint16_t)((blue * brightness) >> 16)
  };
}

Color wavePulse(uint32_t brightnessFrame, uint32_t colorFrame, int pixelNum, uint8_t brightnessProjectionMap[], uint8_t colorProjectionMap[], uint16_t color_palette[], uint16_t globalBrightnessModifier) 
{
    uint8_t brightnessFrame8bit = (brightnessFrame / 4);
    uint8_t colorFrame8bit = (colorFrame / 15);
    uint8_t brightnessStartingPoint = brightnessProjectionMap[pixelNum];
    uint8_t colorStartingPoint = colorProjectionMap[pixelNum];
    uint16_t brightness = ((uint8_t)(brightnessStartingPoint - brightnessFrame8bit) << 8);
    brightness = (brightness * globalBrightnessModifier) >> 16;
    return colorFromPalette(color_palette, colorStartingPoint + colorFrame8bit, brightness);
}

Color blendColorsUsingMixingFast(Color color1, Color color2, uint16_t blendFactor)
{
  uint16_t red1Squared = (color1.red * color1.red) >> 16;
  uint16_t red1Calc = ((65535 - blendFactor) * red1Squared) >> 16;
  uint16_t red2Squared = (color2.red * color2.red) >> 16;
  uint16_t red2Calc = (blendFactor * red1Squared) >> 16;

  uint16_t green1Squared = (color1.green * color1.green) >> 16;
  uint16_t green1Calc = ((65535 - blendFactor) * green1Squared) >> 16;
  uint16_t green2Squared = (color2.green * color2.green) >> 16;
  uint16_t green2Calc = (blendFactor * green1Squared) >> 16;

  uint16_t blue1Squared = (color1.blue * color1.blue) >> 16;
  uint16_t blue1Calc = ((65535 - blendFactor) * blue1Squared) >> 16;
  uint16_t blue2Squared = (color2.blue * color2.blue) >> 16;
  uint16_t blue2Calc = (blendFactor * blue1Squared) >> 16;

  uint16_t red = sqrt16(red1Calc + red2Calc) << 8;
  uint16_t green = sqrt16(green1Calc + green2Calc) << 8;
  uint16_t blue = sqrt16(blue1Calc + blue2Calc) << 8;
  return {red, green, blue};
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

inline uint8_t sqrt16(uint16_t x)
{
    if( x <= 1) {
        return x;
    }

    uint8_t low = 1; // lower bound
    uint8_t hi, mid;

    if( x > 7904) {
        hi = 255;
    } else {
        hi = (x >> 5) + 8; // initial estimate for upper bound
    }

    do {
        mid = (low + hi) >> 1;
        if ((uint16_t)(mid * mid) > x) {
            hi = mid - 1;
        } else {
            if( mid == 255) {
                return 255;
            }
            low = mid + 1;
        }
    } while (hi >= low);

    return low - 1;
}