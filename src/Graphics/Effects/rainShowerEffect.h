#ifndef RAIN_SHOWER_EFFECT_H
#define RAIN_SHOWER_EFFECT_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

Color rainShowerEffect(int pixelIndex, Effect *effect);

#endif