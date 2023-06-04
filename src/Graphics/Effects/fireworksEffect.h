#ifndef FIREWORKS_EFFECT_H
#define FIREWORKS_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

void incrementFireworksEffect(Effect *effect, int fireworkProbability);
Color fireworksEffect(int pixelIndex, Effect *effect);
void triggerFirework();

#endif