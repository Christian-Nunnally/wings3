
#include "../Graphics/moods.h"

void setupNormalMood(EffectSettings *settings)
{
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetected = 230;
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero = 10;
    settings->LikelihoodEffectsAreRandomizedWhenAntiBeatDetected = 10;
    settings->LikelihoodEffectsAreRandomizedWhenStepIsDetected = 230;
    settings->LikelihoodNothingChangesWhenRandomizingEffect = 50;
    settings->LikelihoodBlendingModeChangesWhenRandomizingEffect = 10;
    settings->LikelihoodSubPaletteChangesWhenRandomizingEffect = 40;
    settings->LikelihoodPaletteChangesWhenRandomizingEffect = 20;
    settings->LikelihoodTwoPalettesAreUsedWhenPaletteChanges = 50;
    settings->LikelihoodBackgroundTransitionTimeChangesWhenRandomizingEffect = 10;
    settings->LikelihoodScreenMapChangesWhenRandomizingEffect = 4;
    settings->LikelihoodEffectsAreSwappedWhenRandomizingEffect = 20;
    settings->LikelihoodTransformMapsAreSwitchedWhenEffectsAreSwapped = 200;
    settings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched = 128;
    settings->LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect = 4;
    settings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized = 100;
    settings->LikelihoodTimeModesRandomizeWhenRandomizingEffect = 30;
    settings->LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes = 100;
    settings->LikelihoodEffectsAndRandomizedWhenRandomizingEffect = 15;
    settings->LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect = 30;
    settings->LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect = 30;
    settings->LikelihoodIndividualGlobalBrightnessModesChange = 128;
    settings->LikelihoodTransitionDirectionChangesWhenRandomizingEffect = 15;
    settings->LikelihoodMovementBasedBrightnessModeIsPicked = 200;
    settings->LikelihoodMusicBasedBrightnessModeIsPicked = 100;
    settings->LikelihoodMovementBasedTimeModeIsPicked = 200;
    settings->MillisecondToMoveToNextPaletteFrame = 50;
    settings->MillisecondsForEffectTransitionsMinimum = 500;
    settings->MillisecondsForEffectTransitionsMaximum = 2000;
    settings->MillisecondsForBlendingModeTransitionsMinimum = 100;
    settings->MillisecondsForBlendingModeTransitionsMaximum = 2000;
    settings->MillisecondsForBackgroundTransitionFadeMinimum = 2000;
    settings->MillisecondsForBackgroundTransitionFadeMaximum = 10000;
    settings->GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps = 1000;
    settings->GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps = 10000;
    settings->LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect = 9;
    settings->AudioLevelThresholdToShowMoreIntenseEffectMinimum = 32767;
    settings->AudioLevelThresholdToShowMoreIntenseEffectMaximum = 64224;
    settings->LikelihoodWingsAreMirroredWhenTransformMapsAreRandomized = 128;

    settings->AllowedEffects[0] = 0;
    settings->AllowedEffects[1] = 1;
    settings->AllowedEffects[2] = 2;
    settings->AllowedEffects[3] = 3;
    settings->AllowedEffects[4] = 4;
    settings->AllowedEffects[5] = 5;
    settings->AllowedEffects[6] = 6;
    settings->AllowedEffects[7] = 7;
    settings->AllowedEffects[8] = 8;
    settings->AllowedEffects[9] = 9;
    settings->AllowedEffects[10] = 10;
    settings->AllowedEffects[11] = 11;
    settings->AllowedEffects[12] = 12;
    settings->AllowedEffects[13] = 13;
    settings->AllowedEffects[14] = 14;
    settings->AllowedEffects[15] = 15;
    settings->AllowedEffects[16] = 16;
    settings->AllowedEffects[17] = 17;
    settings->AllowedEffects[18] = 18;
    settings->AllowedEffects[19] = 9;

    for (int i = 0; i < NumberOfAllowedPalettesToPickBetween; i++)
    {
        settings->AllowedPalettes[i][0] = NumberOfAllowedPalettesToPickBetween;
        for (int j = 0; j < NumberOfAllowedPalettesToPickBetween; j++)
        {
            settings->AllowedPalettes[i][j + 1] = j;
        }
    }

    settings->MinimumEffectSize = 10;
    settings->MaximumEffectSize = 200;
}