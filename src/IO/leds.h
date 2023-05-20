#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

bool setupLeds();
void refreshLeds();
void renderLeds();
void setGlobalLedBrightness(uint16_t brightness);

#endif