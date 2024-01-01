#ifndef TIME_MODES_H
#define TIME_MODES_H

#include "effect.h"
#include "../commonHeaders.h"

void setupTimeModes();
void pickRandomTimeModesForEffect(Effect *effect, bool pickFromBasicTimeModes, bool pickFromMusicBasedTimeModes, bool pickFromMovementBasedTimeModes, uint8_t likelihoodIndividualModeChanges);
void incrementTimesForEffect(Effect *effect, int timeDelta);
void pickRandomTimeModesForAllEffects(Effect *effect1, Effect *effect2, Effect *effect3, Effect *effect4, uint8_t likelihoodMovementBaseModeIsPicked, uint8_t likelihoodMusicBaseModeIsPicked, uint8_t likelihoodIndividualModeChanges, bool shouldTryToPickMovementMode, bool shouldTryToPickMusicMode);
float getGlobalTimeFactor();
void setGlobalTimeFactor(float newTimeFactor);

#endif