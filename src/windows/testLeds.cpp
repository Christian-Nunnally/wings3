#include "../windows/testLeds.h"

uint8_t testLedGlobalBrightnessValue = 255;

void setTestLedsPixelColor(int pixelIndex, uint8_t red, uint8_t green, uint8_t blue)
{
    std::cout << STANDARD_OUTPUT_OPERATION_CODE_SET_LED;
    std::cout << ",";
    std::cout << pixelIndex;
    std::cout << ",";
    std::cout << (int) ((red * testLedGlobalBrightnessValue) >> 8);
    std::cout << ",";
    std::cout << (int) ((green * testLedGlobalBrightnessValue) >> 8);
    std::cout << ",";
    std::cout << (int) ((blue * testLedGlobalBrightnessValue) >> 8);
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