#ifndef FIRE_EFFECT_H
#define FIRE_EFFECT_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

void incrementFireEffect(Effect *effect, int flameDecay);
Color fireEffect(int pixelIndex, Effect *effect, int sparks);

#endif