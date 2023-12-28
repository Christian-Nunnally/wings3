#ifndef TEST_LEDS_H
#define TEST_LEDS_H

#include <stdint.h>
#include <iostream>
#include "../tests/testLeds.h"
#include "../src/IO/standardOutputCommandOperationCodes.h"

void setTestLedsPixelColor(int pixelIndex, uint8_t red, uint8_t green, uint8_t blue);
void setTestLedsBrightness(uint8_t brightness);
void showTestLeds();
void clearTestLeds();

#endif