#ifndef BRIGHTNESS_CONTROL_MODES_H
#define BRIGHTNESS_CONTROL_MODES_H

#include "Arduino.h"
#include "../Graphics/effect.h"

void pickRandomGlobalBrightnessControlModesForEffect(Effect *effect, byte likelihood, byte likelihoodMusicBasedMode, byte likelihoodMovementBasedMode);
void incrementBrightnessModeLevels();

#endif