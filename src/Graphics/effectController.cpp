#include <Arduino.h>
#include "../settings.h"
#include "../Peripherals/microphone.h"
#include "../Peripherals/movementDetection.h"
#include "../Graphics/graphics.h"
#include "../Graphics/effectController.h"
#include "../Graphics/transformMaps.h"
#include "../Graphics/screenMaps.h"
#include "../Graphics/effectSettings.h"
#include "../Graphics/colorMixing.h"
#include "../Utility/time.h"
#include "../IO/analogInput.h"
#include "../Observers/stepDectectedObserver.h"
#include "../Observers/movementDetectedObserver.h"
#include "../Utility/fastRandom.h"

#include "palettes.h"
//#include "palettesMinimal.h"

Color blendIncorporatingOldMixingMode(Color color1, Color color2);
void handleStepDetected();
void handleMovementDetected();
void incrementBrightnessModeLevels(float currentAudioIntensityLevel);
void incrementTime();
void incrementTransition();
void incrementColorPalettesTowardsTargets();
void detectBeat();
void randomizeEffectsNaturally();
void pickRandomBlendingMode();
void pickRandomSubPalette();
void pickRandomPalette();
int pickRandomSubPaletteFromPalette(int palette);
void pickRandomTransformMaps();
void swapEffects();
void pickRandomScreenMap();
void pickRandomTimeModes();
void pickRandomEffects();
void pickRandomSizeParametersForEffects();
void pickRandomGlobalBrightnessControlModes();
void pickRandomGlobalBrightnessControlModesOfTheSameType();
void pickRandomGlobalBrightnessControlModesOfDifferentTypes();
void pickRandomTransitionTime();
void pickRandomAudioLevelThresholdForMoreIntenseEffect();
void switchTransitionDirection();

Color (*effect1)(int pixel) {};
Color (*effect1Plus)(int pixel) {};
Color (*effect2)(int pixel) {};
Color (*effect2Plus)(int pixel) {};
Color (*effect3)(int pixel) {};
Color (*effect3Plus)(int pixel) {};
Color (*effect4)(int pixel) {};
Color (*effect4Plus)(int pixel) {};

Color (*frameEffect1)(int pixel) {};
Color (*frameEffect2)(int pixel) {};
Color (*frameEffect3)(int pixel) {};
Color (*frameEffect4)(int pixel) {};
Color (*frameEffect1Plus)(int pixel) {};
Color (*frameEffect2Plus)(int pixel) {};
Color (*frameEffect3Plus)(int pixel) {};
Color (*frameEffect4Plus)(int pixel) {};

const int MaxNumberOfMixingModeBlendFunctions = 20;
int numberOfMixingModeBlendFunctions;
Color (*mixingModeBlendFunctions[MaxNumberOfMixingModeBlendFunctions])(Color color1, Color color2, uint16_t transitionAmount);
Color (*mixingModeBlendFunction)(Color color1, Color color2, uint16_t transitionAmount) {};
Color (*oldMixingModeBlendFunction)(Color color1, Color color2, uint16_t transitionAmount) {};

uint8_t const *effect1TransformMap1[272];
uint8_t const *effect1TransformMap2[272];
uint8_t const *effect2TransformMap1[272];
uint8_t const *effect2TransformMap2[272];
uint8_t const *effect3TransformMap1[272];
uint8_t const *effect3TransformMap2[272];
uint8_t const *effect4TransformMap1[272];
uint8_t const *effect4TransformMap2[272];

int frameTimeDelta;
int effect1Time1;
int effect2Time1;
int effect3Time1;
int effect4Time1;
int effect1Time2;
int effect2Time2;
int effect3Time2;
int effect4Time2;

byte effect1TimeMode1;
byte effect2TimeMode1;
byte effect3TimeMode1;
byte effect4TimeMode1;
byte effect1TimeMode2;
byte effect2TimeMode2;
byte effect3TimeMode2;
byte effect4TimeMode2;

int currentPalette1;
int currentPalette2;
int currentPalette1Offset; 
int currentPalette2Offset;
int currentPalette1OffsetTarget; 
int currentPalette2OffsetTarget; 
int *currentPalette1OffsetPointer; 
int *currentPalette2OffsetPointer; 

uint16_t *effect1GlobalBrightnessPointer; 
uint16_t *effect2GlobalBrightnessPointer; 
uint16_t *effect3GlobalBrightnessPointer; 
uint16_t *effect4GlobalBrightnessPointer; 

const byte MaxNumberOfTimeModes = 40;
byte numberOfTimeModes;
byte numberOfMovementBasedTimeModes;
int (*timeModeIncrementFunctions[MaxNumberOfTimeModes])(int currentTime, int timeDelta);

const byte NumberOfNormalGlobalBrightnessChangeModes = 3;
const byte NumberOfMovementBasedGlobalBrightnessChangeModes = 3;
const byte NumberOfMusicBasedGlobalBrightnessChangeModes = 3;
uint16_t brightnessModeMaxBrightness = UINT16_MAX;
uint16_t brightnessModeAlmostMaxBrightness = 50000;
uint16_t brightnessModeHalfBrightness = UINT16_MAX > 1;
uint16_t brightnessModeAudioLevelBasedBrightness;
uint16_t brightnessModeAudioLevelBasedBrightnessBrighter;
uint16_t brightnessModeAudioLevelBasedBrightnessInverse;
uint16_t brightnessModePitchBasedMovement;
uint16_t brightnessModeYawBasedMovement;
uint16_t brightnessModeRollBasedMovement;
uint16_t* normalBrightnessModes[NumberOfNormalGlobalBrightnessChangeModes] = {&brightnessModeMaxBrightness, &brightnessModeAlmostMaxBrightness, &brightnessModeHalfBrightness};
uint16_t* movementBasedBrightnessModes[NumberOfMovementBasedGlobalBrightnessChangeModes] = {&brightnessModePitchBasedMovement, &brightnessModeYawBasedMovement, &brightnessModeRollBasedMovement};
uint16_t* musicBasedBrightnessModes[NumberOfMovementBasedGlobalBrightnessChangeModes] = {&brightnessModeAudioLevelBasedBrightness, &brightnessModeAudioLevelBasedBrightnessBrighter, &brightnessModeAudioLevelBasedBrightnessInverse};

Color ledColorMap[TOTAL_LEDS];

byte *currentScreenMap[TOTAL_LEDS];

int meteorSize = 100;

EffectSettings effectSettings;
EffectSettings effectSettingsStationary;
EffectSettings effectSettingsWalking;
EffectSettings effectSettingsJogging;
EffectSettings effectSettingsBiking;
EffectSettings effectSettingsDriving;

float effect1AudioLevelThresholdToShowMoreIntenseEffect = .9;
float effect2AudioLevelThresholdToShowMoreIntenseEffect = .9;

int currentTime;
float audioInfluenceFactorForAudioScaledTime = 2;
float currentAudioIntensityLevel;

int transition;
int currentTransitionIncrement;

bool transitionDirection;
int lastPeakDetectorValue;
int mixingMode;
int oldMixingMode;
int mixingModeBlendCounter;
int mixingModeBlendCounterMax = 1;

int switchMapBlendCounter;
int switchMapBlendCounterMax = 1;

bool switchMap = false;

Color getLedColorForFrame(int ledIndex)
{
    return fireworks(frameTimeDelta, effect1Time1, ledIndex, 40, *currentPalette1OffsetPointer, 65535);
    return lightChase(effect1Time1, ledIndex, effect1Time2, *currentPalette1OffsetPointer, 5, 65535);
    //return fireAnimation(frameTimeDelta, ledIndex, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer);
    //return fireAnimation(frameTimeDelta, ledIndex, 80, 250 * currentAudioIntensityLevel, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer);

    byte currentScreen = (*currentScreenMap)[ledIndex];
    if ((switchMap && !(currentScreen & 1)) || (!switchMap && currentScreen & 1))
    {
        frameEffect1 = effect1;
        frameEffect2 = effect2;
        frameEffect3 = effect3;
        frameEffect4 = effect4;
        frameEffect1Plus = effect1Plus;
        frameEffect2Plus = effect2Plus;
        frameEffect3Plus = effect3Plus;
        frameEffect4Plus = effect4Plus;
    }
    else 
    {
        frameEffect1 = effect3;
        frameEffect2 = effect4;
        frameEffect3 = effect1;
        frameEffect4 = effect2;
        frameEffect1Plus = effect3Plus;
        frameEffect2Plus = effect4Plus;
        frameEffect3Plus = effect1Plus;
        frameEffect4Plus = effect2Plus;
    }

    Color color1 = currentAudioIntensityLevel < effect1AudioLevelThresholdToShowMoreIntenseEffect ? frameEffect1(ledIndex) : frameEffect1Plus(ledIndex);
    Color color2 = currentAudioIntensityLevel < effect2AudioLevelThresholdToShowMoreIntenseEffect ? frameEffect2(ledIndex) : frameEffect2Plus(ledIndex);
    Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2);
    if (switchMapBlendCounter)
    {
        Color color3 = currentAudioIntensityLevel < effect1AudioLevelThresholdToShowMoreIntenseEffect ? frameEffect3(ledIndex) : frameEffect4Plus(ledIndex);
        Color color4 = currentAudioIntensityLevel < effect2AudioLevelThresholdToShowMoreIntenseEffect ? frameEffect4(ledIndex) : effect4Plus(ledIndex);
        Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4);
        resultColor1 = blendColorsUsingMixing(resultColor1, resultColor2, (switchMapBlendCounter / switchMapBlendCounterMax) * UINT16_MAX);
    }
    
    if (switchMapBlendCounter) ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps);
    else ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps);
    if (currentScreen & 2) {
        return {ledColorMap[ledIndex].green, ledColorMap[ledIndex].red, ledColorMap[ledIndex].blue};
    }
    return ledColorMap[ledIndex];
}

void incrementEffectFrame()
{
    currentAudioIntensityLevel = getAudioIntensityRatio();
    incrementBrightnessModeLevels(currentAudioIntensityLevel);
    incrementTime();
    incrementTransition();
    incrementColorPalettesTowardsTargets();
    detectBeat();
}

void setupEffects()
{
    currentPalette1OffsetPointer = &currentPalette1Offset;
    currentPalette2OffsetPointer = &currentPalette2Offset;

    effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, UINT16_MAX); };
    effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette1OffsetPointer, UINT16_MAX); };
    effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette2OffsetPointer, UINT16_MAX); };
    effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette2OffsetPointer, UINT16_MAX); };
    effect1Plus = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, UINT16_MAX); };
    effect2Plus = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette1OffsetPointer, UINT16_MAX); };
    effect3Plus = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette2OffsetPointer, UINT16_MAX); };
    effect4Plus = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette2OffsetPointer, UINT16_MAX); };

    *effect1TransformMap1 = normalTransformMapX;
    *effect1TransformMap2 = normalTransformMapY;
    *effect2TransformMap1 = normalTransformMapX;
    *effect2TransformMap2 = normalTransformMapY;
    *effect3TransformMap1 = normalTransformMapX;
    *effect3TransformMap2 = normalTransformMapY;
    *effect4TransformMap1 = normalTransformMapX;
    *effect4TransformMap2 = normalTransformMapY;

    mixingModeBlendFunction = blendColorsUsingMixing;
    oldMixingModeBlendFunction = blendColorsUsingMixing;

    subscribeToStepDetectedEvent(handleStepDetected);
    subscribeToMovementDetectedEvent(handleMovementDetected);

    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + (timeDelta >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)timeDelta / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)timeDelta >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - (timeDelta >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)timeDelta / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)timeDelta >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };

    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 11); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 11); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 11); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 12); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 12); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 12); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentRollPosition() >> 10); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentPitchPosition() >> 10); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return (int)(getCurrentYawPosition() >> 10); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 11)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 11)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 11)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 12)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 12)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 12)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentRollPosition() >> 10)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentPitchPosition() >> 10)); };
    timeModeIncrementFunctions[numberOfTimeModes + numberOfMovementBasedTimeModes++] = [](int currentTime, int timeDelta) { return -((int)(getCurrentYawPosition() >> 10)); };

    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixingGlitched;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixing;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAdd;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingScreen;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAverage;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixingGlitched;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixing;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAdd;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingScreen;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAverage;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingShimmer;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixingGlitched;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixing;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAdd;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingScreen;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAverage;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingShimmer;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingOverlay;

    effect1GlobalBrightnessPointer = normalBrightnessModes[0];
    effect2GlobalBrightnessPointer = normalBrightnessModes[1];
    effect3GlobalBrightnessPointer = normalBrightnessModes[2];
    effect4GlobalBrightnessPointer = normalBrightnessModes[3];

    effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetected = 230;
    effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero = 10;
    effectSettings.LikelihoodEffectsAreRandomizedWhenAntiBeatDetected = 10;
    effectSettings.LikelihoodEffectsAreRandomizedWhenStepIsDetected = 230;
    effectSettings.LikelihoodNothingChangesWhenRandomizingEffect = 50;
    effectSettings.LikelihoodBlendingModeChangesWhenRandomizingEffect = 10;
    effectSettings.LikelihoodSubPaletteChangesWhenRandomizingEffect = 19;
    effectSettings.LikelihoodSubPalettesAreDifferentWhenChangingSubPalettes = 50;
    effectSettings.LikelihoodPaletteChangesWhenRandomizingEffect = 5;
    effectSettings.LikelihoodTwoPalettesAreUsedWhenPaletteChanges = 25;
    effectSettings.LikelihoodBackgroundTransitionTimeChangesWhenRandomizingEffect = 10;
    effectSettings.LikelihoodScreenMapChangesWhenRandomizingEffect = 4;
    effectSettings.LikelihoodEffectsAreSwappedWhenRandomizingEffect = 17;
    effectSettings.LikelihoodTransformMapsAreSwitchedWhenEffectsAreSwapped = 200;
    effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched = 128;
    effectSettings.LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect = 4;
    effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized = 100;
    effectSettings.LikelihoodTimeModesRandomizeWhenRandomizingEffect = 30;
    effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes = 100;
    effectSettings.LikelihoodEffectsAndRandomizedWhenRandomizingEffect = 15;
    effectSettings.LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect = 30;
    effectSettings.LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect = 30;
    effectSettings.LikelihoodIndividualGlobalBrightnessModesChange = 128;
    effectSettings.LikelihoodAllGlobalBrightnessModesAreOfTheSameType = 100;
    effectSettings.LikelihoodTransitionDirectionChangesWhenRandomizingEffect = 15;
    effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked = 200;
    effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked = 100;
    effectSettings.LikelihoodMovementBasedTimeModeIsPicked = 200;
    effectSettings.MillisecondToMoveToNextPaletteFrame = 50;

    effectSettings.MillisecondsForEffectTransitionsMinimum = 500;
    effectSettings.MillisecondsForEffectTransitionsMaximum = 2000;
    
    effectSettings.MillisecondsForBlendingModeTransitionsMinimum = 100;
    effectSettings.MillisecondsForBlendingModeTransitionsMaximum = 2000;

    effectSettings.MillisecondsForBackgroundTransitionFadeMinimum = 2000;
    effectSettings.MillisecondsForBackgroundTransitionFadeMaximum = 10000;

    effectSettings.GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps = 10000;
    effectSettings.GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps = 30000;

    effectSettings.LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect = 9;
    effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMinimum = 32767;
    effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMaximum = 64224;

    effectSettings.LikelihoodWingsAreMirroredWhenTransformMapsAreRandomized = 128;

    effectSettings.AllowedEffects[0] = 0;
    effectSettings.AllowedEffects[1] = 1;
    effectSettings.AllowedEffects[2] = 2;
    effectSettings.AllowedEffects[3] = 3;
    effectSettings.AllowedEffects[4] = 4;
    effectSettings.AllowedEffects[5] = 5;
    effectSettings.AllowedEffects[6] = 6;
    effectSettings.AllowedEffects[7] = 7;
    effectSettings.AllowedEffects[8] = 8;
    effectSettings.AllowedEffects[9] = 9;
    effectSettings.AllowedEffects[10] = 9;
    effectSettings.AllowedEffects[11] = 9;
    effectSettings.AllowedEffects[12] = 12;
    effectSettings.AllowedEffects[13] = 13;
    effectSettings.AllowedEffects[14] = 14;
    effectSettings.AllowedEffects[15] = 15;
    effectSettings.AllowedEffects[16] = 16;
    effectSettings.AllowedEffects[17] = 17;
    effectSettings.AllowedEffects[18] = 18;
    effectSettings.AllowedEffects[19] = 19;

    for (int i = 0; i < NumberOfAllowedPalettesToPickBetween; i++)
    {
        effectSettings.AllowedPalettes[i][0] = NumberOfAllowedPalettesToPickBetween;
        for (int j = 0; j < NumberOfAllowedPalettesToPickBetween; j++)
        {
            effectSettings.AllowedPalettes[i][j + 1] = j;
        }
    }

    effectSettings.MinimumEffectSize = 10;
    effectSettings.MaximumEffectSize = 200;

    effectSettingsStationary = effectSettings;
    effectSettingsWalking = effectSettings;
    effectSettingsJogging = effectSettings;
    effectSettingsBiking = effectSettings;
    effectSettingsDriving = effectSettings;

    for (int i = 0; i < 150; i++) randomizeEffectsNaturally();

    pickRandomAudioLevelThresholdForMoreIntenseEffect();

    *currentScreenMap = infinityScreenMap;
}

inline Color blendIncorporatingOldMixingMode(Color color1, Color color2)
{
    Color newColor = mixingModeBlendFunction(color1, color2, transition);
    if (oldMixingMode == mixingMode || mixingModeBlendCounter == 0) return newColor;
    Color oldColor = oldMixingModeBlendFunction(color1, color2, transition);
    return blendColorsUsingMixing(newColor, oldColor, (mixingModeBlendCounter / mixingModeBlendCounterMax) * UINT16_MAX);
}

void handleStepDetected()
{
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenStepIsDetected) randomizeEffectsNaturally();
    boostRainChance();
    Serial.println("Step detected!");
}

void handleMovementDetected()
{
    MovementType movementType = getCurrentMovementType();
    if (movementType == Stationary) effectSettings = effectSettingsStationary;
    if (movementType == Walking) effectSettings = effectSettingsWalking;
    if (movementType == Jogging) effectSettings = effectSettingsJogging;
    if (movementType == Biking) effectSettings = effectSettingsBiking;
    if (movementType == Driving) effectSettings = effectSettingsDriving;
    if (movementType != Stationary)
    {
        effect1TimeMode1 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
        effect2TimeMode1 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
        effect3TimeMode1 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
        effect4TimeMode1 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
        effect1TimeMode2 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
        effect2TimeMode2 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
        effect3TimeMode2 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
        effect4TimeMode2 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
    }
    else 
    {
        effect1TimeMode1 = fastRandomInteger(numberOfTimeModes);
        effect2TimeMode1 = fastRandomInteger(numberOfTimeModes);
        effect3TimeMode1 = fastRandomInteger(numberOfTimeModes);
        effect4TimeMode1 = fastRandomInteger(numberOfTimeModes);
        effect1TimeMode2 = fastRandomInteger(numberOfTimeModes);
        effect2TimeMode2 = fastRandomInteger(numberOfTimeModes);
        effect3TimeMode2 = fastRandomInteger(numberOfTimeModes);
        effect4TimeMode2 = fastRandomInteger(numberOfTimeModes);
    }
}

void incrementBrightnessModeLevels(float currentAudioIntensityLevel)
{
    brightnessModeAudioLevelBasedBrightness = currentAudioIntensityLevel * UINT16_MAX;
    brightnessModeAudioLevelBasedBrightnessBrighter = 32767 + 32767 * currentAudioIntensityLevel;
    brightnessModeAudioLevelBasedBrightnessInverse = 60000 - (currentAudioIntensityLevel * 60000);
    brightnessModePitchBasedMovement = (UINT16_MAX / 256) * ((getCurrentPitchPosition() >> 19) & 0xff);
    brightnessModeRollBasedMovement = (UINT16_MAX / 256) * ((getCurrentRollPosition() >> 19) & 0xff);
    brightnessModeYawBasedMovement = (UINT16_MAX / 256) * ((getCurrentYawPosition() >> 19) & 0xff);
}

inline void incrementTime()
{
    const float TimeDeltaResolutionIncreaseFactor = 64.0;
    int newTime = (int)getTime();
    frameTimeDelta = (newTime - currentTime);
    int timeDelta = frameTimeDelta * TimeDeltaResolutionIncreaseFactor;
    currentTime = newTime;
    effect1Time1 = timeModeIncrementFunctions[effect1TimeMode1](effect1Time1, timeDelta) / TimeDeltaResolutionIncreaseFactor;
    effect2Time1 = timeModeIncrementFunctions[effect2TimeMode1](effect2Time1, timeDelta) / TimeDeltaResolutionIncreaseFactor;
    effect3Time1 = timeModeIncrementFunctions[effect3TimeMode1](effect3Time1, timeDelta) / TimeDeltaResolutionIncreaseFactor;
    effect4Time1 = timeModeIncrementFunctions[effect4TimeMode1](effect4Time1, timeDelta) / TimeDeltaResolutionIncreaseFactor;
    effect1Time2 = timeModeIncrementFunctions[effect1TimeMode2](effect1Time2, timeDelta) / TimeDeltaResolutionIncreaseFactor;
    effect2Time2 = timeModeIncrementFunctions[effect2TimeMode2](effect2Time2, timeDelta) / TimeDeltaResolutionIncreaseFactor;
    effect3Time2 = timeModeIncrementFunctions[effect3TimeMode2](effect3Time2, timeDelta) / TimeDeltaResolutionIncreaseFactor;
    effect4Time2 = timeModeIncrementFunctions[effect4TimeMode2](effect4Time2, timeDelta) / TimeDeltaResolutionIncreaseFactor;
}

inline void incrementTransition()
{
    currentTransitionIncrement = UINT16_MAX / fastRandomInteger(effectSettings.MillisecondsForBlendingModeTransitionsMinimum, effectSettings.MillisecondsForBlendingModeTransitionsMaximum);
    int incrementAmount = currentTransitionIncrement * frameTimeDelta;
    if (transitionDirection && UINT16_MAX - transition > incrementAmount) transition += incrementAmount;
    else if (transition > incrementAmount) transition -= incrementAmount;

    if (mixingModeBlendCounter) 
    {
        if (mixingModeBlendCounter - frameTimeDelta > 0) mixingModeBlendCounter -= frameTimeDelta;
        else mixingModeBlendCounter = 0;
    }
    else if (oldMixingMode != mixingMode) 
    {
        oldMixingMode = mixingMode;
        oldMixingModeBlendFunction = mixingModeBlendFunction;
    }

    if (switchMapBlendCounter - frameTimeDelta > 0) switchMapBlendCounter -= frameTimeDelta;
    else switchMapBlendCounter = 0;
}

void incrementColorPalettesTowardsTargets()
{
    static int lastMoveColorPalettesTowardsTargetsTime;
    if (currentTime - lastMoveColorPalettesTowardsTargetsTime < effectSettings.MillisecondToMoveToNextPaletteFrame) return;
    lastMoveColorPalettesTowardsTargetsTime = currentTime;
    if (currentPalette1Offset > currentPalette1OffsetTarget) currentPalette1Offset -= PALETTE_LENGTH;
    else if (currentPalette1Offset < currentPalette1OffsetTarget) currentPalette1Offset += PALETTE_LENGTH;
    if (currentPalette2Offset > currentPalette2OffsetTarget) currentPalette2Offset -= PALETTE_LENGTH;
    else if (currentPalette2Offset < currentPalette2OffsetTarget) currentPalette2Offset += PALETTE_LENGTH;
}

void detectBeat()
{
    const int PositivePeak = 1;
    int nextPeak = getCurrentPeakDetectorValue();
    if (nextPeak == lastPeakDetectorValue) return;
    lastPeakDetectorValue = nextPeak;
    if (nextPeak == PositivePeak && fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetected) randomizeEffectsNaturally();
    else if (!nextPeak && fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero) randomizeEffectsNaturally();
    else if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenAntiBeatDetected) randomizeEffectsNaturally();
}

void randomizeEffectsNaturally()
{
    if (fastRandomByte() < effectSettings.LikelihoodNothingChangesWhenRandomizingEffect) return;
    if (fastRandomByte() < effectSettings.LikelihoodBlendingModeChangesWhenRandomizingEffect) pickRandomBlendingMode();
    if (fastRandomByte() < effectSettings.LikelihoodSubPaletteChangesWhenRandomizingEffect) pickRandomSubPalette();
    if (fastRandomByte() < effectSettings.LikelihoodPaletteChangesWhenRandomizingEffect) pickRandomPalette();
    if (fastRandomByte() < effectSettings.LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect) pickRandomTransformMaps();
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreSwappedWhenRandomizingEffect) swapEffects();
    if (fastRandomByte() < effectSettings.LikelihoodTimeModesRandomizeWhenRandomizingEffect) pickRandomTimeModes();
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAndRandomizedWhenRandomizingEffect) pickRandomEffects();
    if (fastRandomByte() < effectSettings.LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect) pickRandomSizeParametersForEffects();
    if (fastRandomByte() < effectSettings.LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect) pickRandomGlobalBrightnessControlModes();
    if (fastRandomByte() < effectSettings.LikelihoodTransitionDirectionChangesWhenRandomizingEffect) switchTransitionDirection();
    if (fastRandomByte() < effectSettings.LikelihoodBackgroundTransitionTimeChangesWhenRandomizingEffect) pickRandomTransitionTime();
    if (fastRandomByte() < effectSettings.LikelihoodAudioLevelThresholdsForMoreIntenseEffectChangeWhenRandomizingEffect) pickRandomAudioLevelThresholdForMoreIntenseEffect();
    if (fastRandomByte() < effectSettings.LikelihoodScreenMapChangesWhenRandomizingEffect) pickRandomScreenMap();
}

void pickRandomScreenMap()
{
    *currentScreenMap = screenMaps[fastRandomInteger(screenMapsCount)];
}

void pickRandomBlendingMode()
{
    mixingModeBlendFunction = mixingModeBlendFunctions[fastRandomInteger(numberOfMixingModeBlendFunctions)];
    mixingModeBlendCounterMax = fastRandomInteger(effectSettings.MillisecondsForBlendingModeTransitionsMinimum, effectSettings.MillisecondsForBlendingModeTransitionsMaximum);
    mixingModeBlendCounter = mixingModeBlendCounterMax;
}

void pickRandomSubPalette()
{
    currentPalette1OffsetTarget = pickRandomSubPaletteFromPalette(currentPalette1);
    currentPalette2OffsetTarget = currentPalette1OffsetTarget;
    if (fastRandomByte() < effectSettings.LikelihoodSubPalettesAreDifferentWhenChangingSubPalettes) currentPalette2OffsetTarget = pickRandomSubPaletteFromPalette(currentPalette2);
    else currentPalette2OffsetTarget = currentPalette1OffsetTarget;
}

void pickRandomPalette()
{
    byte numberOfPossiblePalettesToSwitchTo = effectSettings.AllowedPalettes[currentPalette1][0];
    byte indexOfNextPalette = 1 + fastRandomInteger(numberOfPossiblePalettesToSwitchTo);
    byte nextPalette = effectSettings.AllowedPalettes[currentPalette1][indexOfNextPalette];
    currentPalette1 = nextPalette;
    if (fastRandomByte() < effectSettings.LikelihoodTwoPalettesAreUsedWhenPaletteChanges) 
    {
        byte numberOfPossiblePalettesToSwitchTo = effectSettings.AllowedPalettes[currentPalette2][0];
        byte indexOfNextPalette = 1 + fastRandomInteger(numberOfPossiblePalettesToSwitchTo);
        byte nextPalette = effectSettings.AllowedPalettes[currentPalette2][indexOfNextPalette];
        currentPalette2 = nextPalette;
    }
    else currentPalette2 = currentPalette1;
    currentPalette1OffsetTarget = pickRandomSubPaletteFromPalette(currentPalette1);
    currentPalette2OffsetTarget = pickRandomSubPaletteFromPalette(currentPalette2);
    //currentPalette1Offset = currentPalette1OffsetTarget;
    //currentPalette2Offset = currentPalette2OffsetTarget;
}

int pickRandomSubPaletteFromPalette(int palette)
{
    int subPaletteOffset = fastRandomInteger(paletteHeights[palette]) * PALETTE_LENGTH;
    return paletteStarts[palette] + subPaletteOffset;
}

void pickRandomTransformMaps()
{
    // if (fastRandomByte() < effectSettings.LikelihoodWingsAreMirroredWhenTransformMapsAreRandomized)
    // {
    //     if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect1TransformMap1 = mirroredTransformMaps[fastRandomInteger(mirroredTransformMapsCount)];
    //     if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect1TransformMap2 = mirroredTransformMaps[fastRandomInteger(mirroredTransformMapsCount)];
    //     if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect2TransformMap1 = mirroredTransformMaps[fastRandomInteger(mirroredTransformMapsCount)];
    //     if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect2TransformMap2 = mirroredTransformMaps[fastRandomInteger(mirroredTransformMapsCount)];
    //     if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect3TransformMap1 = mirroredTransformMaps[fastRandomInteger(mirroredTransformMapsCount)];
    //     if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect3TransformMap2 = mirroredTransformMaps[fastRandomInteger(mirroredTransformMapsCount)];
    //     if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect4TransformMap1 = mirroredTransformMaps[fastRandomInteger(mirroredTransformMapsCount)];
    //     if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect4TransformMap2 = mirroredTransformMaps[fastRandomInteger(mirroredTransformMapsCount)];
    // }
    // else 
    // {
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect1TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect1TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect2TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect2TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect3TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect3TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect4TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect4TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
    // }
}

void swapEffects()
{
    switchMap = !switchMap;
    if (switchMap)
    {
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched) *effect1TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched) *effect1TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched) *effect2TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched) *effect2TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
    }
    else
    {
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched) *effect3TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched) *effect3TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched) *effect4TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
        if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched) *effect4TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
    }
    switchMapBlendCounterMax = fastRandomInteger(effectSettings.MillisecondsForEffectTransitionsMinimum, effectSettings.MillisecondsForEffectTransitionsMaximum);
    switchMapBlendCounter = switchMapBlendCounterMax;
}

void pickRandomTimeModes()
{
    int startIndex = 0;
    int length = numberOfTimeModes;
    if ((fastRandomByte() < effectSettings.LikelihoodMovementBasedTimeModeIsPicked) && (getCurrentMovementType() != Stationary))
    {
        startIndex = numberOfTimeModes;
        length = numberOfMovementBasedTimeModes;
    }
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect1TimeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect2TimeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect3TimeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect4TimeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect1TimeMode2 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect2TimeMode2 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect3TimeMode2 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect4TimeMode2 = fastRandomInteger(length) + startIndex;
}

void pickRandomEffects()
{
    int effectRandom = fastRandomInteger(NumberOfAllowedEffectsToPickBetween);
    byte effectSelection = effectSettings.AllowedEffects[effectRandom];
    if (fastRandomInteger(2) == 0)
    {
        if (effectSelection == 0) effect1 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect1Time1, effect1Time2, effect1Time2 >> 3, ledIndex, meteorSize, .12, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effectSelection == 1) effect1 = [](int ledIndex) { return meteorRain(frameTimeDelta, effect1Time1,effect1Time2, ledIndex, meteorSize, .12, *effect1TransformMap2, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effectSelection == 2) effect1 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect1Time1, effect1Time2, 0, ledIndex, meteorSize, .12, *effect1TransformMap1, *effect1TransformMap1, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effectSelection == 3) effect1 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effectSelection == 4) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap2, *effect1TransformMap1, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effectSelection == 5) effect1 = [](int ledIndex) { return rainShower(frameTimeDelta, effect1Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effectSelection == 6) effect1 = [](int ledIndex) { return colorOrb(frameTimeDelta, ledIndex, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effectSelection == 7) effect1 = [](int ledIndex) { return lightningBug(frameTimeDelta, ledIndex, *currentPalette1OffsetPointer); };
        else if (effectSelection == 8) effect1 = [](int ledIndex) { return fireAnimation(frameTimeDelta, ledIndex, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effectSelection == 9) effect1 = [](int ledIndex) { return simpleColor(effect1Time1, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        if (effectSelection == 0) effect3 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect3Time1, effect3Time2, effect3Time2 >> 3, ledIndex, meteorSize, .12, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effectSelection == 1) effect3 = [](int ledIndex) { return meteorRain(frameTimeDelta, effect3Time1,effect3Time2, ledIndex, meteorSize, .12, *effect3TransformMap2, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effectSelection == 2) effect3 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect3Time1, effect3Time2, 0, ledIndex, meteorSize, .12, *effect3TransformMap1, *effect3TransformMap1, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effectSelection == 3) effect3 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effectSelection == 4) effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap2, *effect3TransformMap1, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effectSelection == 5) effect3 = [](int ledIndex) { return rainShower(frameTimeDelta, effect3Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effectSelection == 6) effect3 = [](int ledIndex) { return colorOrb(frameTimeDelta, ledIndex, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effectSelection == 7) effect3 = [](int ledIndex) { return lightningBug(frameTimeDelta, ledIndex, *currentPalette1OffsetPointer); };
        else if (effectSelection == 8) effect3 = [](int ledIndex) { return fireAnimation(frameTimeDelta, ledIndex, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel, normalTransformMapX, normalTransformMapY, *currentPalette2OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effectSelection == 9) effect3 = [](int ledIndex) { return simpleColor(effect3Time1, *currentPalette2OffsetPointer, *effect3GlobalBrightnessPointer); };
        else effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
    }
    else
    {
        if (effectSelection == 0) effect2 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect2Time1, effect2Time2, effect2Time2 >> 3, ledIndex, meteorSize, .12, normalTransformMapX, normalTransformMapY, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effectSelection == 1) effect2 = [](int ledIndex) { return meteorRain(frameTimeDelta, effect2Time1,effect2Time2, ledIndex, meteorSize, .12, *effect2TransformMap2, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effectSelection == 2) effect2 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect2Time1, effect2Time2, 0, ledIndex, meteorSize, .12, *effect2TransformMap1, *effect2TransformMap1, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effectSelection == 3) effect2 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effectSelection == 4) effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap2, *effect2TransformMap1, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effectSelection == 5) effect2 = [](int ledIndex) { return rainShower(frameTimeDelta, effect2Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effectSelection == 6) effect2 = [](int ledIndex) { return colorOrb(frameTimeDelta, ledIndex, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effectSelection == 7) effect2 = [](int ledIndex) { return lightningBug(frameTimeDelta, ledIndex, *currentPalette2OffsetPointer); };
        else if (effectSelection == 8) effect2 = [](int ledIndex) { return fireAnimation(frameTimeDelta, ledIndex, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effectSelection == 9) effect2 = [](int ledIndex) { return simpleColor(effect2Time1, *currentPalette1OffsetPointer, *effect2GlobalBrightnessPointer); };
        else effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        if (effectSelection == 0) effect4 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect4Time1, effect4Time2, effect4Time2 >> 3, ledIndex, meteorSize, .12, normalTransformMapX, normalTransformMapY, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effectSelection == 1) effect4 = [](int ledIndex) { return meteorRain(frameTimeDelta, effect4Time1,effect4Time2, ledIndex, meteorSize, .12, *effect4TransformMap2, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effectSelection == 2) effect4 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect4Time1, effect4Time2, 0, ledIndex, meteorSize, .12, *effect4TransformMap1, *effect4TransformMap1, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effectSelection == 3) effect4 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effectSelection == 4) effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap2, *effect4TransformMap1, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effectSelection == 5) effect4 = [](int ledIndex) { return rainShower(frameTimeDelta, effect4Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effectSelection == 6) effect4 = [](int ledIndex) { return colorOrb(frameTimeDelta, ledIndex, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effectSelection == 7) effect4 = [](int ledIndex) { return lightningBug(frameTimeDelta, ledIndex, *currentPalette2OffsetPointer); };
        else if (effectSelection == 8) effect4 = [](int ledIndex) { return fireAnimation(frameTimeDelta, ledIndex, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel, normalTransformMapX, normalTransformMapY, *currentPalette2OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effectSelection == 9) effect4 = [](int ledIndex) { return simpleColor(effect4Time1, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
    }
}

void pickRandomSizeParametersForEffects()
{
    meteorSize = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
}

void pickRandomGlobalBrightnessControlModes()
{
    if (fastRandomByte() < effectSettings.LikelihoodAllGlobalBrightnessModesAreOfTheSameType) pickRandomGlobalBrightnessControlModesOfTheSameType();
    else pickRandomGlobalBrightnessControlModesOfDifferentTypes();
}

void pickRandomGlobalBrightnessControlModesOfTheSameType()
{
    if ((fastRandomByte() < effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked) && (getCurrentMovementType() != Stationary))
    {
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect1GlobalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect2GlobalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect3GlobalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect4GlobalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
    }
    else if (fastRandomByte() < effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked)
    {
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect1GlobalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect2GlobalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect3GlobalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect4GlobalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
    }
    else 
    {
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect1GlobalBrightnessPointer = normalBrightnessModes[fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes)];
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect2GlobalBrightnessPointer = normalBrightnessModes[fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes)];
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect2GlobalBrightnessPointer = normalBrightnessModes[fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes)];
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect4GlobalBrightnessPointer = normalBrightnessModes[fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes)];
    }
}

void pickRandomGlobalBrightnessControlModesOfDifferentTypes()
{
    if ((fastRandomByte() < effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked) && (getCurrentMovementType() != Stationary))
    {
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect1GlobalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
    }
    else if (fastRandomByte() < effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked)
    {
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect1GlobalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
    }
    else if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect1GlobalBrightnessPointer = normalBrightnessModes[fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes)];
    if ((fastRandomByte() < effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked) && (getCurrentMovementType() != Stationary))
    {
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect2GlobalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
    }
    else if (fastRandomByte() < effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked)
    {
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect2GlobalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
    }
    else if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect2GlobalBrightnessPointer = normalBrightnessModes[fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes)];
    if ((fastRandomByte() < effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked) && (getCurrentMovementType() != Stationary))
    {
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect3GlobalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
    }
    else if (fastRandomByte() < effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked)
    {
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect3GlobalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
    }
    else if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect2GlobalBrightnessPointer = normalBrightnessModes[fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes)];
    if ((fastRandomByte() < effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked) && (getCurrentMovementType() != Stationary))
    {
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect4GlobalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
    }
    else if (fastRandomByte() < effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked)
    {
        if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect4GlobalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
    }
    else if (fastRandomByte() < effectSettings.LikelihoodIndividualGlobalBrightnessModesChange) effect4GlobalBrightnessPointer = normalBrightnessModes[fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes)];
}

void pickRandomTransitionTime()
{
    currentTransitionIncrement = UINT16_MAX / fastRandomInteger(effectSettings.MillisecondsForBlendingModeTransitionsMinimum, effectSettings.MillisecondsForBlendingModeTransitionsMaximum);
}

void switchTransitionDirection()
{
    transitionDirection = !transitionDirection;
}

void pickRandomAudioLevelThresholdForMoreIntenseEffect()
{
    effect1AudioLevelThresholdToShowMoreIntenseEffect = (float)fastRandomInteger(effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMinimum, effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMaximum) / (float)UINT16_MAX;
    effect2AudioLevelThresholdToShowMoreIntenseEffect = (float)fastRandomInteger(effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMinimum, effectSettings.AudioLevelThresholdToShowMoreIntenseEffectMaximum) / (float)UINT16_MAX;
}