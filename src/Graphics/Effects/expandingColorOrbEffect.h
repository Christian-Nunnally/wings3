#ifndef EXPANDING_COLOR_ORB_EFFECT_H
#define EXPANDING_COLOR_ORB_EFFECT_H

#include <Arduino.h>
#include "../../Graphics/color.h"

Color expandingColorOrbEffect(int frameDelta, int pixelIndex, int paletteOffset, uint16_t globalBrightnessModifier);

#endif