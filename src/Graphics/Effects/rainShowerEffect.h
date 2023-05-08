#ifndef RAIN_SHOWER_EFFECT_H
#define RAIN_SHOWER_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"

Color rainShowerEffect(int frameDelta, uint32_t rainAnimationFrame, int pixelIndex, byte rainLength, const uint8_t projectionMap[], const uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier);

#endif