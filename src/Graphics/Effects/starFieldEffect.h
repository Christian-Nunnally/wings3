#ifndef STAR_FIELD_EFFECT_H
#define STAR_FIELD_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"

Color starFieldEffect(int frameDelta, int fadeAmount, uint16_t newStarLikelihood, int pixelIndex, int paletteOffset, uint16_t globalBrightnessModifier);

#endif