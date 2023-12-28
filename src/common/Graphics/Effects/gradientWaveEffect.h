#ifndef GRADIENT_WAVE_EFFECT_H
#define GRADIENT_WAVE_EFFECT_H

#include "../../commonHeaders.h"
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

Color gradientWaveEffect(int pixelIndex, Effect *effect);
Color gradientWaveEffectWithMaxGlobalBrightness(int pixelIndex, Effect *effect);

#endif