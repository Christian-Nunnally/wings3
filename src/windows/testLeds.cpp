#include "../common/settings.h"
#include "../common/utility/time.h"
#include "../windows/testLeds.h"

unsigned long lastFrameTime = 0;
uint8_t testLedGlobalBrightnessValue = 255;
uint32_t testLedBuffer[TOTAL_LEDS];
void setTestLedsPixelColor(int pixelIndex, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t brightnessAdjustedRed = (red * testLedGlobalBrightnessValue) >> 8;
    uint8_t brightnessAdjustedGreen = (green * testLedGlobalBrightnessValue) >> 8;
    uint8_t brightnessAdjustedBlue = (blue * testLedGlobalBrightnessValue) >> 8;
    testLedBuffer[pixelIndex] = (brightnessAdjustedRed << 16) | (brightnessAdjustedGreen << 8) | (brightnessAdjustedBlue << 0);
}

void setTestLedsBrightness(uint8_t brightness)
{
    testLedGlobalBrightnessValue = brightness;
}

void showTestLeds()
{
    // if (getTime() - lastFrameTime > 16)
    // {
        lastFrameTime = getTime();
        std::cout << STANDARD_OUTPUT_OPERATION_CODE_SHOW_LEDS;
        std::cout << ",";
        for (int i = 0; i < TOTAL_LEDS; i++)
        {
            std::cout << std::hex << testLedBuffer[i] << std::dec;
            if (i != TOTAL_LEDS - 1)
            {
                std::cout << ",";
            }
        }
        std::cout << "\n";
    // }
}

void clearTestLeds()
{
    std::cout << STANDARD_OUTPUT_OPERATION_CODE_CLEAR_LEDS;
    std::cout << "\n";  
}