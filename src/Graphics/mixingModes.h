#ifndef MIXING_MODES_H
#define MIXING_MODES_H

#include <Arduino.h>
#include "../Graphics/color.h"

void setupMixingModes();
Color blendIncorporatingOldMixingMode(Color color1, Color color2, byte percentOfEffectBToShow);
void incrementMixingModeBlend(int frameTimeDelta);
void pickRandomMixingMode(int minimumMillisecondsForTransition, int maximumMillisecondsForTransition);

#endif