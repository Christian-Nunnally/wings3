#ifndef EFFECTS_H
#define EFFECTS_H

#include "../Graphics/effect.h"
#include "../Graphics/effectSettings.h"

#define METEOR_RAIN_EFFECT_INDEX 0
#define METEOR_RAIN_EFFECT_2_INDEX 1
#define STAR_FIELD_EFFECT_INDEX 2
#define GRADIENT_WAVE_EFFECT_INDEX 3
#define RAIN_SHOWER_EFFECT_INDEX 4
#define EXPANDING_COLOR_ORB_EFFECT_INDEX 5
#define LIGHTNING_BUG_EFFECT_INDEX 6
#define FIRE_EFFECT_INDEX 7
#define SIMPLE_SOLID_COLOR_FILL_EFFECT_INDEX 8
#define FIREWORKS_EFFECT_INDEX 9
#define LIGHT_CHASE_EFFECT_INDEX 10

void setAudioIntensityLevel(float level);
void pickRandomEffects();
void syncEffects(Effect *syncFrom, Effect *syncTo);
void incrementEffects();

extern Effect effectA1;
extern Effect effectB1;
extern Effect effectA2;
extern Effect effectB2;

extern EffectSettings effectSettings;

extern float currentAudioIntensityLevel;

#endif