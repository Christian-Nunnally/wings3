#ifndef EXPANDING_COLOR_ORB_EFFECT_H
#define EXPANDING_COLOR_ORB_EFFECT_H

#include "../../commonHeaders.h"
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

void incrementExpandingColorOrbEffect(Effect *effect);
Color expandingColorOrbEffect(int pixelIndex, Effect *effect);

#endif