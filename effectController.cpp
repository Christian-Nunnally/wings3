#include <Adafruit_NeoPXL8.h>
#include "effectController.h"
#include "settings.h"
#include "graphics.h"
#include "microphone.h"
#include "movementDetection.h"
#include "transformMaps.h"
#include "time.h"
#include "analogInput.h"
#include "src/Observers/stepDectectedObserver.h"
#include "src/Observers/movementDetectedObserver.h"

#include "palettes.h"
//#include "palettesMinimal.h"

#define MIN_CROSS_FADE_DURATION 15.0
#define MAX_CROSS_FADE_DURATION 200.0
#define STEP_RESET_TIME_MAX 1500

int currentTime;
float audioInfluenceFactorForAudioScaledTime = 2;
float currentAudioIntensityLevel;

int transition;
int loopCount;
int loopCountDirection = 1;
int lastPeakDetectorValue;
int mixingMode;
int oldMixingMode;
int mixingModeBlendCounter;
float mixingModeBlendCounterMax = MAX_CROSS_FADE_DURATION;

int switchMapBlendCounter;
float switchMapBlendCounterMax = MAX_CROSS_FADE_DURATION;

bool switchMap = false;

void incrementTime();
void incrementTransition();
Color blendIncorporatingOldMixingMode(Color color1, Color color2);
void randomizeBlendingMode();
void handleStepDetected();
void handleMovementDetected();
void pickRandomEffects();
void detectBeat();
void randomizeEffectsNaturally();

Color (*effect1)(int pixel) {};
Color (*effect1Plus)(int pixel) {};
Color (*effect2)(int pixel) {};
Color (*effect2Plus)(int pixel) {};
Color (*effect3)(int pixel) {};
Color (*effect3Plus)(int pixel) {};
Color (*effect4)(int pixel) {};
Color (*effect4Plus)(int pixel) {};

const int MaxNumberOfMixingModeBlendFunctions = 10;
int numberOfMixingModeBlendFunctions;
Color (*mixingModeBlendFunctions[MaxNumberOfMixingModeBlendFunctions])(Color color1, Color color2, uint16_t transitionAmount);
Color (*mixingModeBlendFunction)(Color color1, Color color2, uint16_t transitionAmount) {};
Color (*oldMixingModeBlendFunction)(Color color1, Color color2, uint16_t transitionAmount) {};

uint8_t *effect1TransformMap1[272];
uint8_t *effect1TransformMap2[272];
uint8_t *effect2TransformMap1[272];
uint8_t *effect2TransformMap2[272];
uint8_t *effect3TransformMap1[272];
uint8_t *effect3TransformMap2[272];
uint8_t *effect4TransformMap1[272];
uint8_t *effect4TransformMap2[272];

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
int *currentPalette1OffsetPointer; 
int *currentPalette2OffsetPointer; 

const byte MaxNumberOfTimeModes = 40;
byte numberOfTimeModes;
int (*timeModeIncrementFunctions[MaxNumberOfTimeModes])(int timeDelta);

Color getLedColorForFrame(int ledIndex)
{
    if (switchMap)
    {
        Color color1 = currentAudioIntensityLevel < .98 ? effect1(ledIndex) : effect1Plus(ledIndex);
        Color color2 = currentAudioIntensityLevel < .6 ? effect2(ledIndex) : effect2Plus(ledIndex);
        Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2);
        if (switchMapBlendCounter)
        {
            Color color3 = currentAudioIntensityLevel < .98 ? effect3(ledIndex) : effect4Plus(ledIndex);
            Color color4 = currentAudioIntensityLevel < .6 ? effect4(ledIndex) : effect4Plus(ledIndex);
            Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4);
            return blendColorsUsingMixing(resultColor1, resultColor2, (switchMapBlendCounter / switchMapBlendCounterMax) * 65535);
        }
        return resultColor1;
    }
    else 
    {
        Color color1 = currentAudioIntensityLevel < .98 ? effect3(ledIndex) : effect4Plus(ledIndex);
        Color color2 = currentAudioIntensityLevel < .6 ? effect4(ledIndex) : effect4Plus(ledIndex);
        Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2);
        if (switchMapBlendCounter)
        {
            Color color3 = currentAudioIntensityLevel < .98 ? effect1(ledIndex) : effect1Plus(ledIndex);
            Color color4 = currentAudioIntensityLevel < .6 ? effect2(ledIndex) : effect2Plus(ledIndex);
            Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4);
            return blendColorsUsingMixing(resultColor1, resultColor2, (switchMapBlendCounter / switchMapBlendCounterMax) * 65535);
        }
        return resultColor1;
    }
}

inline Color blendIncorporatingOldMixingMode(Color color1, Color color2)
{
    Color newColor = mixingModeBlendFunction(color1, color2, transition);
    if (oldMixingMode == mixingMode || mixingModeBlendCounter == 0) return newColor;
    Color oldColor = oldMixingModeBlendFunction(color1, color2, transition);
    return blendColorsUsingMixing(newColor, oldColor, (mixingModeBlendCounter / mixingModeBlendCounterMax) * 65535);
}

int pickRandomSubPaletteFromPalette(int palette)
{
    int subPaletteOffset = random(paletteHeights[palette]) * PALETTE_LENGTH;
    return paletteStarts[palette] + subPaletteOffset;
}

void incrementEffectFrame()
{
    incrementTime();
    incrementTransition();
    currentAudioIntensityLevel = getAudioIntensityRatio();
    detectBeat();
}

void detectBeat()
{
    int nextPeak = getCurrentPeakDetectorValue();
    if (nextPeak == lastPeakDetectorValue) return;
    lastPeakDetectorValue = nextPeak;
    if (nextPeak == 1) randomizeEffectsNaturally();
    else if (nextPeak == 0) 
    {
        if (random(20) == 1) randomizeEffectsNaturally();
    }
    else
    {
        if (random(8) == 1) randomizeEffectsNaturally();
    }
}

void randomizeEffectsNaturally()
{
    Serial.println("RANDOMIZING!");
    int currentRandom = random(12);
    if (currentRandom == 0) randomizeBlendingMode();
    if (currentRandom == 1)
    {
        currentPalette1 = random(TOTAL_NUMBER_OF_PALETTES);
        currentPalette2 = currentPalette1;
    }
    if (currentRandom == 2)
    {
        currentPalette1Offset = pickRandomSubPaletteFromPalette(currentPalette1);
        currentPalette2Offset = pickRandomSubPaletteFromPalette(currentPalette1);
    }
    else if (currentRandom == 3)
    {
        currentPalette1Offset = pickRandomSubPaletteFromPalette(currentPalette1);
        currentPalette2Offset = currentPalette1Offset;
    }
    else if (currentRandom == 4)
    {
        switchMap = !switchMap;
        if (switchMap)
        {
            if (random(10) > 5) *effect1TransformMap1 = transformMaps[random(transformMapsCount)];
            if (random(10) > 5) *effect1TransformMap2 = transformMaps[random(transformMapsCount)];
            if (random(10) > 5) *effect2TransformMap1 = transformMaps[random(transformMapsCount)];
            if (random(10) > 5) *effect2TransformMap2 = transformMaps[random(transformMapsCount)];
        }
        else
        {
            if (random(10) > 5) *effect3TransformMap1 = transformMaps[random(transformMapsCount)];
            if (random(10) > 5) *effect3TransformMap2 = transformMaps[random(transformMapsCount)];
            if (random(10) > 5) *effect4TransformMap1 = transformMaps[random(transformMapsCount)];
            if (random(10) > 5) *effect4TransformMap2 = transformMaps[random(transformMapsCount)];
        }
        switchMapBlendCounterMax = random(MAX_CROSS_FADE_DURATION - MIN_CROSS_FADE_DURATION) + MIN_CROSS_FADE_DURATION;
        switchMapBlendCounter = switchMapBlendCounterMax;
    }
    else if (currentRandom == 5)
    {
        if (random(10) > 6) effect1TimeMode1 = random(numberOfTimeModes);
        if (random(10) > 6) effect2TimeMode1 = random(numberOfTimeModes);
        if (random(10) > 6) effect3TimeMode1 = random(numberOfTimeModes);
        if (random(10) > 6) effect4TimeMode1 = random(numberOfTimeModes);
        if (random(10) > 6) effect1TimeMode2 = random(numberOfTimeModes);
        if (random(10) > 6) effect2TimeMode2 = random(numberOfTimeModes);
        if (random(10) > 6) effect3TimeMode2 = random(numberOfTimeModes);
        if (random(10) > 6) effect4TimeMode2 = random(numberOfTimeModes);
    }
    else if (currentRandom == 6)
    {
        pickRandomEffects();
    }

    if (random(5) == 0)
    {
        if (random(10) > 6) effect1TimeMode1 = random(numberOfTimeModes);
        if (random(10) > 6) effect2TimeMode1 = random(numberOfTimeModes);
        if (random(10) > 6) effect3TimeMode1 = random(numberOfTimeModes);
        if (random(10) > 6) effect4TimeMode1 = random(numberOfTimeModes);
        if (random(10) > 6) effect1TimeMode2 = random(numberOfTimeModes);
        if (random(10) > 6) effect2TimeMode2 = random(numberOfTimeModes);
        if (random(10) > 6) effect3TimeMode2 = random(numberOfTimeModes);
        if (random(10) > 6) effect4TimeMode2 = random(numberOfTimeModes);
    }
}

void pickRandomEffects()
{
    int effect = random(8);

    if (effect == 0)
    {
        effect = random(8);
        effect = 1;
        // if (effect == 0) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, currentAudioIntensityLevel * 65535); };
        if (effect == 1) effect1 = [](int ledIndex) { return meteorRain2(effect1Time1, effect1Time2, effect1Time2 >> 3, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 2) effect1 = [](int ledIndex) { return meteorRain(effect1Time1,effect1Time2, ledIndex, 50, .12, *effect1TransformMap2, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 3) effect1 = [](int ledIndex) { return meteorRain2(effect1Time1, effect1Time2, 0, ledIndex, 50, .12, *effect1TransformMap1, *effect1TransformMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 4) effect1 = [](int ledIndex) { return starShimmer(40, 200, ledIndex, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 5) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap2, *effect1TransformMap1, *currentPalette1OffsetPointer, 65535); };
        // else if (effect == 6) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap2, *effect1TransformMap1, *currentPalette1OffsetPointer, 32767 + (32766 * currentAudioIntensityLevel)); };
        // else if (effect == 7) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap2, *effect1TransformMap1, *currentPalette1OffsetPointer, 65535 - (currentAudioIntensityLevel * 65535)); };
    }
    else if (effect == 1)
    {
        effect = random(8);
        // if (effect == 0) effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette1OffsetPointer, currentAudioIntensityLevel * 65535); };
        if (effect == 1) effect2 = [](int ledIndex) { return meteorRain2(effect2Time1, effect2Time2, effect2Time2 / 2, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 2) effect2 = [](int ledIndex) { return meteorRain(effect2Time1,effect2Time2, ledIndex, 50, .12, *effect2TransformMap2, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 3) effect2 = [](int ledIndex) { return meteorRain2(effect2Time1, effect2Time2, 0, ledIndex, 50, .12, *effect2TransformMap1, *effect2TransformMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 4) effect2 = [](int ledIndex) { return starShimmer(40, 200, ledIndex, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 5) effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap2, *effect2TransformMap1, *currentPalette1OffsetPointer, 65535); };
        // else if (effect == 6) effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap2, *effect2TransformMap1, *currentPalette1OffsetPointer, 32767 + (32766 * currentAudioIntensityLevel)); };
        // else if (effect == 7) effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap2, *effect2TransformMap1, *currentPalette1OffsetPointer, 65535 - (currentAudioIntensityLevel * 65535)); };
    }
    else if (effect == 2)
    {
        effect = random(8);
        // if (effect == 0) effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette1OffsetPointer, currentAudioIntensityLevel * 65535); };
        if (effect == 1) effect3 = [](int ledIndex) { return meteorRain2(effect3Time1, effect3Time2, effect3Time2 / 2, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 2) effect3 = [](int ledIndex) { return meteorRain(effect3Time1,effect3Time2, ledIndex, 50, .12, *effect3TransformMap2, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 3) effect3 = [](int ledIndex) { return meteorRain2(effect3Time1, effect3Time2, 0, ledIndex, 50, .12, *effect3TransformMap1, *effect3TransformMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 4) effect3 = [](int ledIndex) { return starShimmer(40, 200, ledIndex, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 5) effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap2, *effect3TransformMap1, *currentPalette1OffsetPointer, 65535); };
        // else if (effect == 6) effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap2, *effect3TransformMap1, *currentPalette1OffsetPointer, 32767 + (32766 * currentAudioIntensityLevel)); };
        // else if (effect == 7) effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap2, *effect3TransformMap1, *currentPalette1OffsetPointer, 65535 - (currentAudioIntensityLevel * 65535)); };
    }
    else if (effect == 3)
    {
        effect = random(8);
        // if (effect == 0) effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette1OffsetPointer, currentAudioIntensityLevel * 65535); };
        if (effect == 1) effect4 = [](int ledIndex) { return meteorRain2(effect4Time1, effect4Time2, effect4Time2 / 2, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 2) effect4 = [](int ledIndex) { return meteorRain(effect4Time1,effect4Time2, ledIndex, 50, .12, *effect4TransformMap2, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 3) effect4 = [](int ledIndex) { return meteorRain2(effect4Time1, effect4Time2, 0, ledIndex, 50, .12, *effect4TransformMap1, *effect4TransformMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 4) effect4 = [](int ledIndex) { return starShimmer(40, 120, ledIndex, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 5) effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap2, *effect4TransformMap1, *currentPalette1OffsetPointer, 65535); };
        // else if (effect == 6) effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap2, *effect4TransformMap1, *currentPalette1OffsetPointer, 32767 + (32766 * currentAudioIntensityLevel)); };
        // else if (effect == 7) effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap2, *effect4TransformMap1, *currentPalette1OffsetPointer, 65535 - (currentAudioIntensityLevel * 65535)); };
    }
}

void randomizeBlendingMode()
{
    mixingModeBlendFunction = mixingModeBlendFunctions[random(numberOfMixingModeBlendFunctions)];
    mixingModeBlendCounterMax = random(MAX_CROSS_FADE_DURATION - MIN_CROSS_FADE_DURATION) + MIN_CROSS_FADE_DURATION;
    mixingModeBlendCounter = mixingModeBlendCounterMax;
}

void setupEffects()
{
    currentPalette1OffsetPointer = &currentPalette1Offset;
    currentPalette2OffsetPointer = &currentPalette2Offset;
    effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, getAudioIntensityRatio() * 65535); };
    effect1Plus = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, 65535); };
    effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette1OffsetPointer, 65535); };
    effect2Plus = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette1OffsetPointer, 65535); };
    effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette2OffsetPointer, 65535); };
    effect3Plus = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette2OffsetPointer, 65535); };
    effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette2OffsetPointer, 65535); };
    effect4Plus = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette2OffsetPointer, 65535); };

    *effect1TransformMap1 = normalTransformMapX;
    *effect1TransformMap2 = normalTransformMapY;

    mixingModeBlendFunction = blendColorsUsingMixing;
    oldMixingModeBlendFunction = blendColorsUsingMixing;

    subscribeToStepDetectedEvent(handleStepDetected);
    subscribeToMovementDetectedEvent(handleMovementDetected);

    //timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return timeDelta; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return timeDelta >> 1; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)timeDelta / 3; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)timeDelta >> 2; };
    //timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 1; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) / 3; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 2; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5))); };
    //timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .6))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5))); };
    //timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .6))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return (int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4))); };
    
    //timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -timeDelta; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -timeDelta >> 1; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)timeDelta / 3; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)timeDelta >> 2; };
    //timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 1; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) / 3; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(currentAudioIntensityLevel * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 2; };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5))); };
    //timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .6))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)(timeDelta * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5))); };
    //timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .6))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int timeDelta) { return -(int)((timeDelta >> 1) * (audioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4))); };

    //mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixingGlitched;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixing;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAdd;
    //mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingOverlay;
    //mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingScreen;
    //mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAverage;
    //mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingShimmer;
    //mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMultiply;
    //mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingSubtract;

    for (int i = 0; i < 150; i++) randomizeEffectsNaturally();
}

inline void incrementTime()
{
    int newTime = (int)getTime();
    int timeDelta = (newTime - currentTime) * 64;
    currentTime = getTime();
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
    loopCount += 1;
    if (loopCount > 1500)
    {
        if (loopCountDirection == 1)
        {
            if (transition < 65000) transition = transition + 35;
            else {
                loopCountDirection = -1;
                loopCount = 0;
            }
        }
        else 
        {
            if (transition > 70) transition = transition - 35;
            else { 
                loopCountDirection = 1;
                loopCount = 0;
            }
        }
    }

    if (oldMixingMode != mixingMode && mixingModeBlendCounter) mixingModeBlendCounter--;
    else {
        oldMixingMode = mixingMode;
        oldMixingModeBlendFunction = mixingModeBlendFunction;
    }
}

void handleStepDetected()
{
    Serial.println("Step detected!");
}

void handleMovementDetected()
{
    Serial.println("Movement detected!");
    Serial.println(getCurrentMovementType());
}