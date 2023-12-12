#ifndef BRIGHTNESS_CONTROL_MODES_H
#define BRIGHTNESS_CONTROL_MODES_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "../Graphics/effect.h"

void pickRandomGlobalBrightnessControlModesForEffect(Effect *effect, uint8_t likelihood, uint8_t likelihoodMusicBasedMode, uint8_t likelihoodMovementBasedMode);
void incrementBrightnessModeLevels();

#endif