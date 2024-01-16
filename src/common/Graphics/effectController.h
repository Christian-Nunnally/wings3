#ifndef EFFECT_CONTROLLER_H
#define EFFECT_CONTROLLER_H

#include "../Graphics/color.h"

void incrementEffectFrame();
void setupEffects();
void randomizeEffectsNaturally();
extern Color (*getLedColorForFrame)(int pixel);

#endif