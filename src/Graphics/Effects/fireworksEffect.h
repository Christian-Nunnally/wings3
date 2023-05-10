#ifndef FIREWORKS_EFFECT_H
#define FIREWORKS_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

Color fireworksEffect(int pixelIndex, Effect *effect, int frameDelta, int fireworkProbability);

#endif