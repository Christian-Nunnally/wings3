#ifndef LIGHTNING_BUG_EFFECT_H
#define LIGHTNING_BUG_EFFECT_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "../../Graphics/color.h"
#include "../../Graphics/effect.h"

void incrementLightningBugEffect(Effect *effect);
Color lightningBugEffect(int pixelIndex, Effect *effect);

#endif