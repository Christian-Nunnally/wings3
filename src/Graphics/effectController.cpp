#include <Arduino.h>
#include "../settings.h"
#include "../Peripherals/microphone.h"
#include "../Peripherals/movementDetection.h"
#include "../Graphics/effectController.h"
#include "../Graphics/transformMaps.h"
#include "../Graphics/screenMaps.h"
#include "../Graphics/effectSettings.h"
#include "../Graphics/colorMixing.h"
#include "../Graphics/palettes.h"
#include "../Graphics/effect.h"
#include "../Graphics/moods.h"
#include "../Graphics/Effects/solidColorFillEffect.h"
#include "../Graphics/Effects/lightChaseEffect.h"
#include "../Graphics/Effects/fireworksEffect.h"
#include "../Graphics/Effects/fireEffect.h"
#include "../Graphics/Effects/expandingColorOrbEffect.h"
#include "../Graphics/Effects/lightningBugEffect.h"
#include "../Graphics/Effects/rainShowerEffect.h"
#include "../Graphics/Effects/meteorRainEffect.h"
#include "../Graphics/Effects/gradientWaveEffect.h"
#include "../Graphics/Effects/starFieldEffect.h"
#include "../Utility/time.h"
#include "../IO/analogInput.h"
#include "../Observers/stepDectectedObserver.h"
#include "../Observers/movementDetectedObserver.h"
#include "../Utility/fastRandom.h"

Color blendIncorporatingOldMixingMode(Color color1, Color color2);
void handleStepDetected();
void handleMovementDetected();
void incrementBrightnessModeLevels(float currentAudioIntensityLevel);
void incrementTime();
void incrementTime(Effect *effect, int timeDelta);  
void incrementTransition();
void incrementColorPalettesTowardsTargets();
void detectBeat();
void randomizeEffectsNaturally();
void pickRandomBlendingMode();
void pickRandomSubPalette();
void pickRandomPalette();
int pickRandomSubPaletteFromPalette(int palette);
void pickRandomTransformMaps();
void pickRandomTransformMaps(Effect *effect, byte likelihood);
void swapEffects();
void pickRandomScreenMap();
void pickRandomTimeModes();
void pickRandomTimeModesForEffect(Effect *effect, bool pickFromMovementBasedTimeModes, bool pickFromNonMovementBasedTimeModes);
void pickRandomEffects();
void pickRandomSizeParametersForEffects();
void pickRandomGlobalBrightnessControlModes();
void pickRandomGlobalBrightnessControlModesForEffect(Effect *effect, byte likelihood);
void pickRandomTransitionTime();
void pickRandomAudioLevelThresholdForMoreIntenseEffect();
void switchTransitionDirection();

Effect effect1;
Effect effect2;
Effect effect3;
Effect effect4;
Effect currentPrimaryEffectA;
Effect currentPrimaryEffectB;
Effect currentSecondaryEffectA;
Effect currentSecondaryEffectB;
EffectSettings effectSettings;
EffectSettings effectSettingsStationary;
EffectSettings effectSettingsWalking;
EffectSettings effectSettingsJogging;
EffectSettings effectSettingsBiking;
EffectSettings effectSettingsDriving;

// Mixing Mode variables.
const int MaxNumberOfMixingModeBlendFunctions = 20;
int numberOfMixingModeBlendFunctions;
Color (*mixingModeBlendFunctions[MaxNumberOfMixingModeBlendFunctions])(Color color1, Color color2, uint16_t transitionAmount);
Color (*mixingModeBlendFunction)(Color color1, Color color2, uint16_t transitionAmount) {};
Color (*oldMixingModeBlendFunction)(Color color1, Color color2, uint16_t transitionAmount) {};
int mixingMode;
int oldMixingMode;
int mixingModeBlendCounter;
int mixingModeBlendCounterMax = 1;

// Time Mode variables.
const byte MaxNumberOfTimeModes = 40;
byte numberOfTimeModes;
byte numberOfMovementBasedTimeModes;
int (*timeModeIncrementFunctions[MaxNumberOfTimeModes])(int currentTime, int timeDelta);

// Brightness Mode variables.
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

// Screen Mode variables.
byte *currentScreenMap[TOTAL_LEDS];
Color ledColorMap[TOTAL_LEDS];

// Transition variables.
int transition;
int currentTransitionIncrement;
bool transitionDirection;

int frameTimeDelta;
int currentTime;
float currentAudioIntensityLevel;
int lastPeakDetectorValue;
int switchMapBlendCounter;
int switchMapBlendCounterMax = 1;
bool switchMap = false;
float effect1AudioLevelThresholdToShowMoreIntenseEffect = .9;
float effect2AudioLevelThresholdToShowMoreIntenseEffect = .9;
const float AudioInfluenceFactorForAudioScaledTime = 2.0;

Color getLedColorForFrame(int ledIndex)
{
    byte currentScreen = (*currentScreenMap)[ledIndex];
    if (currentScreen == 4) return {0, 0, 0};
    if ((switchMap && !(currentScreen & 1)) || (!switchMap && currentScreen & 1))
    {
        currentPrimaryEffectA = effect1;
        currentPrimaryEffectB = effect2;
        currentSecondaryEffectA = effect3;
        currentSecondaryEffectB = effect4;
    }
    else 
    {
        currentPrimaryEffectA = effect3;
        currentPrimaryEffectB = effect4;
        currentSecondaryEffectA = effect1;
        currentSecondaryEffectB = effect2;
    }

    Color color1 = currentAudioIntensityLevel < effect1AudioLevelThresholdToShowMoreIntenseEffect ? currentPrimaryEffectA.effectFunction(ledIndex) : currentPrimaryEffectA.effectFunctionHighlight(ledIndex);
    Color color2 = currentAudioIntensityLevel < effect2AudioLevelThresholdToShowMoreIntenseEffect ? currentPrimaryEffectB.effectFunction(ledIndex) : currentPrimaryEffectB.effectFunctionHighlight(ledIndex);
    Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2);
    if (switchMapBlendCounter)
    {
        Color color3 = currentAudioIntensityLevel < effect1AudioLevelThresholdToShowMoreIntenseEffect ? currentSecondaryEffectA.effectFunction(ledIndex) : currentSecondaryEffectA.effectFunctionHighlight(ledIndex);
        Color color4 = currentAudioIntensityLevel < effect2AudioLevelThresholdToShowMoreIntenseEffect ? currentSecondaryEffectB.effectFunction(ledIndex) : currentSecondaryEffectB.effectFunctionHighlight(ledIndex);
        Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4);
        resultColor1 = blendColorsUsingMixing(resultColor1, resultColor2, (switchMapBlendCounter / switchMapBlendCounterMax) * UINT16_MAX);
    }
    
    if (switchMapBlendCounter) ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenSwitchingTransformMaps);
    else ledColorMap[ledIndex] = blendColorsUsingMixing(ledColorMap[ledIndex], resultColor1, effectSettings.GlobalPercentOfLastFrameToUseWhenNotSwitchingTransformMaps);
    if (currentScreen & 2) return {ledColorMap[ledIndex].green, ledColorMap[ledIndex].red, ledColorMap[ledIndex].blue};
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
    effect1.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect1.time1, effect1.time2, ledIndex, *(effect1.transformMap1), *(effect1.transformMap2), effect1.currentPaletteOffset, UINT16_MAX); };
    effect2.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect2.time1, effect2.time2, ledIndex, *(effect2.transformMap1), *(effect2.transformMap2), effect2.currentPaletteOffset, UINT16_MAX); };
    effect3.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect3.time1, effect3.time2, ledIndex, *(effect3.transformMap1), *(effect3.transformMap2), effect3.currentPaletteOffset, UINT16_MAX); };
    effect4.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect4.time1, effect4.time2, ledIndex, *(effect4.transformMap1), *(effect4.transformMap2), effect4.currentPaletteOffset, UINT16_MAX); };
    effect1.effectFunctionHighlight = effect1.effectFunction;
    effect2.effectFunctionHighlight = effect2.effectFunction;
    effect3.effectFunctionHighlight = effect3.effectFunction;
    effect4.effectFunctionHighlight = effect4.effectFunction;

    mixingModeBlendFunction = blendColorsUsingMixing;
    oldMixingModeBlendFunction = blendColorsUsingMixing;

    subscribeToStepDetectedEvent(handleStepDetected);
    subscribeToMovementDetectedEvent(handleMovementDetected);

    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + (timeDelta >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)timeDelta / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)timeDelta >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) + ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - (timeDelta >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)timeDelta / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)timeDelta >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 1); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) / 3); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(currentAudioIntensityLevel * AudioInfluenceFactorForAudioScaledTime * timeDelta) >> 2); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)(timeDelta * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .5)))); };
    timeModeIncrementFunctions[numberOfTimeModes++] = [](int currentTime, int timeDelta) { return (currentTime * 64) - ((int)((timeDelta >> 1) * (AudioInfluenceFactorForAudioScaledTime * (currentAudioIntensityLevel - .4)))); };

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

    effect1.globalBrightnessPointer = normalBrightnessModes[0];
    effect2.globalBrightnessPointer = normalBrightnessModes[1];
    effect3.globalBrightnessPointer = normalBrightnessModes[2];
    effect4.globalBrightnessPointer = normalBrightnessModes[3];

    setupNormalMood(&effectSettings);

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

    pickRandomTimeModesForEffect(&effect1, movementType != Stationary, movementType == Stationary);
    pickRandomTimeModesForEffect(&effect2, movementType != Stationary, movementType == Stationary);
    pickRandomTimeModesForEffect(&effect3, movementType != Stationary, movementType == Stationary);
    pickRandomTimeModesForEffect(&effect4, movementType != Stationary, movementType == Stationary);
}

void pickRandomTimeModesForEffect(Effect *effect, bool pickFromMovementBasedTimeModes, bool pickFromNonMovementBasedTimeModes)
{
    if (pickFromMovementBasedTimeModes && pickFromNonMovementBasedTimeModes)
    {
        effect->timeMode1 = fastRandomInteger(numberOfMovementBasedTimeModes + numberOfTimeModes);
        effect->timeMode2 = fastRandomInteger(numberOfMovementBasedTimeModes + numberOfTimeModes);
    }
    else if (pickFromMovementBasedTimeModes)
    {
        effect->timeMode1 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
        effect->timeMode2 = fastRandomInteger(numberOfMovementBasedTimeModes) + numberOfTimeModes;
    }
    else 
    {
        effect->timeMode1 = fastRandomInteger(numberOfTimeModes);
        effect->timeMode2 = fastRandomInteger(numberOfTimeModes);
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

const float TimeDeltaResolutionIncreaseFactor = 64.0;
inline void incrementTime()
{
    int newTime = (int)getTime();
    frameTimeDelta = (newTime - currentTime);
    int timeDelta = frameTimeDelta * TimeDeltaResolutionIncreaseFactor;
    currentTime = newTime;
    incrementTime(&effect1, timeDelta);
    incrementTime(&effect2, timeDelta);
    incrementTime(&effect3, timeDelta);
    incrementTime(&effect4, timeDelta);
}

inline void incrementTime(Effect *effect, int timeDelta)
{
    effect->time1 = timeModeIncrementFunctions[effect->timeMode1](effect->time1, timeDelta) / TimeDeltaResolutionIncreaseFactor;
    effect->time2 = timeModeIncrementFunctions[effect->timeMode1](effect->time1, timeDelta) / TimeDeltaResolutionIncreaseFactor;
}

inline void incrementTransition()
{
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
    incrementColorPalettesTowardsTargetsForEffect(&effect1);
    incrementColorPalettesTowardsTargetsForEffect(&effect2);
    incrementColorPalettesTowardsTargetsForEffect(&effect3);
    incrementColorPalettesTowardsTargetsForEffect(&effect4);
}

void incrementColorPalettesTowardsTargetsForEffect(Effect *effect)
{
    if (effect->currentPaletteOffset > effect->currentPaletteOffsetTarget) effect->currentPaletteOffset -= PALETTE_LENGTH;
    else if (effect->currentPaletteOffset < effect->currentPaletteOffsetTarget) effect->currentPaletteOffset += PALETTE_LENGTH;
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
    pickRandomSubPaletteForEffect(&effect1);
    pickRandomSubPaletteForEffect(&effect2);
    pickRandomSubPaletteForEffect(&effect3);
    pickRandomSubPaletteForEffect(&effect4);
}

void pickRandomSubPaletteForEffect(Effect *effect)
{
    effect->currentPaletteOffsetTarget = pickRandomSubPaletteFromPalette(effect->currentPalette);
}

void pickRandomPalette()
{
    pickRandomPaletteForEffect(&effect1);
    if (fastRandomByte() < effectSettings.LikelihoodTwoPalettesAreUsedWhenPaletteChanges) 
    {
        pickRandomPaletteForEffect(&effect2);
        pickRandomPaletteForEffect(&effect3);
        pickRandomPaletteForEffect(&effect4);
    }
    else
    {
        effect2.currentPalette = effect1.currentPalette;
        effect3.currentPalette = effect1.currentPalette;
        effect4.currentPalette = effect1.currentPalette;
    }
    pickRandomSubPaletteForEffect(&effect1);
    pickRandomSubPaletteForEffect(&effect2);
    pickRandomSubPaletteForEffect(&effect3);
    pickRandomSubPaletteForEffect(&effect4);
}

void pickRandomPaletteForEffect(Effect *effect)
{
    byte numberOfPossiblePalettesToSwitchTo = effectSettings.AllowedPalettes[effect->currentPalette][0];
    byte indexOfNextPalette = 1 + fastRandomInteger(numberOfPossiblePalettesToSwitchTo);
    byte nextPalette = effectSettings.AllowedPalettes[effect->currentPalette][indexOfNextPalette];
    effect->currentPalette = nextPalette;
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
    //     pickRandomMirroredTransformMaps(&effect1, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
    //     pickRandomMirroredTransformMaps(&effect2, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
    //     pickRandomMirroredTransformMaps(&effect3, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
    //     pickRandomMirroredTransformMaps(&effect4, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
    // }
    // else 
    // {
        pickRandomTransformMaps(&effect1, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
        pickRandomTransformMaps(&effect2, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
        pickRandomTransformMaps(&effect3, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
        pickRandomTransformMaps(&effect4, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreRandomized);
    // }
}

void pickRandomTransformMaps(Effect *effect, byte likelihood)
{
    if (fastRandomByte() < likelihood) *(effect->transformMap1) = transformMaps[fastRandomInteger(transformMapsCount)];
    if (fastRandomByte() < likelihood) *(effect->transformMap2) = transformMaps[fastRandomInteger(transformMapsCount)];
}

// void pickRandomMirroredTransformMaps(Effect *effect, byte likelihood)
// {
//     if (fastRandomByte() < likelihood) *(effect->transformMap1) = mirroredTransformMaps[fastRandomInteger(transformMapsCount)];
//     if (fastRandomByte() < likelihood) *(effect->transformMap2) = mirroredTransformMaps[fastRandomInteger(transformMapsCount)];
// }

void swapEffects()
{
    switchMap = !switchMap;
    if (switchMap)
    {
        pickRandomTransformMaps(&effect1, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched);
        pickRandomTransformMaps(&effect2, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched);
    }
    else
    {
        pickRandomTransformMaps(&effect3, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched);
        pickRandomTransformMaps(&effect4, effectSettings.LikelihoodAnyIndividualTransformMapChangesWhenTransformMapsAreSwitched);
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
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effect1.timeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effect2.timeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effect3.timeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effect4.timeMode1 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effect1.timeMode2 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effect2.timeMode2 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effect3.timeMode2 = fastRandomInteger(length) + startIndex;
    if (fastRandomByte() < effectSettings.LikelihoodAnyIndividualTimeModeChangesWhenTimeModeRandomizes) effect4.timeMode2 = fastRandomInteger(length) + startIndex;
}

void pickRandomEffects()
{
    int effectRandom = fastRandomInteger(NumberOfAllowedEffectsToPickBetween);
    byte effectSelection = effectSettings.AllowedEffects[effectRandom];
    if (fastRandomInteger(2) == 0)
    {
        if (effectSelection == 0) effect1.effectFunction = [](int ledIndex) { return meteorRainEffect2(frameTimeDelta, effect1.time1, effect1.time2, effect1.time2 >> 3, ledIndex, effect1.size, .12, normalTransformMapX, normalTransformMapY, effect1.currentPaletteOffset, *(effect1.globalBrightnessPointer)); };
        else if (effectSelection == 1) effect1.effectFunction = [](int ledIndex) { return meteorRainEffect(frameTimeDelta, effect1.time1,effect1.time2, ledIndex, effect1.size, .12, *(effect1.transformMap1), effect1.currentPaletteOffset, *(effect1.globalBrightnessPointer)); };
        else if (effectSelection == 2) effect1.effectFunction = [](int ledIndex) { return meteorRainEffect2(frameTimeDelta, effect1.time1, effect1.time2, 0, ledIndex, effect1.size, .12, *(effect1.transformMap1), *(effect1.transformMap1), effect1.currentPaletteOffset, *(effect1.globalBrightnessPointer)); };
        else if (effectSelection == 3) effect1.effectFunction = [](int ledIndex) { return starFieldEffect(frameTimeDelta, 16, 80, ledIndex, effect1.currentPaletteOffset, *(effect1.globalBrightnessPointer)); };
        else if (effectSelection == 4) effect1.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect1.time1, effect1.time2, ledIndex, *(effect1.transformMap2), *(effect1.transformMap1), effect1.currentPaletteOffset, *(effect1.globalBrightnessPointer)); };
        else if (effectSelection == 5) effect1.effectFunction = [](int ledIndex) { return rainShowerEffect(frameTimeDelta, effect1.time1, ledIndex, effect1.size, normalTransformMapY, normalTransformMapX, effect1.currentPaletteOffset, *(effect1.globalBrightnessPointer)); };
        else if (effectSelection == 6) effect1.effectFunction = [](int ledIndex) { return expandingColorOrbEffect(frameTimeDelta, ledIndex, effect1.currentPaletteOffset, *(effect1.globalBrightnessPointer)); };
        else if (effectSelection == 7) effect1.effectFunction = [](int ledIndex) { return lightningBugEffect(frameTimeDelta, ledIndex, effect1.currentPaletteOffset); };
        else if (effectSelection == 8) effect1.effectFunction = [](int ledIndex) { return fireEffect(frameTimeDelta, ledIndex, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel, normalTransformMapX, normalTransformMapY, effect1.currentPaletteOffset, *(effect1.globalBrightnessPointer)); };
        else if (effectSelection == 9) effect1.effectFunction = [](int ledIndex) { return simpleSolidColorFillEffect(effect1.time1, effect1.currentPaletteOffset, *(effect1.globalBrightnessPointer)); };
        else if (effectSelection == 10) effect1.effectFunction = [](int ledIndex) { return fireworksEffect(frameTimeDelta, effect1.time2, ledIndex, 40, effect1.currentPaletteOffset, 65535); };
        else if (effectSelection == 11) effect1.effectFunction = [](int ledIndex) { return lightChaseEffect(effect1.time2, ledIndex, effect1.time1, effect1.currentPaletteOffset, 5, 65535); };
        else effect1.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect1.time1, effect1.time2, ledIndex, *(effect1.transformMap1), *(effect1.transformMap2), effect1.currentPaletteOffset, *(effect1.globalBrightnessPointer)); };
        if (effectSelection == 0) effect3.effectFunction = [](int ledIndex) { return meteorRainEffect2(frameTimeDelta, effect3.time1, effect3.time2, effect3.time2 >> 3, ledIndex, effect3.size, .12, normalTransformMapX, normalTransformMapY, effect3.currentPaletteOffset, *(effect3.globalBrightnessPointer)); };
        else if (effectSelection == 1) effect3.effectFunction = [](int ledIndex) { return meteorRainEffect(frameTimeDelta, effect3.time1,effect3.time2, ledIndex, effect3.size, .12, *(effect3.transformMap2), effect3.currentPaletteOffset, *(effect3.globalBrightnessPointer)); };
        else if (effectSelection == 2) effect3.effectFunction = [](int ledIndex) { return meteorRainEffect2(frameTimeDelta, effect3.time1, effect3.time2, 0, ledIndex, effect3.size, .12, *(effect3.transformMap1), *(effect3.transformMap2), effect3.currentPaletteOffset, *(effect3.globalBrightnessPointer)); };
        else if (effectSelection == 3) effect3.effectFunction = [](int ledIndex) { return starFieldEffect(frameTimeDelta, 16, 80, ledIndex, effect3.currentPaletteOffset, *(effect3.globalBrightnessPointer)); };
        else if (effectSelection == 4) effect3.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect3.time1, effect3.time2, ledIndex, *(effect3.transformMap2), *(effect3.transformMap1), effect3.currentPaletteOffset, *(effect3.globalBrightnessPointer)); };
        else if (effectSelection == 5) effect3.effectFunction = [](int ledIndex) { return rainShowerEffect(frameTimeDelta, effect3.time1, ledIndex, effect3.size, normalTransformMapY, normalTransformMapX, effect3.currentPaletteOffset, *(effect3.globalBrightnessPointer)); };
        else if (effectSelection == 6) effect3.effectFunction = [](int ledIndex) { return expandingColorOrbEffect(frameTimeDelta, ledIndex, effect3.currentPaletteOffset, *(effect3.globalBrightnessPointer)); };
        else if (effectSelection == 7) effect3.effectFunction = [](int ledIndex) { return lightningBugEffect(frameTimeDelta, ledIndex, effect3.currentPaletteOffset); };
        else if (effectSelection == 8) effect3.effectFunction = [](int ledIndex) { return fireEffect(frameTimeDelta, ledIndex, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel, normalTransformMapX, normalTransformMapY, effect3.currentPaletteOffset, *(effect1.globalBrightnessPointer)); };
        else if (effectSelection == 9) effect3.effectFunction = [](int ledIndex) { return simpleSolidColorFillEffect(effect3.time1, effect3.currentPaletteOffset, *(effect3.globalBrightnessPointer)); };
        else if (effectSelection == 10) effect3.effectFunction = [](int ledIndex) { return fireworksEffect(frameTimeDelta, effect3.time2, ledIndex, 40, effect3.currentPaletteOffset, 65535); };
        else if (effectSelection == 11) effect3.effectFunction = [](int ledIndex) { return lightChaseEffect(effect3.time2, ledIndex, effect3.time1, effect3.currentPaletteOffset, 3, 65535); };
        else effect3.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect3.time1, effect3.time2, ledIndex, *(effect3.transformMap1), *(effect3.transformMap2), effect3.currentPaletteOffset, *(effect3.globalBrightnessPointer)); };
    }
    else
    {
        if (effectSelection == 0) effect2.effectFunction = [](int ledIndex) { return meteorRainEffect2(frameTimeDelta, effect2.time1, effect2.time2, effect2.time2 >> 3, ledIndex, effect2.size, .12, normalTransformMapX, normalTransformMapY, effect2.currentPaletteOffset, *(effect2.globalBrightnessPointer)); };
        else if (effectSelection == 1) effect2.effectFunction = [](int ledIndex) { return meteorRainEffect(frameTimeDelta, effect2.time1,effect2.time2, ledIndex, effect2.size, .12, *(effect2.transformMap2), effect2.currentPaletteOffset, *(effect2.globalBrightnessPointer)); };
        else if (effectSelection == 2) effect2.effectFunction = [](int ledIndex) { return meteorRainEffect2(frameTimeDelta, effect2.time1, effect2.time2, 0, ledIndex, effect2.size, .12, *(effect2.transformMap1), *(effect2.transformMap1), effect2.currentPaletteOffset, *(effect2.globalBrightnessPointer)); };
        else if (effectSelection == 3) effect2.effectFunction = [](int ledIndex) { return starFieldEffect(frameTimeDelta, 16, 80, ledIndex, effect2.currentPaletteOffset, *(effect2.globalBrightnessPointer)); };
        else if (effectSelection == 4) effect2.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect2.time1, effect2.time2, ledIndex, *(effect2.transformMap2), *(effect2.transformMap1), effect2.currentPaletteOffset, *(effect2.globalBrightnessPointer)); };
        else if (effectSelection == 5) effect2.effectFunction = [](int ledIndex) { return rainShowerEffect(frameTimeDelta, effect2.time1, ledIndex, effect2.size, normalTransformMapY, normalTransformMapX, effect2.currentPaletteOffset, *(effect2.globalBrightnessPointer)); };
        else if (effectSelection == 6) effect2.effectFunction = [](int ledIndex) { return expandingColorOrbEffect(frameTimeDelta, ledIndex, effect2.currentPaletteOffset, *(effect2.globalBrightnessPointer)); };
        else if (effectSelection == 7) effect2.effectFunction = [](int ledIndex) { return lightningBugEffect(frameTimeDelta, ledIndex, effect2.currentPaletteOffset); };
        else if (effectSelection == 8) effect2.effectFunction = [](int ledIndex) { return fireEffect(frameTimeDelta, ledIndex, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel, normalTransformMapX, normalTransformMapY, effect2.currentPaletteOffset, *(effect2.globalBrightnessPointer)); };
        else if (effectSelection == 9) effect2.effectFunction = [](int ledIndex) { return simpleSolidColorFillEffect(effect2.time1, effect2.currentPaletteOffset, *(effect2.globalBrightnessPointer)); };
        else if (effectSelection == 10) effect2.effectFunction = [](int ledIndex) { return fireworksEffect(frameTimeDelta, effect2.time2, ledIndex, 40, effect2.currentPaletteOffset, 65535); };
        else if (effectSelection == 11) effect2.effectFunction = [](int ledIndex) { return lightChaseEffect(effect2.time2, ledIndex, effect2.time1, effect2.currentPaletteOffset, 6, 65535); };
        else effect2.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect2.time1, effect2.time2, ledIndex, *(effect2.transformMap1), *(effect2.transformMap2), effect2.currentPaletteOffset, *(effect2.globalBrightnessPointer)); };
        if (effectSelection == 0) effect4.effectFunction = [](int ledIndex) { return meteorRainEffect2(frameTimeDelta, effect4.time1, effect4.time2, effect4.time2 >> 3, ledIndex, effect4.size, .12, normalTransformMapX, normalTransformMapY, effect4.currentPaletteOffset, *(effect4.globalBrightnessPointer)); };
        else if (effectSelection == 1) effect4.effectFunction = [](int ledIndex) { return meteorRainEffect(frameTimeDelta, effect4.time1,effect4.time2, ledIndex, effect4.size, .12, *(effect4.transformMap2), effect4.currentPaletteOffset, *(effect4.globalBrightnessPointer)); };
        else if (effectSelection == 2) effect4.effectFunction = [](int ledIndex) { return meteorRainEffect2(frameTimeDelta, effect4.time1, effect4.time2, 0, ledIndex, effect4.size, .12, *(effect4.transformMap1), *(effect4.transformMap1), effect4.currentPaletteOffset, *(effect4.globalBrightnessPointer)); };
        else if (effectSelection == 3) effect4.effectFunction = [](int ledIndex) { return starFieldEffect(frameTimeDelta, 16, 80, ledIndex, effect4.currentPaletteOffset, *(effect4.globalBrightnessPointer)); };
        else if (effectSelection == 4) effect4.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect4.time1, effect4.time2, ledIndex, *(effect4.transformMap2), *(effect4.transformMap1), effect4.currentPaletteOffset, *(effect4.globalBrightnessPointer)); };
        else if (effectSelection == 5) effect4.effectFunction = [](int ledIndex) { return rainShowerEffect(frameTimeDelta, effect4.time1, ledIndex, effect4.size, normalTransformMapY, normalTransformMapX, effect4.currentPaletteOffset, *(effect4.globalBrightnessPointer)); };
        else if (effectSelection == 6) effect4.effectFunction = [](int ledIndex) { return expandingColorOrbEffect(frameTimeDelta, ledIndex, effect4.currentPaletteOffset, *(effect4.globalBrightnessPointer)); };
        else if (effectSelection == 7) effect4.effectFunction = [](int ledIndex) { return lightningBugEffect(frameTimeDelta, ledIndex, effect4.currentPaletteOffset); };
        else if (effectSelection == 8) effect4.effectFunction = [](int ledIndex) { return fireEffect(frameTimeDelta, ledIndex, 100 - (currentAudioIntensityLevel * 100), 500 * currentAudioIntensityLevel * currentAudioIntensityLevel, normalTransformMapX, normalTransformMapY, effect4.currentPaletteOffset, *(effect4.globalBrightnessPointer)); };
        else if (effectSelection == 9) effect4.effectFunction = [](int ledIndex) { return simpleSolidColorFillEffect(effect4.time1, effect4.currentPaletteOffset, *(effect4.globalBrightnessPointer)); };
        else if (effectSelection == 10) effect4.effectFunction = [](int ledIndex) { return fireworksEffect(frameTimeDelta, effect3.time2, ledIndex, 40, effect4.currentPaletteOffset, 65535); };
        else if (effectSelection == 11) effect4.effectFunction = [](int ledIndex) { return lightChaseEffect(effect4.time2, ledIndex, effect4.time1, effect4.currentPaletteOffset, 10, 65535); };
        else effect4.effectFunction = [](int ledIndex) { return gradientWaveEffect(effect4.time1, effect4.time2, ledIndex, *(effect4.transformMap1), *(effect4.transformMap2), effect4.currentPaletteOffset, *(effect4.globalBrightnessPointer)); };
    }
}

void pickRandomSizeParametersForEffects()
{
    effect1.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
    effect2.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
    effect3.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
    effect4.size = fastRandomInteger(effectSettings.MinimumEffectSize, effectSettings.MaximumEffectSize);
}

void pickRandomGlobalBrightnessControlModes()
{
    pickRandomGlobalBrightnessControlModesForEffect(&effect1, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange);
    pickRandomGlobalBrightnessControlModesForEffect(&effect2, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange);
    pickRandomGlobalBrightnessControlModesForEffect(&effect3, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange);
    pickRandomGlobalBrightnessControlModesForEffect(&effect4, effectSettings.LikelihoodIndividualGlobalBrightnessModesChange);
}

void pickRandomGlobalBrightnessControlModesForEffect(Effect *effect, byte likelihood)
{
    if (fastRandomByte() > likelihood) return;
    if ((fastRandomByte() < effectSettings.LikelihoodMovementBasedBrightnessModeIsPicked) && (getCurrentMovementType() != Stationary)) effect->globalBrightnessPointer = movementBasedBrightnessModes[fastRandomInteger(NumberOfMovementBasedGlobalBrightnessChangeModes)];
    else if (fastRandomByte() < effectSettings.LikelihoodMusicBasedBrightnessModeIsPicked) effect->globalBrightnessPointer = musicBasedBrightnessModes[fastRandomInteger(NumberOfMusicBasedGlobalBrightnessChangeModes)];
    else effect->globalBrightnessPointer = normalBrightnessModes[fastRandomInteger(NumberOfNormalGlobalBrightnessChangeModes)];
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