#ifndef MIXING_MODES_H
#define MIXING_MODES_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "../Graphics/color.h"

void setupMixingModes();
Color blendIncorporatingOldMixingMode(Color color1, Color color2, uint8_t percentOfEffectBToShow);
void incrementMixingModeBlend(int frameTimeDelta);
void pickRandomMixingMode(int minimumMillisecondsForTransition, int maximumMillisecondsForTransition);

#endif