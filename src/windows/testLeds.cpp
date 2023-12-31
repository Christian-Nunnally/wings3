#include "../windows/testLeds.h"

uint8_t testLedGlobalBrightnessValue = 255;

void setTestLedsPixelColor(int pixelIndex, uint8_t red, uint8_t green, uint8_t blue)
{
    std::cout << STANDARD_OUTPUT_OPERATION_CODE_SET_LED;
    std::cout << ",";
    std::cout << pixelIndex;
    std::cout << ",";
    uint8_t brightnessAdjustedRed = (red * testLedGlobalBrightnessValue) >> 8;
    uint8_t brightnessAdjustedGreen = (green * testLedGlobalBrightnessValue) >> 8;
    uint8_t brightnessAdjustedBlue = (blue * testLedGlobalBrightnessValue) >> 8;
    std::cout << (uint32_t)((brightnessAdjustedRed << 16) | (brightnessAdjustedGreen << 8) | (brightnessAdjustedBlue << 0));
    std::cout << "\n";
}

void setTestLedsBrightness(uint8_t brightness)
{
    testLedGlobalBrightnessValue = brightness;
}

void showTestLeds()
{
    std::cout << STANDARD_OUTPUT_OPERATION_CODE_SHOW_LEDS;
    std::cout << "\n";
}

void clearTestLeds()
{
    std::cout << STANDARD_OUTPUT_OPERATION_CODE_CLEAR_LEDS;
    std::cout << "\n";  
}