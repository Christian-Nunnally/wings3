#ifndef FIREWORKS_EFFECT_H
#define FIREWORKS_EFFECT_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

void incrementFireworksEffect(Effect *effect, int fireworkProbability);
Color fireworksEffect(int pixelIndex, Effect *effect);
void triggerFirework();

#endif