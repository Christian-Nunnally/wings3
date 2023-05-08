#ifndef GRADIENT_WAVE_EFFECT_H
#define GRADIENT_WAVE_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"

Color gradientWaveEffect(uint32_t brightnessFrame, uint32_t colorFrame, int pixelIndex, const uint8_t brightnessProjectionMap[], const uint8_t colorProjectionMap[], int paletteOffset, uint16_t globalBrightnessModifier);

#endif