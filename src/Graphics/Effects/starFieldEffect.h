#ifndef STAR_FIELD_EFFECT_H
#define STAR_FIELD_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

Color starFieldEffect(int pixelIndex, Effect *effect, int frameDelta, int fadeAmount, uint16_t newStarLikelihood);

#endif