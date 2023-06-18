#ifndef TIME_MODES_H
#define TIME_MODES_H

#include <Arduino.h>
#include "../Graphics/effect.h"

void setupTimeModes();
void pickRandomTimeModesForEffect(Effect *effect, bool pickFromBasicTimeModes, bool pickFromMusicBasedTimeModes, bool pickFromMovementBasedTimeModes, byte likelihoodIndividualModeChanges);
void incrementTimesForEffect(Effect *effect, int timeDelta);
void pickRandomTimeModesForAllEffects(Effect *effect1, Effect *effect2, Effect *effect3, Effect *effect4, byte likelihoodMovementBaseModeIsPicked, byte likelihoodMusicBaseModeIsPicked, byte likelihoodIndividualModeChanges, bool shouldTryToPickMovementMode, bool shouldTryToPickMusicMode);

#endif