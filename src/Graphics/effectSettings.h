#ifndef EFFECT_SETTINGS_H
#define EFFECT_SETTINGS_H

#include <Arduino.h>

typedef struct
{
  byte LikelihoodEffectsAreRandomizedWhenBeatDetected;
  byte LikelihoodEffectsAreRandomizedWhenAntiBeatDetected;
  byte LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero;
  byte LikelihoodEffectsAreRandomizedWhenStepIsDetected;

  byte LikelihoodNothingChangesWhenRandomizingEffect;
  byte LikelihoodBlendingModeChangesWhenRandomizingEffect;
  byte LikelihoodSubPaletteChangesWhenRandomizingEffect;
  byte LikelihoodSubPalettesAreDifferentWhenChangingSubPalettes;
  byte LikelihoodPaletteChangesWhenRandomizingEffect;
  byte LikelihoodTwoPalettesAreUsedWhenPaletteChanges;

  byte LikelihoodEffectsAreSwappedWhenRandomizingEffect;
  byte LikelihoodTransformMapsAreSwitchedWhenEffectsAreSwapped;
  byte LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched;
  byte LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect;
  byte LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized;

  byte LikelihoodTimeModesRandomizeWhenRandomizingEffect;
  byte LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes;

  byte LikelihoodEffectsAndRandomizedWhenRandomizingEffect;

  byte LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect;

  byte LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect;
  byte LikelihoodIndividualGlobalBrightnessModesChange;

  byte LikelihoodTransitionDirectionChangesWhenRandomizingEffect;
}
EffectSettings;

#endif