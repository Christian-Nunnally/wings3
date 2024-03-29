#ifndef EFFECT_SETTINGS_H
#define EFFECT_SETTINGS_H

#include "../commonHeaders.h"
#include "../Graphics/palettes.h"

#define EFFECT_NAME 0

const int NumberOfAllowedEffectsToPickBetween = 20;
const int NumberOfAllowedPalettesToPickBetween = TOTAL_NUMBER_OF_PALETTES * 5;

typedef struct
{
  uint8_t LikelihoodEffectsAreRandomizedWhenBeatDetected;
  uint8_t LikelihoodEffectsAreRandomizedWhenAntiBeatDetected;
  uint8_t LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero;
  uint8_t LikelihoodEffectsAreRandomizedWhenStepIsDetected;

  uint8_t LikelihoodNothingChangesWhenRandomizingEffect;
  uint8_t LikelihoodBlendingModeChangesWhenRandomizingEffect;
  uint8_t LikelihoodSubPaletteChangesWhenRandomizingEffect;
  uint8_t LikelihoodPaletteChangesWhenRandomizingEffect;
  uint8_t LikelihoodTwoPalettesAreUsedWhenPaletteChanges;

  uint8_t LikelihoodBackgroundTransitionTimeChangesWhenRandomizingEffect;
  uint8_t LikelihoodScreenMapChangesWhenRandomizingEffect;
  uint8_t LikelihoodEffectsAreSwappedWhenRandomizingEffect;
  uint8_t LikelihoodTransformMapsAreSwitchedWhenEffectsAreSwapped;
  uint8_t LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched;
  uint8_t LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect;
  uint8_t LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized;

  uint8_t LikelihoodTimeModesRandomizeWhenRandomizingEffect;
  uint8_t LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes;

  uint8_t LikelihoodEffectsAreRandomizedWhenRandomizingEffect;

  uint8_t LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect;

  uint8_t LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect;
  uint8_t LikelihoodIndividualGlobalBrightnessModesChange;

  uint8_t LikelihoodTransitionDirectionChangesWhenRandomizingEffect;

  uint8_t LikelihoodMovementBasedBrightnessModeIsPicked;
  uint8_t LikelihoodMusicBasedBrightnessModeIsPicked;
  uint8_t LikelihoodMovementBasedTimeModeIsPicked;
  
  uint16_t MillisecondToMoveToNextPaletteFrame;
  uint16_t MillisecondsForEffectTransitionsMinimum;
  uint16_t MillisecondsForEffectTransitionsMaximum;
  uint16_t MillisecondsForBlendingModeTransitionsMinimum;
  uint16_t MillisecondsForBlendingModeTransitionsMaximum;

  uint8_t GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps;
  uint8_t GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps;

  uint8_t LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect;
  uint8_t AudioLevelThresholdToShowMoreIntenseEffectMinimum;
  uint8_t AudioLevelThresholdToShowMoreIntenseEffectMaximum;

  uint8_t AllowedEffects[NumberOfAllowedEffectsToPickBetween];
  uint8_t AllowedPalettes[NumberOfAllowedPalettesToPickBetween][NumberOfAllowedPalettesToPickBetween + 1];

  uint8_t MinimumEffectSize;
  uint8_t MaximumEffectSize;

  uint8_t LikelihoodWingsAreMirroredWhenTransformMapsAreRandomized;

  uint8_t CurrentScreenMapIndex;

  bool PrimaryEffectToggle = false;
  bool RandomizeEffectsAutomaticallyOverTime = false;
  uint16_t MillisecondsBetweenTimeBasedAutomaticEffectRandomization = 1000;

  uint16_t millisecondsLeftInMixingModeBlend;
  uint16_t desiredDurationOfMixingModeBlendInMilliseconds = 1;
  uint8_t mixingModeBlendFunctionIndex = 0;
  uint8_t oldMixingModeBlendFunctionIndex = 0;

  uint16_t percentOfEffectBToShow;
  uint8_t percentOfSecondaryEffectToShow;
  uint16_t currentTransitionIncrement;
  uint16_t millisecondsLeftInTransitionFromSecondaryToPrimaryEffect;
  uint16_t millisecondsLeftInTransitionFromSecondaryToPrimaryEffectMax = 1;
  bool transitionDirection;
}
EffectSettings;

extern EffectSettings effectSettings;

#endif