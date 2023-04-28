#include <Adafruit_NeoPXL8.h>
#include "effectController.h"
#include "settings.h"
#include "graphics.h"
#include "microphone.h"
#include "movementDetection.h"
#include "transformMaps.h"
#include "time.h"
#include "analogInput.h"
#include "screenMaps.h"
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
void moveColorPalettesTowardsTargets();

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

uint8_t *effect1TransformMap1[272];
uint8_t *effect1TransformMap2[272];
uint8_t *effect2TransformMap1[272];
uint8_t *effect2TransformMap2[272];
uint8_t *effect3TransformMap1[272];
uint8_t *effect3TransformMap2[272];
uint8_t *effect4TransformMap1[272];
uint8_t *effect4TransformMap2[272];

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

const byte MaxNumberOfTimeModes = 40;
byte numberOfTimeModes;
int (*timeModeIncrementFunctions[MaxNumberOfTimeModes])(int timeDelta);

Color ledColorMap[TOTAL_LEDS];

byte *currentScreenMap[TOTAL_LEDS];

int meteorSize = 100;

Color getLedColorForFrame(int ledIndex)
{
    byte currentScreen = (*currentScreenMap)[ledIndex];
    if (currentScreen == 1) return {0, 65535, 0};
    if (switchMap || currentScreen == 1)
    {
        if (currentScreenMap[ledIndex])
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
    ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, (.2 * 65535));
    if (switchMapBlendCounter) ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, (.1 * 65535));
    else ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, (.5 * 65535));
    return ledColorMap[ledIndex];
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
    moveColorPalettesTowardsTargets();
}

uint32_t lastMoveColorPalettesTowardsTargetsTime;
uint32_t moveColorPalettesTowardsTargetsTimeThreshold = 80;
void moveColorPalettesTowardsTargets()
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
        int currentRandom2 = random(4);
        if (currentRandom2 == 0)
        {
            currentPalette1 = random(TOTAL_NUMBER_OF_PALETTES);
            currentPalette2 = currentPalette1;
            currentPalette1OffsetTarget = pickRandomSubPaletteFromPalette(currentPalette1);
            currentPalette2OffsetTarget = pickRandomSubPaletteFromPalette(currentPalette1);
        }
        else if (currentRandom2 == 1) {
            currentPalette1OffsetTarget = pickRandomSubPaletteFromPalette(currentPalette1);
            currentPalette2OffsetTarget = pickRandomSubPaletteFromPalette(currentPalette1);
        }
        else if (currentRandom2 == 2) {
            currentPalette1OffsetTarget = pickRandomSubPaletteFromPalette(currentPalette1);
            currentPalette2OffsetTarget = currentPalette1Offset;
        }
        else 
        {
            if (random(20 == 0)) 
            {
                currentPalette1 = random(TOTAL_NUMBER_OF_PALETTES);
                currentPalette2 = random(TOTAL_NUMBER_OF_PALETTES);
                currentPalette1Offset = pickRandomSubPaletteFromPalette(currentPalette1);
                currentPalette2Offset = pickRandomSubPaletteFromPalette(currentPalette1);
            }
        }
    }
    else if (currentRandom == 4)
    {
        switchMap = !switchMap;
        if (switchMap)
        {
            if (random(10) > 7) *effect1TransformMap1 = transformMaps[random(transformMapsCount)];
            if (random(10) > 7) *effect1TransformMap2 = transformMaps[random(transformMapsCount)];
            if (random(10) > 7) *effect2TransformMap1 = transformMaps[random(transformMapsCount)];
            if (random(10) > 7) *effect2TransformMap2 = transformMaps[random(transformMapsCount)];
        }
        else
        {
            if (random(10) > 7) *effect3TransformMap1 = transformMaps[random(transformMapsCount)];
            if (random(10) > 7) *effect3TransformMap2 = transformMaps[random(transformMapsCount)];
            if (random(10) > 7) *effect4TransformMap1 = transformMaps[random(transformMapsCount)];
            if (random(10) > 7) *effect4TransformMap2 = transformMaps[random(transformMapsCount)];
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
    else if (currentRandom == 7)
    {
        //*currentScreenMap = screenMaps[random(screenMapsCount)];
    }
    else if (currentRandom == 8)
    {
        meteorSize = random(190) + 10;
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
        effect = random(12);
        if (effect == 0) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, currentAudioIntensityLevel * 65535); };
        else if (effect == 1) effect1 = [](int ledIndex) { return meteorRain2(effect1Time1, effect1Time2, effect1Time2 >> 3, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 2) effect1 = [](int ledIndex) { return meteorRain(effect1Time1,effect1Time2, ledIndex, meteorSize, .12, *effect1TransformMap2, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 3) effect1 = [](int ledIndex) { return meteorRain2(effect1Time1, effect1Time2, 0, ledIndex, 50, .12, *effect1TransformMap1, *effect1TransformMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 4) effect1 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 5) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap2, *effect1TransformMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 6) effect1 = [](int ledIndex) { return rainShower(frameTimeDelta, effect1Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette1OffsetPointer, 65535); };
        else if (random(5 == 0)) effect1 = [](int ledIndex) { return ledTest(currentTime, ledIndex, effect1Time2, *currentPalette1OffsetPointer, 5, currentAudioIntensityLevel * 65535); };
    }
    else if (effect == 1)
    {
        effect = random(12);
        if (effect == 0) effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette2OffsetPointer, currentAudioIntensityLevel * 65535); };
        else if (effect == 1) effect2 = [](int ledIndex) { return meteorRain2(effect2Time1, effect2Time2, effect2Time2 >> 3, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette2OffsetPointer, 65535); };
        else if (effect == 2) effect2 = [](int ledIndex) { return meteorRain(effect2Time1,effect2Time2, ledIndex, meteorSize, .12, *effect2TransformMap2, *currentPalette2OffsetPointer, 65535); };
        else if (effect == 3) effect2 = [](int ledIndex) { return meteorRain2(effect2Time1, effect2Time2, 0, ledIndex, 50, .12, *effect2TransformMap1, *effect2TransformMap1, *currentPalette2OffsetPointer, 65535); };
        else if (effect == 4) effect2 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette2OffsetPointer, 65535); };
        else if (effect == 5) effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap2, *effect2TransformMap1, *currentPalette2OffsetPointer, 65535); };
        else if (effect == 6) effect2 = [](int ledIndex) { return rainShower(frameTimeDelta, effect2Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette2OffsetPointer, 65535); };
        else if (random(5 == 0)) effect2 = [](int ledIndex) { return ledTest(currentTime, ledIndex, effect2Time2, *currentPalette2OffsetPointer, 10, 65535); };
    }
    else if (effect == 2)
    {
        effect = random(12);
        if (effect == 0) effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette1OffsetPointer, currentAudioIntensityLevel * 65535); };
        else if (effect == 1) effect3 = [](int ledIndex) { return meteorRain2(effect3Time1, effect3Time2, effect3Time2 >> 3, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 2) effect3 = [](int ledIndex) { return meteorRain(effect3Time1,effect3Time2, ledIndex, meteorSize, .12, *effect3TransformMap2, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 3) effect3 = [](int ledIndex) { return meteorRain2(effect3Time1, effect3Time2, 0, ledIndex, 50, .12, *effect3TransformMap1, *effect3TransformMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 4) effect3 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 40, ledIndex, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 5) effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap2, *effect3TransformMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 6) effect3 = [](int ledIndex) { return rainShower(frameTimeDelta, effect3Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette1OffsetPointer, 65535); };
        else if (random(5 == 0)) effect3 = [](int ledIndex) { return ledTest(currentTime, ledIndex, effect3Time2, *currentPalette2OffsetPointer, 10, 65535); };
    }
    else if (effect == 3)
    {
        effect = random(12);
        if (effect == 0) effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette2OffsetPointer, currentAudioIntensityLevel * 65535); };
        else if (effect == 1) effect4 = [](int ledIndex) { return meteorRain2(effect4Time1, effect4Time2, effect4Time2 >> 3, ledIndex, 30, .12, normalTransformMapX, normalTransformMapY, *currentPalette2OffsetPointer, 65535); };
        else if (effect == 2) effect4 = [](int ledIndex) { return meteorRain(effect4Time1,effect4Time2, ledIndex, meteorSize, .12, *effect4TransformMap2, *currentPalette2OffsetPointer, 65535); };
        else if (effect == 3) effect4 = [](int ledIndex) { return meteorRain2(effect4Time1, effect4Time2, 0, ledIndex, 50, .12, *effect4TransformMap1, *effect4TransformMap1, *currentPalette2OffsetPointer, 65535); };
        else if (effect == 4) effect4 = [](int ledIndex) { return starShimmer(frameTimeDelta, 16, 80, ledIndex, *currentPalette2OffsetPointer, 65535); };
        else if (effect == 5) effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap2, *effect4TransformMap1, *currentPalette2OffsetPointer, 65535); };
        else if (effect == 6) effect4 = [](int ledIndex) { return rainShower(frameTimeDelta, effect4Time1, ledIndex, meteorSize, normalTransformMapY, normalTransformMapX, *currentPalette2OffsetPointer, 65535); };
        else if (random(5 == 0)) effect4 = [](int ledIndex) { return ledTest(currentTime, ledIndex, effect4Time2, *currentPalette2OffsetPointer, 10, 65535); };
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

    for (int i = 0; i < 150; i++) randomizeEffectsNaturally();

    *currentScreenMap = logoScreenMap;
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