#ifndef FIRE_EFFECT_H
#define FIRE_EFFECT_H

#include "../../commonHeaders.h"
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

void incrementFireEffect(Effect *effect, int flameDecay);
Color fireEffect(int pixelIndex, Effect *effect, int sparks);

#endif