#ifndef STAR_FIELD_EFFECT_H
#define STAR_FIELD_EFFECT_H

#include "../../commonHeaders.h"
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

void incrementStarFieldEffect(Effect *effect, int fadeAmount, int16_t newStarLikelihood);
Color starFieldEffect(int pixelIndex, Effect *effect, int fadeAmount);

#endif