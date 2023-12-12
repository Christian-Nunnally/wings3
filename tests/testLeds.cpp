#include <iostream>
#include "../tests/testLeds.h"

void setTestLedsPixelColor(int pixelIndex, uint8_t red, uint8_t green, uint8_t blue)
{
    std::cout << "setPixel";
    std::cout << ",";
    std::cout << pixelIndex;
    std::cout << ",";
    std::cout << (int) red;
    std::cout << ",";
    std::cout << (int) green;
    std::cout << ",";
    std::cout << (int) blue;
    std::cout << "\n";
}

void setTestLedsBrightness(uint8_t brightness)
{
}

void showTestLeds()
{
    std::cout << "showFrame";
    std::cout << "\n";
}