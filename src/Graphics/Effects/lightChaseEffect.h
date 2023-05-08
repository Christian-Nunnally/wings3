#ifndef LIGHT_CHASE_EFFECT_H
#define LIGHT_CHASE_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"

Color lightChaseEffect(uint32_t ledTestAnimationFrame, int pixelIndex, uint32_t colorAnimationFrame, int paletteOffset, int mod, uint16_t globalBrightnessModifier);

#endif