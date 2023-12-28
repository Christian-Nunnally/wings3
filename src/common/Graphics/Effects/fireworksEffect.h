#ifndef FIREWORKS_EFFECT_H
#define FIREWORKS_EFFECT_H

#include "../../commonHeaders.h"
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

void incrementFireworksEffect(Effect *effect, int fireworkProbability);
Color fireworksEffect(int pixelIndex, Effect *effect);
void triggerFirework();

#endif