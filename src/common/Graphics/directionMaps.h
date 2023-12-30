#ifndef DIRECTION_MAPS_H
#define DIRECTION_MAPS_H

#include "../commonHeaders.h"
#include "../settings.h"

const static uint16_t northernLedMap[TOTAL_LEDS] = {4, 4, 0, 1 };
const static uint16_t southernLedMap[TOTAL_LEDS] = {2, 3, 4, 4 };
const static int isBottomLed[TOTAL_LEDS] = {-1, -1, -1, -1 };

#endif
