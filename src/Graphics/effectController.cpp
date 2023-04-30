#include <Adafruit_NeoPXL8.h>
#include "../settings.h"
#include "../Peripherals/microphone.h"
#include "../Peripherals/movementDetection.h"
#include "../Graphics/graphics.h"
#include "../Graphics/effectController.h"
#include "../Graphics/transformMaps.h"
#include "../Graphics/screenMaps.h"
#include "../Graphics/effectSettings.h"
#include "../Utility/time.h"
#include "../IO/analogInput.h"
#include "../Observers/stepDectectedObserver.h"
#include "../Observers/movementDetectedObserver.h"
#include "../Utility/fastRandom.h"

#include "palettes.h"
//#include "palettesMinimal.h"

#define MIN_CROSS_FADE_DURATION 100.0
#define MAX_CROSS_FADE_DURATION 2000.0
#define STEP_RESET_TIME_MAX 1500

int currentTime;
float audioInfluenceFactorForAudioScaledTime = 2;
float currentAudioIntensityLevel;

int transition;
bool transitionDirection;
int lastPeakDetectorValue;
int mixingMode;
int oldMixingMode;
int mixingModeBlendCounter;
float mixingModeBlendCounterMax = MAX_CROSS_FADE_DURATION;

int switchMapBlendCounter;
float switchMapBlendCounterMax = MAX_CROSS_FADE_DURATION;

bool switchMap = false;

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
void pickRandomTimeModes();
void pickRandomEffects();
void pickRandomSizeParametersForEffects();
void pickRandomGlobalBrightnessControlModes();
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
int (*timeModeIncrementFunctions[MaxNumberOfTimeModes])(int timeDelta);

const byte NumberOfGlobalBrightnessChangeModes = 4;
uint16_t brightnessModeMaxBrightness = 65535;
uint16_t brightnessModeAudioLevelBasedBrightness = 65535;
uint16_t brightnessModeAudioLevelBasedBrightnessBrighter = 65535;
uint16_t brightnessModeAudioLevelBasedBrightnessInverse = 65535;
uint16_t* brightnessModes[NumberOfGlobalBrightnessChangeModes] = {&brightnessModeMaxBrightness, &brightnessModeAudioLevelBasedBrightness, &brightnessModeAudioLevelBasedBrightnessBrighter, &brightnessModeAudioLevelBasedBrightnessInverse};

Color ledColorMap[TOTAL_LEDS];

byte *currentScreenMap[TOTAL_LEDS];

int meteorSize = 100;

EffectSettings effectSettings;

Color getLedColorForFrame(int ledIndex)
{
    byte currentScreen = (*currentScreenMap)[ledIndex];
    if (switchMap)
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

    Color color1 = currentAudioIntensityLevel < .98 ? frameEffect1(ledIndex) : frameEffect1Plus(ledIndex);
    Color color2 = currentAudioIntensityLevel < .6 ? frameEffect2(ledIndex) : frameEffect2Plus(ledIndex);
    Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2);
    if (switchMapBlendCounter)
    {
        Color color3 = currentAudioIntensityLevel < .98 ? frameEffect3(ledIndex) : frameEffect4Plus(ledIndex);
        Color color4 = currentAudioIntensityLevel < .6 ? frameEffect4(ledIndex) : effect4Plus(ledIndex);
        Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4);
        resultColor1 = blendColorsUsingMixing(resultColor1, resultColor2, (switchMapBlendCounter / switchMapBlendCounterMax) * 65535);
    }
    
    if (switchMapBlendCounter) ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, (.1 * 65535));
    else ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, (.5 * 65535));
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

    effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, 65535); };
    effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette1OffsetPointer, 65535); };
    effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette2OffsetPointer, 65535); };
    effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette2OffsetPointer, 65535); };
    effect1Plus = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, 65535); };
    effect2Plus = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette1OffsetPointer, 65535); };
    effect3Plus = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette2OffsetPointer, 65535); };
    effect4Plus = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette2OffsetPointer, 65535); };

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

    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return timeDelta >> 1; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)timeDelta / 3; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)timeDelta >> 2; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 1; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) / 3; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 2; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -timeDelta >> 1; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)timeDelta / 3; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)timeDelta >> 2; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 1; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) / 3; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 2; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4))); };

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

    effect1GlobalBrightnessPointer = brightnessModes[0];
    effect2GlobalBrightnessPointer = brightnessModes[1];
    effect3GlobalBrightnessPointer = brightnessModes[2];
    effect4GlobalBrightnessPointer = brightnessModes[3];

    effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetected = 50;
    effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero = 10;
    effectSettings.LikelihoodEffectsAreRandomizedWhenAntiBeatDetected = 255;
    effectSettings.LikelihoodEffectsAreRandomizedWhenStepIsDetected = 255;
    effectSettings.LikelihoodNothingChangesWhenRandomizingEffect = 40;
    effectSettings.LikelihoodBlendingModeChangesWhenRandomizingEffect = 20;
    effectSettings.LikelihoodSubPaletteChangesWhenRandomizingEffect = 20;
    effectSettings.LikelihoodSubPalettesAreDifferentWhenChangingSubPalettes = 128;
    effectSettings.LikelihoodPaletteChangesWhenRandomizingEffect = 5;
    effectSettings.LikelihoodTwoPalettesAreUsedWhenPaletteChanges = 25;
    effectSettings.LikelihoodEffectsAreSwappedWhenRandomizingEffect = 20;
    effectSettings.LikelihoodTransformMapsAreSwitchedWhenEffectsAreSwapped = 255;
    effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched = 175;
    effectSettings.LikelihoodTransformMapsAreRandomizedWithoutFadeWhenRandomizingEffect = 5;
    effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized = 100;
    effectSettings.LikelihoodTimeModesRandomizeWhenRandomizingEffect = 50;
    effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes = 100;
    effectSettings.LikelihoodEffectsAndRandomizedWhenRandomizingEffect = 15;
    effectSettings.LikelihoodSizeParameterForEffectsChangesWhenWhenRandomizingEffect = 30;
    effectSettings.LikelihoodGlobalBrightnessModesChangeWhenRandomizingEffect = 30;
    effectSettings.LikelihoodIndividualGlobalBrightnessModesChange = 80;
    effectSettings.LikelihoodTransitionDirectionChangesWhenRandomizingEffect = 4;

    for (int i = 0; i < 150; i++) randomizeEffectsNaturally();
}

inline Color blendIncorporatingOldMixingMode(Color color1, Color color2)
{
    Color newColor = mixingModeBlendFunction(color1, color2, transition);
    if (oldMixingMode == mixingMode || mixingModeBlendCounter == 0) return newColor;
    Color oldColor = oldMixingModeBlendFunction(color1, color2, transition);
    return blendColorsUsingMixing(newColor, oldColor, (mixingModeBlendCounter / mixingModeBlendCounterMax) * 65535);
}

void handleStepDetected()
{
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenStepIsDetected) randomizeEffectsNaturally();
    Serial.println("Step detected!");
}

void handleMovementDetected()
{
    Serial.println("Movement detected!");
    Serial.println(getCurrentMovementType());
}

void incrementBrightnessModeLevels(float currentAudioIntensityLevel)
{
    brightnessModeAudioLevelBasedBrightness = currentAudioIntensityLevel * 65535;
    brightnessModeAudioLevelBasedBrightnessBrighter = 32767 + 32767 * currentAudioIntensityLevel;
    brightnessModeAudioLevelBasedBrightnessInverse = 60000 - (currentAudioIntensityLevel * 60000);
}

inline void incrementTime()
{
    int newTime = (int)getTime();
    frameTimeDelta = (newTime - currentTime);
    int timeDelta = frameTimeDelta * 64;
    currentTime = newTime;
    effect1Time1 += timeModeIncrementFunctions[effect1TimeMode1](timeDelta) / 64;
    effect2Time1 += timeModeIncrementFunctions[effect2TimeMode1](timeDelta) / 64;
    effect3Time1 += timeModeIncrementFunctions[effect3TimeMode1](timeDelta) / 64;
    effect4Time1 += timeModeIncrementFunctions[effect4TimeMode1](timeDelta) / 64;
    effect1Time2 += timeModeIncrementFunctions[effect1TimeMode2](timeDelta) / 64;
    effect2Time2 += timeModeIncrementFunctions[effect2TimeMode2](timeDelta) / 64;
    effect3Time2 += timeModeIncrementFunctions[effect3TimeMode2](timeDelta) / 64;
    effect4Time2 += timeModeIncrementFunctions[effect4TimeMode2](timeDelta) / 64;
}

inline void incrementTransition()
{
    int incrementAmount = frameTimeDelta * 7;
    if (transitionDirection && 65535 - transition > incrementAmount) transition += incrementAmount;
    else if (transition > incrementAmount) transition -= incrementAmount;

    if (mixingModeBlendCounter) 
    {
        if (mixingModeBlendCounter - frameTimeDelta > 0) mixingModeBlendCounter -= frameTimeDelta;
        else mixingModeBlendCounter = 0;
    }
    else if (oldMixingMode != mixingMode) {
        oldMixingMode = mixingMode;
        oldMixingModeBlendFunction = mixingModeBlendFunction;
    }

    if (switchMapBlendCounter - frameTimeDelta > 0) switchMapBlendCounter -= frameTimeDelta;
    else switchMapBlendCounter = 0;
}

uint32_t lastMoveColorPalettesTowardsTargetsTime;
uint32_t moveColorPalettesTowardsTargetsTimeThreshold = 80;
void incrementColorPalettesTowardsTargets()
{
    if (currentTime - lastMoveColorPalettesTowardsTargetsTime < moveColorPalettesTowardsTargetsTimeThreshold) return;
    lastMoveColorPalettesTowardsTargetsTime = currentTime;
    if (currentPalette1Offset > currentPalette1OffsetTarget) currentPalette1Offset -= PALETTE_LENGTH;
    else if (currentPalette1Offset < currentPalette1OffsetTarget) currentPalette1Offset += PALETTE_LENGTH;
    if (currentPalette2Offset > currentPalette2OffsetTarget) currentPalette2Offset -= PALETTE_LENGTH;
    else if (currentPalette2Offset < currentPalette2OffsetTarget) currentPalette2Offset += PALETTE_LENGTH;
}

void detectBeat()
{
    int nextPeak = getCurrentPeakDetectorValue();
    if (nextPeak == lastPeakDetectorValue) return;
    lastPeakDetectorValue = nextPeak;
    if (nextPeak == 1 && fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetected) randomizeEffectsNaturally();
    else if (nextPeak == 0 && fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetectorReturnsToZero) randomizeEffectsNaturally();
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
}

void pickRandomBlendingMode()
{
    mixingModeBlendFunction = mixingModeBlendFunctions[fastRandomInteger(numberOfMixingModeBlendFunctions)];
    mixingModeBlendCounterMax = fastRandomInteger(MAX_CROSS_FADE_DURATION - MIN_CROSS_FADE_DURATION) + MIN_CROSS_FADE_DURATION;
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
    currentPalette1 = fastRandomInteger(TOTAL_NUMBER_OF_PALETTES);
    if (fastRandomByte() < effectSettings.LikelihoodTwoPalettesAreUsedWhenPaletteChanges) currentPalette2 = fastRandomInteger(TOTAL_NUMBER_OF_PALETTES);
    else currentPalette2 = currentPalette1;
    currentPalette1OffsetTarget = pickRandomSubPaletteFromPalette(currentPalette1);
    currentPalette2OffsetTarget = pickRandomSubPaletteFromPalette(currentPalette2);
}

int pickRandomSubPaletteFromPalette(int palette)
{
    int subPaletteOffset = fastRandomInteger(paletteHeights[palette]) * PALETTE_LENGTH;
    return paletteStarts[palette] + subPaletteOffset;
}

void pickRandomTransformMaps()
{
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect1TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect1TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect2TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect2TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect3TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect3TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect4TransformMap1 = transformMaps[fastRandomInteger(transformMapsCount)];
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized) *effect4TransformMap2 = transformMaps[fastRandomInteger(transformMapsCount)];
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
    switchMapBlendCounterMax = fastRandomInteger(MAX_CROSS_FADE_DURATION - MIN_CROSS_FADE_DURATION) + MIN_CROSS_FADE_DURATION;
    switchMapBlendCounter = switchMapBlendCounterMax;
}

void pickRandomTimeModes()
{
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect1TimeMode1 = fastRandomInteger(numberOfTimeModes);
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect2TimeMode1 = fastRandomInteger(numberOfTimeModes);
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect3TimeMode1 = fastRandomInteger(numberOfTimeModes);
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect4TimeMode1 = fastRandomInteger(numberOfTimeModes);
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect1TimeMode2 = fastRandomInteger(numberOfTimeModes);
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect2TimeMode2 = fastRandomInteger(numberOfTimeModes);
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect3TimeMode2 = fastRandomInteger(numberOfTimeModes);
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeTimeChangesWhenTimeModeRandomizes) effect4TimeMode2 = fastRandomInteger(numberOfTimeModes);
}

void pickRandomEffects()
{
    int effect = fastRandomInteger(8);
    if (effect == 1)
    {
        effect = fastRandomInteger(12);
        if (effect == 1) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effect == 1) effect1 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect1Time1, effect1Time2, effect1Time2 >> 3, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effect == 2) effect1 = [](int ledIndex) { return meteorRain(frameTimeDelta, effect1Time1,effect1Time2, ledIndex, meteorSize, .12, *effect1TransformMap2, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effect == 3) effect1 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect1Time1, effect1Time2, 0, ledIndex, 50, .12, *effect1TransformMap1, *effect1TransformMap1, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effect == 4) effect1 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effect == 5) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap2, *effect1TransformMap1, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effect == 6) effect1 = [](int ledIndex) { return rainShower(frameTimeDelta, effect1Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effect == 7) effect1 = [](int ledIndex) { return colorOrb(frameTimeDelta, ledIndex, *currentPalette1OffsetPointer, *effect1GlobalBrightnessPointer); };
        else if (effect == 8) effect1 = [](int ledIndex) { return lightningBug(frameTimeDelta, ledIndex, *currentPalette1OffsetPointer); };
        else if (fastRandomInteger(5 == 0)) effect1 = [](int ledIndex) { return ledTest(currentTime, ledIndex, effect1Time2, *currentPalette1OffsetPointer, 5, *effect1GlobalBrightnessPointer); };
    }
    if (effect == 2)
    {
        effect = fastRandomInteger(12);
        if (effect == 2) effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effect == 1) effect2 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect2Time1, effect2Time2, effect2Time2 >> 3, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effect == 2) effect2 = [](int ledIndex) { return meteorRain(frameTimeDelta, effect2Time1,effect2Time2, ledIndex, meteorSize, .12, *effect2TransformMap2, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effect == 3) effect2 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect2Time1, effect2Time2, 0, ledIndex, 50, .12, *effect2TransformMap1, *effect2TransformMap1, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effect == 4) effect2 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effect == 5) effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap2, *effect2TransformMap1, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effect == 6) effect2 = [](int ledIndex) { return rainShower(frameTimeDelta, effect2Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effect == 7) effect2 = [](int ledIndex) { return colorOrb(frameTimeDelta, ledIndex, *currentPalette2OffsetPointer, *effect2GlobalBrightnessPointer); };
        else if (effect == 8) effect2 = [](int ledIndex) { return lightningBug(frameTimeDelta, ledIndex, *currentPalette2OffsetPointer); };
        else if (fastRandomInteger(5 == 0)) effect2 = [](int ledIndex) { return ledTest(currentTime, ledIndex, effect2Time2, *currentPalette2OffsetPointer, 5, *effect2GlobalBrightnessPointer); };
    }
    if (effect == 3)
    {
        effect = fastRandomInteger(12);
        if (effect == 3) effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effect == 1) effect3 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect3Time1, effect3Time2, effect3Time2 >> 3, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effect == 2) effect3 = [](int ledIndex) { return meteorRain(frameTimeDelta, effect3Time1,effect3Time2, ledIndex, meteorSize, .12, *effect3TransformMap2, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effect == 3) effect3 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect3Time1, effect3Time2, 0, ledIndex, 50, .12, *effect3TransformMap1, *effect3TransformMap1, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effect == 4) effect3 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effect == 5) effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap2, *effect3TransformMap1, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effect == 6) effect3 = [](int ledIndex) { return rainShower(frameTimeDelta, effect3Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effect == 7) effect3 = [](int ledIndex) { return colorOrb(frameTimeDelta, ledIndex, *currentPalette1OffsetPointer, *effect3GlobalBrightnessPointer); };
        else if (effect == 8) effect3 = [](int ledIndex) { return lightningBug(frameTimeDelta, ledIndex, *currentPalette1OffsetPointer); };
        else if (fastRandomInteger(5 == 0)) effect3 = [](int ledIndex) { return ledTest(currentTime, ledIndex, effect3Time2, *currentPalette1OffsetPointer, 5, *effect3GlobalBrightnessPointer); };
    }
    if (effect == 4)
    {
        effect = fastRandomInteger(12);
        if (effect == 4) effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effect == 1) effect4 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect4Time1, effect4Time2, effect4Time2 >> 3, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effect == 2) effect4 = [](int ledIndex) { return meteorRain(frameTimeDelta, effect4Time1,effect4Time2, ledIndex, meteorSize, .12, *effect4TransformMap2, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effect == 3) effect4 = [](int ledIndex) { return meteorRain2(frameTimeDelta, effect4Time1, effect4Time2, 0, ledIndex, 50, .12, *effect4TransformMap1, *effect4TransformMap1, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effect == 4) effect4 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effect == 5) effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap2, *effect4TransformMap1, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effect == 6) effect4 = [](int ledIndex) { return rainShower(frameTimeDelta, effect4Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effect == 7) effect4 = [](int ledIndex) { return colorOrb(frameTimeDelta, ledIndex, *currentPalette2OffsetPointer, *effect4GlobalBrightnessPointer); };
        else if (effect == 8) effect4 = [](int ledIndex) { return lightningBug(frameTimeDelta, ledIndex, *currentPalette2OffsetPointer); };
        else if (fastRandomInteger(5 == 0)) effect4 = [](int ledIndex) { return ledTest(currentTime, ledIndex, effect4Time2, *currentPalette2OffsetPointer, 5, *effect4GlobalBrightnessPointer); };
    }
}

void pickRandomSizeParametersForEffects()
{
    meteorSize = fastRandomInteger(190) + 10;
}

void pickRandomGlobalBrightnessControlModes()
{
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetected) effect1GlobalBrightnessPointer = brightnessModes[fastRandomInteger(4)];
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetected) effect2GlobalBrightnessPointer = brightnessModes[fastRandomInteger(4)];
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetected) effect3GlobalBrightnessPointer = brightnessModes[fastRandomInteger(4)];
    if (fastRandomByte() < effectSettings.LikelihoodEffectsAreRandomizedWhenBeatDetected) effect4GlobalBrightnessPointer = brightnessModes[fastRandomInteger(4)];
}

void switchTransitionDirection()
{
    transitionDirection = !transitionDirection;
}