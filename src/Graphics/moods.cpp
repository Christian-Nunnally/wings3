
#include "../Graphics/moods.h"

void setupTestMood(EffectSettings *settings)
{
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetected = 20;
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero = 0;
    settings->LikelihoodEffectsAreRandomizedWhenAntiBeatDetected = 0;
    settings->LikelihoodEffectsAreRandomizedWhenStepIsDetected = 255;
    settings->LikelihoodNothingChangesWhenRandomizingEffect = 200;
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
    settings->AllowedEffects[1] = 0;
    settings->AllowedEffects[2] = 0;
    settings->AllowedEffects[3] = 0;
    settings->AllowedEffects[4] = 0;
    settings->AllowedEffects[5] = 0;
    settings->AllowedEffects[6] = 0;
    settings->AllowedEffects[7] = 0;
    settings->AllowedEffects[8] = 0;
    settings->AllowedEffects[9] = 0;
    settings->AllowedEffects[10] = 0;
    settings->AllowedEffects[11] = 0;
    settings->AllowedEffects[12] = 0;
    settings->AllowedEffects[13] = 0;
    settings->AllowedEffects[14] = 0;
    settings->AllowedEffects[15] = 0;
    settings->AllowedEffects[16] = 0;
    settings->AllowedEffects[17] = 0;
    settings->AllowedEffects[18] = 0;
    settings->AllowedEffects[19] = 0;

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

void setupNormalMood(EffectSettings *settings)
{
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetected = 255;
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero = 0;
    settings->LikelihoodEffectsAreRandomizedWhenAntiBeatDetected = 20;
    settings->LikelihoodEffectsAreRandomizedWhenStepIsDetected = 255;
    settings->LikelihoodNothingChangesWhenRandomizingEffect = 0;
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
    settings->MillisecondsForBlendingModeTransitionsMinimum = 50;
    settings->MillisecondsForBlendingModeTransitionsMaximum = 2000;
    settings->MillisecondsForBackgroundTransitionFadeMinimum = 50;
    settings->MillisecondsForBackgroundTransitionFadeMaximum = 10000;
    settings->GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps = 10000;
    settings->GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps = 40000;
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

void setupOldWingMood(EffectSettings *settings)
{
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetected = 255;
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero = 0;
    settings->LikelihoodEffectsAreRandomizedWhenAntiBeatDetected = 20;
    settings->LikelihoodEffectsAreRandomizedWhenStepIsDetected = 255;
    settings->LikelihoodNothingChangesWhenRandomizingEffect = 0;
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
    settings->MillisecondsForBlendingModeTransitionsMinimum = 50;
    settings->MillisecondsForBlendingModeTransitionsMaximum = 2000;
    settings->MillisecondsForBackgroundTransitionFadeMinimum = 50;
    settings->MillisecondsForBackgroundTransitionFadeMaximum = 10000;
    settings->GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps = 10000;
    settings->GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps = 40000;
    settings->LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect = 9;
    settings->AudioLevelThresholdToShowMoreIntenseEffectMinimum = 32767;
    settings->AudioLevelThresholdToShowMoreIntenseEffectMaximum = 64224;
    settings->LikelihoodWingsAreMirroredWhenTransformMapsAreRandomized = 128;

    settings->AllowedEffects[0] = 4;
    settings->AllowedEffects[1] = 4;
    settings->AllowedEffects[2] = 4;
    settings->AllowedEffects[3] = 4;
    settings->AllowedEffects[4] = 4;
    settings->AllowedEffects[5] = 4;
    settings->AllowedEffects[6] = 4;
    settings->AllowedEffects[7] = 4;
    settings->AllowedEffects[8] = 4;
    settings->AllowedEffects[9] = 4;
    settings->AllowedEffects[10] = 4;
    settings->AllowedEffects[11] = 4;
    settings->AllowedEffects[12] = 4;
    settings->AllowedEffects[13] = 4;
    settings->AllowedEffects[14] = 4;
    settings->AllowedEffects[15] = 4;
    settings->AllowedEffects[16] = 4;
    settings->AllowedEffects[17] = 4;
    settings->AllowedEffects[18] = 4;
    settings->AllowedEffects[19] = 4;

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