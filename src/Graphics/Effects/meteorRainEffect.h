#ifndef METEOR_RAIN_EFFECT_H
#define METEOR_RAIN_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

Color meteorRainEffect(int pixelIndex, Effect *effect, float meteorTrailDecay);
Color meteorRainEffect2(int pixelIndex, Effect *effect, float meteorTrailDecay);

#endif