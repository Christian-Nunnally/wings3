#include <Adafruit_NeoPXL8.h>
#include "effectController.h"
#include "settings.h"
#include "graphics.h"
#include "microphone.h"
#include "movementDetection.h"
#include "transformMaps.h"
#include "palettes.h"
#include "time.h"

#define MIN_CROSS_FADE_DURATION 15.0
#define MAX_CROSS_FADE_DURATION 200.0

int currentTime;
int currentTimeHalf;
int currentTimeFifth;
int currentTimeEighth;
int currentTimeSixteenth;
int audioScaledTime;
int audioScaledTimeHalf;
int audioScaledTimeFifth;
int audioScaledTimeEighth;
int audioReverseScaledTime;
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
int* getRandomEffectSpeed();

Color (*effect1)(int pixel) {};
Color (*effect1Plus)(int pixel) {};
Color (*effect2)(int pixel) {};
Color (*effect2Plus)(int pixel) {};
Color (*effect3)(int pixel) {};
Color (*effect3Plus)(int pixel) {};
Color (*effect4)(int pixel) {};
Color (*effect4Plus)(int pixel) {};

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

int *effect1Time;
int *effect2Time;
int *effect3Time;
int *effect4Time;

int currentPalette1;
int currentPalette2;
int currentPalette1Offset; 
int currentPalette2Offset; 
int *currentPalette1OffsetPointer; 
int *currentPalette2OffsetPointer; 

Color getLedColorForFrame(int ledIndex)
{
    if (switchMap)
    {
        Color color1 = currentAudioIntensityLevel < .98 ? effect1(ledIndex) : effect1Plus(ledIndex);
        Color color2 = currentAudioIntensityLevel < .5 ? effect2(ledIndex) : effect2Plus(ledIndex);
        Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2);
        if (switchMapBlendCounter)
        {
            Color color3 = currentAudioIntensityLevel < .98 ? effect3(ledIndex) : effect4Plus(ledIndex);
            Color color4 = currentAudioIntensityLevel < .5 ? effect4(ledIndex) : effect4Plus(ledIndex);
            Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4);
            return blendColorsUsingMixing(resultColor1, resultColor2, (switchMapBlendCounter / switchMapBlendCounterMax) * 65535);
        }
        return resultColor1;
    }
    else 
    {
        Color color1 = currentAudioIntensityLevel < .98 ? effect3(ledIndex) : effect4Plus(ledIndex);
        Color color2 = currentAudioIntensityLevel < .5 ? effect4(ledIndex) : effect4Plus(ledIndex);
        Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2);
        if (switchMapBlendCounter)
        {
            Color color3 = currentAudioIntensityLevel < .98 ? effect3(ledIndex) : effect4Plus(ledIndex);
            Color color4 = currentAudioIntensityLevel < .5 ? effect4(ledIndex) : effect4Plus(ledIndex);
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
    int nextPeak = getCurrentPeakDetectorValue();
    if (nextPeak != lastPeakDetectorValue && nextPeak == 1)
    {
        Serial.println("RANDOMIZING!");
        int currentRandom = random(6);
        if (currentRandom == 0) randomizeBlendingMode();
        if (currentRandom == 1)
        {
            currentPalette1 = random(TOTAL_NUMBER_OF_PALETTES);
            currentPalette2 = random(TOTAL_NUMBER_OF_PALETTES);
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
            if (random(10) > 6) effect1Time = getRandomEffectSpeed();
            if (random(10) > 6) effect2Time = getRandomEffectSpeed();
            if (random(10) > 6) effect3Time = getRandomEffectSpeed();
            if (random(10) > 6) effect4Time = getRandomEffectSpeed();
        }
    }
    lastPeakDetectorValue = nextPeak;
}

int* getRandomEffectSpeed()
{
    byte timeMode = random(10);
    if (timeMode == 0) return &currentTime;
    else if (timeMode == 1) return &currentTimeHalf;
    else if (timeMode == 2) return &currentTimeFifth;
    else if (timeMode == 3) return &currentTimeEighth;
    else if (timeMode == 4) return &audioReverseScaledTime;
    else if (timeMode == 5) return &audioScaledTime;
    else if (timeMode == 6) return &audioScaledTimeHalf;
    else if (timeMode == 7) return &audioScaledTimeFifth;
    else if (timeMode == 8) return &currentTimeSixteenth;
    return &audioScaledTimeEighth;
}

void randomizeBlendingMode()
{
    mixingMode = random(8);
    if (mixingMode == 0) mixingModeBlendFunction = blendColorsUsingMixingGlitched;
    else if (mixingMode == 1) mixingModeBlendFunction = blendColorsUsingMixing;
    else if (mixingMode == 2) mixingModeBlendFunction = blendColorsUsingAdd;
    else if (mixingMode == 3) mixingModeBlendFunction = blendColorsUsingOverlay;
    else if (mixingMode == 4) mixingModeBlendFunction = blendColorsUsingScreen;
    else if (mixingMode == 5) mixingModeBlendFunction = blendColorsUsingAverage;
    else if (mixingMode == 6) mixingModeBlendFunction = blendColorsUsingShimmer;
    else mixingModeBlendFunction = blendColorsUsingSubtract;
    mixingModeBlendCounterMax = random(MAX_CROSS_FADE_DURATION - MIN_CROSS_FADE_DURATION) + MIN_CROSS_FADE_DURATION;
    mixingModeBlendCounter = mixingModeBlendCounterMax;
}

void setupEffects()
{
    currentPalette1OffsetPointer = &currentPalette1Offset;
    currentPalette2OffsetPointer = &currentPalette2Offset;
    effect1 = [](int ledIndex) { return wavePulse(*effect1Time, audioScaledTime, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, getAudioIntensityRatio() * 65535); };
    effect1Plus = [](int ledIndex) { return wavePulse(*effect1Time, audioScaledTime, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, 65535); };
    effect2 = [](int ledIndex) { return wavePulse(*effect2Time, audioScaledTime, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette1OffsetPointer, 65535); };
    effect2Plus = [](int ledIndex) { return wavePulse(*effect2Time, audioScaledTime, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette1OffsetPointer, 65535); };
    effect3 = [](int ledIndex) { return wavePulse(*effect3Time, audioScaledTime, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette2OffsetPointer, 65535); };
    effect3Plus = [](int ledIndex) { return wavePulse(*effect3Time, audioScaledTime, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette2OffsetPointer, 65535); };
    effect4 = [](int ledIndex) { return wavePulse(*effect4Time, audioScaledTime, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette2OffsetPointer, 65535); };
    effect4Plus = [](int ledIndex) { return wavePulse(*effect4Time, audioScaledTime, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette2OffsetPointer, 65535); };

    *effect1TransformMap1 = normalTransformMapX;
    *effect1TransformMap2 = normalTransformMapY;

    mixingModeBlendFunction = blendColorsUsingMixing;
    oldMixingModeBlendFunction = blendColorsUsingMixing;
}

inline void incrementTime()
{
    int lastTime = currentTime;
    currentTime = getTime() >> 1;
    currentTimeHalf = currentTime >> 1;
    currentTimeFifth = currentTime / 5;
    currentTimeEighth = currentTime >> 3;
    currentTimeSixteenth = currentTime >> 4;
    int timeDelta = currentTime - lastTime;
    Serial.print("ft: ");
    Serial.println(timeDelta);
    audioScaledTime += getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta;
    audioScaledTimeHalf = audioScaledTime >> 1;
    audioScaledTimeFifth = audioScaledTime / 5;
    audioScaledTimeEighth = audioScaledTime >> 3;

    audioReverseScaledTime += timeDelta;
    audioReverseScaledTime -= getAudioIntensityRatio() / 100;;
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