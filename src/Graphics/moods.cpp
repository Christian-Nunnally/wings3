
#include "../Graphics/moods.h"
#include "../Graphics/effects.h"

void setupNormalMood(EffectSettings *settings)
{
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetected = 255;
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero = 20;
    settings->LikelihoodEffectsAreRandomizedWhenAntiBeatDetected = 20;
    settings->LikelihoodEffectsAreRandomizedWhenStepIsDetected = 255;
    settings->LikelihoodNothingChangesWhenRandomizingEffect = 0;
    settings->LikelihoodBlendingModeChangesWhenRandomizingEffect = 10;
    settings->LikelihoodSubPaletteChangesWhenRandomizingEffect = 40;
    settings->LikelihoodPaletteChangesWhenRandomizingEffect = 20;
    settings->LikelihoodTwoPalettesAreUsedWhenPaletteChanges = 50;
    settings->LikelihoodBackgroundTransitionTimeChangesWhenRandomizingEffect = 10;
    settings->LikelihoodScreenMapChangesWhenRandomizingEffect = 4;
    settings->LikelihoodEffectsAreSwappedWhenRandomizingEffect = 15;
    settings->LikelihoodTransformMapsAreSwitchedWhenEffectsAreSwapped = 180;
    settings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched = 128;
    settings->LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect = 4;
    settings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized = 100;
    settings->LikelihoodTimeModesRandomizeWhenRandomizingEffect = 30;
    settings->LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes = 100;
    settings->LikelihoodEffectsAndRandomizedWhenRandomizingEffect = 10;
    settings->LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect = 30;
    settings->LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect = 30;
    settings->LikelihoodIndividualGlobalBrightnessModesChange = 128;
    settings->LikelihoodTransitionDirectionChangesWhenRandomizingEffect = 15;
    settings->LikelihoodMovementBasedBrightnessModeIsPicked = 200;
    settings->LikelihoodMusicBasedBrightnessModeIsPicked = 80;
    settings->LikelihoodMovementBasedTimeModeIsPicked = 200;
    settings->MillisecondToMoveToNextPaletteFrame = 50;
    settings->MillisecondsForEffectTransitionsMinimum = 10;
    settings->MillisecondsForEffectTransitionsMaximum = 2000;
    settings->MillisecondsForBlendingModeTransitionsMinimum = 200;
    settings->MillisecondsForBlendingModeTransitionsMaximum = 2000;
    settings->MillisecondsForBackgroundTransitionFadeMinimum = 50;
    settings->MillisecondsForBackgroundTransitionFadeMaximum = 10000;
    settings->GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps = 250;
    settings->GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps = 220;
    settings->LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect = 9;
    settings->AudioLevelThresholdToShowMoreIntenseEffectMinimum = 80;
    settings->AudioLevelThresholdToShowMoreIntenseEffectMaximum = 255;
    settings->LikelihoodWingsAreMirroredWhenTransformMapsAreRandomized = 200;
    settings->MinimumEffectSize = 10;
    settings->MaximumEffectSize = 200;

    settings->AllowedEffects[0] = METEOR_RAIN_EFFECT_INDEX;
    settings->AllowedEffects[1] = METEOR_RAIN_EFFECT_2_INDEX;
    settings->AllowedEffects[2] = STAR_FIELD_EFFECT_INDEX;
    settings->AllowedEffects[3] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[4] = RAIN_SHOWER_EFFECT_INDEX;
    settings->AllowedEffects[5] = EXPANDING_COLOR_ORB_EFFECT_INDEX;
    settings->AllowedEffects[6] = LIGHTNING_BUG_EFFECT_INDEX;
    settings->AllowedEffects[7] = FIRE_EFFECT_INDEX;
    settings->AllowedEffects[8] = SIMPLE_SOLID_COLOR_FILL_EFFECT_INDEX;
    settings->AllowedEffects[9] = FIREWORKS_EFFECT_INDEX;
    settings->AllowedEffects[10] = LIGHT_CHASE_EFFECT_INDEX;
    settings->AllowedEffects[11] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[12] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[13] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[14] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[15] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[16] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[17] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[18] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[19] = GRADIENT_WAVE_EFFECT_INDEX;

    for (int i = 0; i < NumberOfAllowedPalettesToPickBetween; i++)
    {
        settings->AllowedPalettes[i][0] = NumberOfAllowedPalettesToPickBetween;
        for (int j = 0; j < NumberOfAllowedPalettesToPickBetween; j++)
        {
            settings->AllowedPalettes[i][j + 1] = j;
        }
    }
}

void setupTestMood(EffectSettings *settings)
{
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetected = 0;
    settings->LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero = 0;
    settings->LikelihoodEffectsAreRandomizedWhenAntiBeatDetected = 0;
    settings->LikelihoodEffectsAreRandomizedWhenStepIsDetected = 0;
    settings->LikelihoodNothingChangesWhenRandomizingEffect = 0;
    settings->LikelihoodBlendingModeChangesWhenRandomizingEffect = 0;
    settings->LikelihoodSubPaletteChangesWhenRandomizingEffect = 0;
    settings->LikelihoodPaletteChangesWhenRandomizingEffect = 0;
    settings->LikelihoodTwoPalettesAreUsedWhenPaletteChanges = 0;
    settings->LikelihoodBackgroundTransitionTimeChangesWhenRandomizingEffect = 0;
    settings->LikelihoodScreenMapChangesWhenRandomizingEffect = 0;
    settings->LikelihoodEffectsAreSwappedWhenRandomizingEffect = 0;
    settings->LikelihoodTransformMapsAreSwitchedWhenEffectsAreSwapped = 0;
    settings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched = 0;
    settings->LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect = 0;
    settings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized = 0;
    settings->LikelihoodTimeModesRandomizeWhenRandomizingEffect = 0;
    settings->LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes = 0;
    settings->LikelihoodEffectsAndRandomizedWhenRandomizingEffect = 0;
    settings->LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect = 0;
    settings->LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect = 0;
    settings->LikelihoodIndividualGlobalBrightnessModesChange = 0;
    settings->LikelihoodTransitionDirectionChangesWhenRandomizingEffect = 0;
    settings->LikelihoodMovementBasedBrightnessModeIsPicked = 0;
    settings->LikelihoodMusicBasedBrightnessModeIsPicked = 0;
    settings->LikelihoodMovementBasedTimeModeIsPicked = 0;
    settings->MillisecondToMoveToNextPaletteFrame = 0;
    settings->MillisecondsForEffectTransitionsMinimum = 0;
    settings->MillisecondsForEffectTransitionsMaximum = 0;
    settings->MillisecondsForBlendingModeTransitionsMinimum = 0;
    settings->MillisecondsForBlendingModeTransitionsMaximum = 0;
    settings->MillisecondsForBackgroundTransitionFadeMinimum = 0;
    settings->MillisecondsForBackgroundTransitionFadeMaximum = 0;
    settings->GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps = 0;
    settings->GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps = 0;
    settings->LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect = 0;
    settings->AudioLevelThresholdToShowMoreIntenseEffectMinimum = 0;
    settings->AudioLevelThresholdToShowMoreIntenseEffectMaximum = 0;
    settings->LikelihoodWingsAreMirroredWhenTransformMapsAreRandomized = 0;
    settings->MinimumEffectSize = 10;
    settings->MaximumEffectSize = 200;

    for (int i = 0; i < 20; i++)
    {
        settings->AllowedEffects[i] = FIREWORKS_EFFECT_INDEX;
    }

    // settings->AllowedEffects[0] = METEOR_RAIN_EFFECT_INDEX;
    // settings->AllowedEffects[1] = METEOR_RAIN_EFFECT_2_INDEX;
    // settings->AllowedEffects[2] = STAR_FIELD_EFFECT_INDEX;
    // settings->AllowedEffects[3] = GRADIENT_WAVE_EFFECT_INDEX;
    // settings->AllowedEffects[4] = RAIN_SHOWER_EFFECT_INDEX;
    // settings->AllowedEffects[5] = EXPANDING_COLOR_ORB_EFFECT_INDEX;
    // settings->AllowedEffects[6] = LIGHTNING_BUG_EFFECT_INDEX;
    // settings->AllowedEffects[7] = FIRE_EFFECT_INDEX;
    // settings->AllowedEffects[8] = SIMPLE_SOLID_COLOR_FILL_EFFECT_INDEX;
    // settings->AllowedEffects[9] = FIREWORKS_EFFECT_INDEX;
    // settings->AllowedEffects[10] = LIGHT_CHASE_EFFECT_INDEX;
    // settings->AllowedEffects[11] = GRADIENT_WAVE_EFFECT_INDEX;
    // settings->AllowedEffects[12] = GRADIENT_WAVE_EFFECT_INDEX;
    // settings->AllowedEffects[13] = GRADIENT_WAVE_EFFECT_INDEX;
    // settings->AllowedEffects[14] = GRADIENT_WAVE_EFFECT_INDEX;
    // settings->AllowedEffects[15] = GRADIENT_WAVE_EFFECT_INDEX;
    // settings->AllowedEffects[16] = GRADIENT_WAVE_EFFECT_INDEX;
    // settings->AllowedEffects[17] = GRADIENT_WAVE_EFFECT_INDEX;
    // settings->AllowedEffects[18] = GRADIENT_WAVE_EFFECT_INDEX;
    // settings->AllowedEffects[19] = GRADIENT_WAVE_EFFECT_INDEX;

    for (int i = 0; i < NumberOfAllowedPalettesToPickBetween; i++)
    {
        settings->AllowedPalettes[i][0] = NumberOfAllowedPalettesToPickBetween;
        for (int j = 0; j < NumberOfAllowedPalettesToPickBetween; j++)
        {
            settings->AllowedPalettes[i][j + 1] = j;
        }
    }
}