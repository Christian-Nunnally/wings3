#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

void setupLeds();
void refreshLeds();
void renderLeds();
void setGlobalLedBrightness(uint8_t brightness);

#endif