
#include "../Graphics/timeModes.h"
#include "../Graphics/effects.h"
#include "../Peripherals/movementDetection.h"
#include "../Utility/fastRandom.h"

const int TimeDeltaResolutionIncreaseFactor = 64;
const float AudioInfluenceFactorForAudioScaledTime = 2.0;
const uint8_t MaxNumberOfTimeModes = 40;
uint8_t numberOfBasicTimeModes;
uint8_t numberOfMusicBasedTimeModes;
uint8_t numberOfMovementBasedTimeModes;
int (*timeModeIncrementFunctions[MaxNumberOfTimeModes])(int currentTime, int timeDelta);

void setupTimeModes()
{
    int timeModesSoFar = 0;
    timeModeIncrementFunctions[timeModesSoFar + numberOfBasicTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + (timeDelta >> 1); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfBasicTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)timeDelta / 3); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfBasicTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)timeDelta >> 2); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfBasicTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - (timeDelta >> 1); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfBasicTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)timeDelta / 3); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfBasicTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)timeDelta >> 2); };

    timeModesSoFar = numberOfBasicTimeModes;
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 1); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) / 3); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 2); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime + ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 1); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) / 3); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 2); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMusicBasedTimeModes++] = [](int currentTime, int timeDelta) { return currentTime - ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };

    timeModesSoFar = numberOfBasicTimeModes + numberOfMusicBasedTimeModes;
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 11); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 11); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 11); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 12); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 12); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 12); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 10); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 10); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 10); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 11)); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 11)); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 11)); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 12)); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 12)); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 12)); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 10)); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 10)); };
    timeModeIncrementFunctions[timeModesSoFar + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 10)); };
}

void pickRandomTimeModesForEffect(Effect *effect, bool pickFromBasicTimeModes, bool pickFromMusicBasedTimeModes, bool pickFromMovementBasedTimeModes, uint8_t likelihoodIndividualModeChanges)
{
    if (pickFromBasicTimeModes)
    {
        if (pickFromMusicBasedTimeModes)
        {
            if (pickFromMovementBasedTimeModes)
            {
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode1 = fastRandomInteger(numberOfBasicTimeModes + numberOfMusicBasedTimeModes + numberOfMovementBasedTimeModes);
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode2 = fastRandomInteger(numberOfBasicTimeModes + numberOfMusicBasedTimeModes + numberOfMovementBasedTimeModes);
            }
            else 
            {
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode1 = fastRandomInteger(numberOfBasicTimeModes + numberOfMusicBasedTimeModes);
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode2 = fastRandomInteger(numberOfBasicTimeModes + numberOfMusicBasedTimeModes);
            }
        }
        else 
        {
            if (pickFromMovementBasedTimeModes)
            {
                if (fastRandomBoolean())
                {
                    if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode1 = fastRandomInteger(numberOfBasicTimeModes);
                    if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode2 = fastRandomInteger(numberOfBasicTimeModes);
                }
                else 
                {
                    if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode1 = fastRandomInteger(numberOfBasicTimeModes + numberOfMusicBasedTimeModes, numberOfBasicTimeModes + numberOfMusicBasedTimeModes + numberOfMovementBasedTimeModes);
                    if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode2 = fastRandomInteger(numberOfBasicTimeModes + numberOfMusicBasedTimeModes, numberOfBasicTimeModes + numberOfMusicBasedTimeModes + numberOfMovementBasedTimeModes);
                }
            }
            else 
            {
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode1 = fastRandomInteger(numberOfBasicTimeModes);
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode2 = fastRandomInteger(numberOfBasicTimeModes);
            }
        }
    }
    else 
    {
        if (pickFromMusicBasedTimeModes)
        {
            if (pickFromMovementBasedTimeModes)
            {
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode1 = fastRandomInteger(numberOfBasicTimeModes, numberOfBasicTimeModes + numberOfMusicBasedTimeModes + numberOfMovementBasedTimeModes);
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode2 = fastRandomInteger(numberOfBasicTimeModes, numberOfBasicTimeModes + numberOfMusicBasedTimeModes + numberOfMovementBasedTimeModes);
            }
            else 
            {
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode1 = fastRandomInteger(numberOfBasicTimeModes, numberOfBasicTimeModes + numberOfMusicBasedTimeModes);
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode2 = fastRandomInteger(numberOfBasicTimeModes, numberOfBasicTimeModes + numberOfMusicBasedTimeModes);
            }
        }
        else 
        {
            if (pickFromMovementBasedTimeModes)
            {
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode1 = fastRandomInteger(numberOfBasicTimeModes + numberOfMusicBasedTimeModes, numberOfBasicTimeModes + numberOfMusicBasedTimeModes + numberOfMovementBasedTimeModes);
                if (fastRandomByte() < likelihoodIndividualModeChanges) effect->timeMode2 = fastRandomInteger(numberOfBasicTimeModes + numberOfMusicBasedTimeModes, numberOfBasicTimeModes + numberOfMusicBasedTimeModes + numberOfMovementBasedTimeModes);
            }
        }
    }
}

void incrementTimesForEffect(Effect *effect, int timeDelta)
{
    int increasedResolutionTimeDelta = (timeDelta * TimeDeltaResolutionIncreaseFactor);
    effect->time1 = timeModeIncrementFunctions[effect->timeMode1](effect->time1 * TimeDeltaResolutionIncreaseFactor, increasedResolutionTimeDelta) / TimeDeltaResolutionIncreaseFactor;
    effect->time2 = timeModeIncrementFunctions[effect->timeMode1](effect->time1 * TimeDeltaResolutionIncreaseFactor, increasedResolutionTimeDelta) / TimeDeltaResolutionIncreaseFactor;
}

void pickRandomTimeModesForAllEffects(Effect *effect1, Effect *effect2, Effect *effect3, Effect *effect4, uint8_t likelihoodMovementBaseModeIsPicked, uint8_t likelihoodMusicBaseModeIsPicked, uint8_t likelihoodIndividualModeChanges, bool shouldTryToPickMovementMode, bool shouldTryToPickMusicMode)
{
    pickRandomTimeModesForEffect(effect1, true, (fastRandomByte() < likelihoodMusicBaseModeIsPicked) && shouldTryToPickMusicMode, (fastRandomByte() < likelihoodMovementBaseModeIsPicked) && shouldTryToPickMovementMode, likelihoodIndividualModeChanges);
    pickRandomTimeModesForEffect(effect2, true, (fastRandomByte() < likelihoodMusicBaseModeIsPicked) && shouldTryToPickMusicMode, (fastRandomByte() < likelihoodMovementBaseModeIsPicked) && shouldTryToPickMovementMode, likelihoodIndividualModeChanges);
    pickRandomTimeModesForEffect(effect3, true, (fastRandomByte() < likelihoodMusicBaseModeIsPicked) && shouldTryToPickMusicMode, (fastRandomByte() < likelihoodMovementBaseModeIsPicked) && shouldTryToPickMovementMode, likelihoodIndividualModeChanges);
    pickRandomTimeModesForEffect(effect4, true, (fastRandomByte() < likelihoodMusicBaseModeIsPicked) && shouldTryToPickMusicMode, (fastRandomByte() < likelihoodMovementBaseModeIsPicked) && shouldTryToPickMovementMode, likelihoodIndividualModeChanges);

    // int startIndex = 0;
    // int length = numberOfBasicTimeModes;
    // if ((fastRandomByte() < likelihoodMovementBaseModeIsPicked) && (getCurrentMovementType() != Stationary))
    // {
    //     startIndex = numberOfBasicTimeModes;
    //     length = numberOfMovementBasedTimeModes;
    // }
    // if (fastRandomByte() < likelihoodIndividualModeChanges) effect1->timeMode1 = fastRandomInteger(length) + startIndex;
    // if (fastRandomByte() < likelihoodIndividualModeChanges) effect2->timeMode1 = fastRandomInteger(length) + startIndex;
    // if (fastRandomByte() < likelihoodIndividualModeChanges) effect3->timeMode1 = fastRandomInteger(length) + startIndex;
    // if (fastRandomByte() < likelihoodIndividualModeChanges) effect4->timeMode1 = fastRandomInteger(length) + startIndex;
    // if (fastRandomByte() < likelihoodIndividualModeChanges) effect1->timeMode2 = fastRandomInteger(length) + startIndex;
    // if (fastRandomByte() < likelihoodIndividualModeChanges) effect2->timeMode2 = fastRandomInteger(length) + startIndex;
    // if (fastRandomByte() < likelihoodIndividualModeChanges) effect3->timeMode2 = fastRandomInteger(length) + startIndex;
    // if (fastRandomByte() < likelihoodIndividualModeChanges) effect4->timeMode2 = fastRandomInteger(length) + startIndex;
}