#ifndef FIRE_EFFECT_H
#define FIRE_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

Color fireEffect(int pixelIndex, Effect *effect, int frameDelta, int flameDecay, int sparks);

#endif