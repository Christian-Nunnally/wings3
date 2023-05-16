#ifndef EFFECTS_H
#define EFFECTS_H

#include "../Graphics/effect.h"
#include "../Graphics/effectSettings.h"

void setAudioIntensityLevel(float level);
void pickRandomEffects();

extern Effect effect1;
extern Effect effect2;
extern Effect effect3;
extern Effect effect4;

extern EffectSettings effectSettings;

extern float currentAudioIntensityLevel;

#endif