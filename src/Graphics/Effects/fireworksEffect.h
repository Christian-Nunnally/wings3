#ifndef FIREWORKS_EFFECT_H
#define FIREWORKS_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"

Color fireworksEffect(int frameDelta, uint32_t colorFrame, int pixelIndex, int fireworkProbability, int paletteOffset, uint16_t globalBrightnessModifier);

#endif