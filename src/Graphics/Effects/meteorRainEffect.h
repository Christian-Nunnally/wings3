#ifndef METEOR_RAIN_EFFECT_H
#define METEOR_RAIN_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"

Color meteorRainEffect(int frameDelta, uint32_t animationFrame, uint32_t colorFrame, int pixelIndex, byte meteorSize, float meteorTrailDecay, const uint8_t projectionMap[], int paletteOffset, uint16_t globalBrightnessModifier);
Color meteorRainEffect2(int frameDelta, uint32_t projectionMap1Frame, uint32_t projectionMap2Frame, uint32_t colorFrame, int pixelIndex, byte meteorSize, float meteorTrailDecay, const uint8_t projectionMap[], const uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier);

#endif