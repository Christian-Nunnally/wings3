#ifndef SOLID_COLOR_FILL_EFFECT_H
#define SOLID_COLOR_FILL_EFFECT_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

Color simpleSolidColorFillEffect(int pixelIndex, Effect *effect);

#endif