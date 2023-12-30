#ifndef PER_PIXEL_ANGLE_TRANSFORM_MAPS_H
#define PER_PIXEL_ANGLE_TRANSFORM_MAPS_H

#include "../commonHeaders.h"
#include "../settings.h"


const static uint8_t angleTransformMapForPixel0[4] PROGMEM = {0, 255, 0, 18};
const static uint8_t angleTransformMapForPixel1[4] PROGMEM = {127, 0, 108, 0};
const static uint8_t angleTransformMapForPixel2[4] PROGMEM = {0, 236, 0, 255};
const static uint8_t angleTransformMapForPixel3[4] PROGMEM = {146, 0, 127, 0};

const static int pixelAngleTransformMapsCount = 4;
const static uint8_t* pixelAngleTransformMaps[pixelAngleTransformMapsCount] = {angleTransformMapForPixel0, angleTransformMapForPixel1, angleTransformMapForPixel2, angleTransformMapForPixel3 };

#endif
