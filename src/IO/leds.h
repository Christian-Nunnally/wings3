#ifndef LEDS_H
#define LEDS_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif

void setupLeds();
void refreshLeds();
void renderLeds();
void setGlobalLedBrightness(uint8_t brightness);
void disableLeds();
void enableLeds();
void clearLeds();

#endif