#ifndef LEDS_H
#define LEDS_H

#include "../commonHeaders.h"

void setupLeds();
void refreshLeds();
void renderLeds();
void setGlobalLedBrightness(uint8_t brightness);
uint8_t getGlobalLedBrightness();
void disableLeds();
void enableLeds();
void clearLeds();

#endif