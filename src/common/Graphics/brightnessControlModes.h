#ifndef BRIGHTNESS_CONTROL_MODES_H
#define BRIGHTNESS_CONTROL_MODES_H

#include "../commonHeaders.h"
#include "effect.h"

void pickRandomGlobalBrightnessControlModesForEffect(Effect *effect, uint8_t likelihood, uint8_t likelihoodMusicBasedMode, uint8_t likelihoodMovementBasedMode);
void incrementBrightnessModeLevels();
void setBrightnessPointerFromIndexForEffect(Effect *effect);

#endif