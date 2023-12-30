
#ifndef SCREEN_MAPS_H
#define SCREEN_MAPS_H

#include "../commonHeaders.h"
#include "../settings.h"

uint8_t infinityScreenMap[TOTAL_LEDS] = {0,0,0,0};
uint8_t logoScreenMap[TOTAL_LEDS] = {0,0,0,0};
uint8_t normalScreenMap[TOTAL_LEDS] = {0,0,0,0};
uint8_t outlineScreenMap[TOTAL_LEDS] = {1,0,0,0};
uint8_t outlineThickScreenMap[TOTAL_LEDS] = {1,1,1,1};

const int screenMapsCount = 5;
uint8_t *screenMaps[screenMapsCount] = {infinityScreenMap, logoScreenMap, normalScreenMap, outlineScreenMap, outlineThickScreenMap };

#endif
