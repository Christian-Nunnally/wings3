#ifndef EFFECT_CONTROLLER_H
#define EFFECT_CONTROLLER_H

#include "../Graphics/color.h"

void incrementEffectFrame();
Color getLedColorForFrame(int ledIndex);
void setupEffects();
void randomizeEffectsNaturally();

#endif