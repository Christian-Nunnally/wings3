#ifndef MIXING_MODES_H
#define MIXING_MODES_H

#include "../commonHeaders.h"
#include "../Graphics/color.h"

void setupMixingModes();
Color blendIncorporatingOldMixingMode(Color color1, Color color2, uint8_t percentOfEffectBToShow);
void incrementMixingModeBlend(int frameTimeDelta);
void pickRandomMixingMode(int minimumMillisecondsForTransition, int maximumMillisecondsForTransition);

#endif