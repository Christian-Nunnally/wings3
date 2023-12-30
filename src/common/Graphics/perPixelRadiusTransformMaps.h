#ifndef PER_PIXEL_RADIUS_TRANSFORM_MAPS_H
#define PER_PIXEL_RADIUS_TRANSFORM_MAPS_H

#include "../commonHeaders.h"
#include "../settings.h"


const static uint8_t radiusTransformMapForPixel0[4] PROGMEM = {0, 228, 114, 255};
const static uint8_t radiusTransformMapForPixel1[4] PROGMEM = {228, 0, 255, 113};
const static uint8_t radiusTransformMapForPixel2[4] PROGMEM = {114, 255, 0, 228};
const static uint8_t radiusTransformMapForPixel3[4] PROGMEM = {255, 113, 228, 0};

const static int pixelRadiusTransformMapsCount = 4;
const static uint8_t* pixelRadiusTransformMaps[pixelRadiusTransformMapsCount] = {radiusTransformMapForPixel0, radiusTransformMapForPixel1, radiusTransformMapForPixel2, radiusTransformMapForPixel3 };

#endif
