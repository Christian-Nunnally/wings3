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
byte pickRandomTimeModeAndAlignCurrentTimes(byte currentTimeMode);
void registerTimeMode(void (*timeModeIncrementFunction)(uint32_t timeDelta));

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
void (*timeModeIncrementFunctions[MaxNumberOfTimeModes])(uint32_t timeDelta);
uint32_t timeModeCurrentTimes[MaxNumberOfTimeModes];

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
        int currentRandom = random(10);
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
            if (random(10) > 6) effect1TimeMode1 = pickRandomTimeModeAndAlignCurrentTimes(effect1TimeMode1);
            if (random(10) > 6) effect2TimeMode1 = pickRandomTimeModeAndAlignCurrentTimes(effect2TimeMode1);
            if (random(10) > 6) effect3TimeMode1 = pickRandomTimeModeAndAlignCurrentTimes(effect3TimeMode1);
            if (random(10) > 6) effect4TimeMode1 = pickRandomTimeModeAndAlignCurrentTimes(effect4TimeMode1);
            if (random(10) > 6) effect1TimeMode2 = pickRandomTimeModeAndAlignCurrentTimes(effect1TimeMode1);
            if (random(10) > 6) effect2TimeMode2 = pickRandomTimeModeAndAlignCurrentTimes(effect2TimeMode1);
            if (random(10) > 6) effect3TimeMode2 = pickRandomTimeModeAndAlignCurrentTimes(effect3TimeMode1);
            if (random(10) > 6) effect4TimeMode2 = pickRandomTimeModeAndAlignCurrentTimes(effect4TimeMode1);
        }
        else if (currentRandom == 6)
        {
            pickRandomEffects();
        }
    }
    lastPeakDetectorValue = nextPeak;
}

byte pickRandomTimeModeAndAlignCurrentTimes(byte currentTimeMode)
{
    byte newTimeMode = random(numberOfTimeModes);
    timeModeCurrentTimes[newTimeMode] = timeModeCurrentTimes[currentTimeMode];
    return newTimeMode;
}

void pickRandomEffects()
{
    int effect = random(8);
    if (effect == 0)
    {
        effect = random(4);
        if (effect == 0) effect1 = [](int ledIndex) { return wavePulse(effect1Time1, effect1Time2, ledIndex, *effect1TransformMap1, *effect1TransformMap2, *currentPalette1OffsetPointer, getAudioIntensityRatio() * 65535); };
        else if (effect == 1) effect1 = [](int ledIndex) { return meteorRain2(effect1Time1, effect1Time2, effect1Time2, ledIndex, 30, .1, normalTopRadiusMap2, normalTopRadiusMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 2) effect1 = [](int ledIndex) { return meteorRain(effect1Time1,effect1Time2, ledIndex, 50, .01, normalTransformMapX, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 3) effect1 = [](int ledIndex) { return starShimmer(15, 120, ledIndex, *currentPalette1OffsetPointer, 65535); };
    }
    else if (effect == 1)
    {
        effect = random(4);
        if (effect == 0) effect2 = [](int ledIndex) { return wavePulse(effect2Time1, effect2Time2, ledIndex, *effect2TransformMap1, *effect2TransformMap2, *currentPalette1OffsetPointer, getAudioIntensityRatio() * 65535); };
        else if (effect == 1) effect2 = [](int ledIndex) { return meteorRain2(effect2Time1, effect2Time2, effect2Time2, ledIndex, 30, .1, normalTopRadiusMap2, normalTopRadiusMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 2) effect2 = [](int ledIndex) { return meteorRain(effect2Time1,effect2Time2, ledIndex, 50, .01, normalTransformMapX, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 3) effect2 = [](int ledIndex) { return starShimmer(15, 120, ledIndex, *currentPalette1OffsetPointer, 65535); };
    }
    else if (effect == 2)
    {
        effect = random(4);
        if (effect == 0) effect3 = [](int ledIndex) { return wavePulse(effect3Time1, effect3Time2, ledIndex, *effect3TransformMap1, *effect3TransformMap2, *currentPalette1OffsetPointer, getAudioIntensityRatio() * 65535); };
        else if (effect == 1) effect3 = [](int ledIndex) { return meteorRain2(effect3Time1, effect3Time2, effect3Time2, ledIndex, 30, .1, normalTopRadiusMap2, normalTopRadiusMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 2) effect3 = [](int ledIndex) { return meteorRain(effect3Time1,effect3Time2, ledIndex, 50, .01, normalTransformMapX, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 3) effect3 = [](int ledIndex) { return starShimmer(15, 120, ledIndex, *currentPalette1OffsetPointer, 65535); };
    }
    else if (effect == 3)
    {
        effect = random(4);
        if (effect == 0) effect4 = [](int ledIndex) { return wavePulse(effect4Time1, effect4Time2, ledIndex, *effect4TransformMap1, *effect4TransformMap2, *currentPalette1OffsetPointer, getAudioIntensityRatio() * 65535); };
        else if (effect == 1) effect4 = [](int ledIndex) { return meteorRain2(effect4Time1, effect4Time2, effect4Time2, ledIndex, 30, .1, normalTopRadiusMap2, normalTopRadiusMap1, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 2) effect4 = [](int ledIndex) { return meteorRain(effect4Time1,effect4Time2, ledIndex, 50, .01, normalTransformMapX, *currentPalette1OffsetPointer, 65535); };
        else if (effect == 3) effect4 = [](int ledIndex) { return starShimmer(15, 120, ledIndex, *currentPalette1OffsetPointer, 65535); };
    }
}

void randomizeBlendingMode()
{
    mixingMode = random(8);
    if (mixingMode == 0) mixingModeBlendFunction = blendColorsUsingMixingGlitched;
    if (mixingMode == 1) mixingModeBlendFunction = blendColorsUsingMixing;
    else if (mixingMode == 2) mixingModeBlendFunction = blendColorsUsingAdd;
    else if (mixingMode == 3) mixingModeBlendFunction = blendColorsUsingOverlay;
    else if (mixingMode == 4) mixingModeBlendFunction = blendColorsUsingScreen;
    else if (mixingMode == 5) mixingModeBlendFunction = blendColorsUsingAverage;
    else if (mixingMode == 6) mixingModeBlendFunction = blendColorsUsingShimmer;
    else mixingModeBlendFunction = blendColorsUsingMultiply;
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

    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[0] += timeDelta >> 1; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[1] += timeDelta / 3; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[2] += timeDelta >> 2; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[3] += timeDelta / 5; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[4] += timeDelta / 7; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[5] += timeDelta >> 3; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[6] += timeDelta / 13; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[7] += timeDelta >> 4; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[8] += (getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta); });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[9] += (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 1; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[10] += (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) / 3; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[11] += (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 2; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[12] += (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) / 5; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[13] += (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) / 7; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[14] += (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 3; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[15] += (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) / 13; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[16] += timeDelta - (getAudioIntensityRatio() * 10); });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[17] += (timeDelta >> 2) - (getAudioIntensityRatio() * 10); });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[18] += (timeDelta >> 3) - (getAudioIntensityRatio() * 10); });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[19] += (timeDelta >> 4) - (getAudioIntensityRatio() * 10); });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[20] -= timeDelta >> 1; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[21] -= timeDelta / 3; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[22] -= timeDelta >> 2; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[23] -= timeDelta / 5; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[24] -= timeDelta / 7; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[25] -= timeDelta >> 3; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[26] -= timeDelta / 13; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[27] -= timeDelta >> 4; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[28] -= (getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta); });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[29] -= (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 1; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[30] -= (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) / 3; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[31] -= (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 2; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[32] -= (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) / 5; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[33] -= (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) / 7; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[34] -= (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) >> 3; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[35] -= (int)(getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta) / 13; });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[36] -= timeDelta - (getAudioIntensityRatio() * 10); });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[37] -= (timeDelta >> 2) - (getAudioIntensityRatio() * 10); });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[38] -= (timeDelta >> 3) - (getAudioIntensityRatio() * 10); });
    registerTimeMode([](uint32_t timeDelta) { timeModeCurrentTimes[39] -= (timeDelta >> 4) - (getAudioIntensityRatio() * 10); });
}

void registerTimeMode(void (*timeModeIncrementFunction)(uint32_t timeDelta))
{
    if (numberOfTimeModes >= MaxNumberOfTimeModes) return;
    timeModeIncrementFunctions[numberOfTimeModes] = timeModeIncrementFunction;
    numberOfTimeModes++;
}

inline void incrementTime()
{
    uint32_t lastTime = currentTime;
    uint32_t timeDelta = currentTime - lastTime;
    currentTime = getTime() >> 1;

    bool timeModesToIncrement[MaxNumberOfTimeModes];
    timeModesToIncrement[effect1TimeMode1] = true;
    timeModesToIncrement[effect2TimeMode1] = true;
    timeModesToIncrement[effect3TimeMode1] = true;
    timeModesToIncrement[effect4TimeMode1] = true;
    timeModesToIncrement[effect1TimeMode2] = true;
    timeModesToIncrement[effect2TimeMode2] = true;
    timeModesToIncrement[effect3TimeMode2] = true;
    timeModesToIncrement[effect4TimeMode2] = true;

    for (int i = 0; i < numberOfTimeModes; i++)
    {
        if (timeModesToIncrement[i]) timeModeIncrementFunctions[i](timeDelta);
    }

    effect1Time2 = timeModeCurrentTimes[effect1TimeMode2];
    effect2Time2 = timeModeCurrentTimes[effect2TimeMode2];
    effect3Time2 = timeModeCurrentTimes[effect3TimeMode2];
    effect4Time2 = timeModeCurrentTimes[effect4TimeMode2];
    effect1Time2 = timeModeCurrentTimes[effect1TimeMode2];
    effect2Time2 = timeModeCurrentTimes[effect2TimeMode2];
    effect3Time2 = timeModeCurrentTimes[effect3TimeMode2];
    effect4Time2 = timeModeCurrentTimes[effect4TimeMode2];
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