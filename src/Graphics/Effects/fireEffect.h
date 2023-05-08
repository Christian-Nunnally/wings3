#ifndef FIRE_EFFECT_H
#define FIRE_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"

Color fireEffect(int frameDelta, int pixelIndex, int flameDecay, int sparks, const uint8_t projectionMap[], const uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier);

#endif