#ifndef SOLID_COLOR_FILL_EFFECT_H
#define SOLID_COLOR_FILL_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"

Color simpleSolidColorFillEffect(int colorFrame, int paletteOffset, uint16_t globalBrightnessModifier);

#endif