#ifndef LIGHTNING_BUG_EFFECT_H
#define LIGHTNING_BUG_EFFECT_H

#include "../../commonHeaders.h"
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

void incrementLightningBugEffect(Effect *effect);
Color lightningBugEffect(int pixelIndex, Effect *effect);

#endif