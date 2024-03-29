
#include "../Graphics/moods.h"
#include "../Graphics/effects.h"
#include "../Graphics/palettes.h"

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
    settings->LikelihoodBackgroundTransitionTimeChangesWhenRandomizingEffect = 6;
    settings->LikelihoodScreenMapChangesWhenRandomizingEffect = 4;
    settings->LikelihoodEffectsAreSwappedWhenRandomizingEffect = 11;
    settings->LikelihoodTransformMapsAreSwitchedWhenEffectsAreSwapped = 120;
    settings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched = 128;
    settings->LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect = 4;
    settings->LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized = 60;
    settings->LikelihoodTimeModesRandomizeWhenRandomizingEffect = 30;
    settings->LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes = 100;
    settings->LikelihoodEffectsAreRandomizedWhenRandomizingEffect = 8;
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
    settings->GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps = 200;
    settings->GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps = 10;
    settings->LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect = 9;
    settings->AudioLevelThresholdToShowMoreIntenseEffectMinimum = 80;
    settings->AudioLevelThresholdToShowMoreIntenseEffectMaximum = 255;
    settings->LikelihoodWingsAreMirroredWhenTransformMapsAreRandomized = 200;
    settings->MinimumEffectSize = 3;
    settings->MaximumEffectSize = 200;
    settings->CurrentScreenMapIndex = 0;
    settings->PrimaryEffectToggle = false;
    settings->RandomizeEffectsAutomaticallyOverTime = true;
    settings->MillisecondsBetweenTimeBasedAutomaticEffectRandomization = 1000;

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
    settings->AllowedEffects[14] = METEOR_RAIN_EFFECT_INDEX;
    settings->AllowedEffects[15] = METEOR_RAIN_EFFECT_INDEX;
    settings->AllowedEffects[16] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[17] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[18] = GRADIENT_WAVE_EFFECT_INDEX;
    settings->AllowedEffects[19] = GRADIENT_WAVE_EFFECT_INDEX;

    for (int i = 0; i < NumberOfAllowedPalettesToPickBetween; i++)
    {
        settings->AllowedPalettes[i][0] = NumberOfAllowedPalettesToPickBetween;
        for (int j = 0; j < NumberOfAllowedPalettesToPickBetween; j++)
        {
            settings->AllowedPalettes[i][j + 1] = j % TOTAL_NUMBER_OF_PALETTES;
        }
    }
}