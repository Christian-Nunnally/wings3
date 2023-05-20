#ifndef SERIAL_H
#define SERIAL_H

#include <Arduino.h>
#include "../Graphics/effect.h"

void setupTimeModes();
void pickRandomTimeModesForEffect(Effect *effect, bool pickFromMovementBasedTimeModes, bool pickFromNonMovementBasedTimeModes);
void incrementTimesForEffect(Effect *effect, int timeDelta);
void pickRandomTimeModesForAllEffects(Effect *effect1, Effect *effect2, Effect *effect3, Effect *effect4, byte likelihoodMovementBaseModeIsPicked, byte likelihoodIndividualModeChanges);

#endif