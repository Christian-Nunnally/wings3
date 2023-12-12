#ifndef EXPANDING_COLOR_ORB_EFFECT_H
#define EXPANDING_COLOR_ORB_EFFECT_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

void incrementExpandingColorOrbEffect(Effect *effect);
Color expandingColorOrbEffect(int pixelIndex, Effect *effect);

#endif