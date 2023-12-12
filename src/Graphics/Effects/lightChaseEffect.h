#ifndef LIGHT_CHASE_EFFECT_H
#define LIGHT_CHASE_EFFECT_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

Color lightChaseEffect(int pixelIndex, Effect *effect, int mod);

#endif