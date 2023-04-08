#include <Adafruit_NeoPXL8.h>
#include "effectController.h"
#include "settings.h"
#include "graphics.h"
#include "microphone.h"
#include "movementDetection.h"
#include "transformMaps.h"
#include "palettes.h"

#define MUSIC_DETECTION_COUNTDOWN 3000
#define MUSIC_DETECTION_RMS_THRESHOLD 800

int currentTime;
int audioScaledTime;
int audioPulsedTime;
float audioInfluenceFactorForAudioScaledTime = 1;

int transition = 0.0;
int loopCount = 0;
int loopCountDirection = 1;
int lastPeakDetectorValue;
bool isMusicDetected = false;
int musicDetectionCountdown = MUSIC_DETECTION_COUNTDOWN;
int mixingMode;
int oldMixingMode;
int mixingModeBlendCounter;
float mixingModeBlendCounterMax = 300.0;

int switchMapBlendCounter;
float switchMapBlendCounterMax = 300.0;

bool switchMap = false;

void monitorAudioLevelsToToggleMusicDetection();
void incrementMusicDetectionToggle();
void incrementTime();
void incrementTransition();
Color blendIncorporatingOldMixingMode(Color color1, Color color2);
Color blendColorsUsingMixingMode(int mixingMode, Color color1, Color color2, uint16_t transitionAmount);

Color (*effect1)(int pixel) {};
Color (*effect1Plus)(int pixel) {};
Color (*effect2)(int pixel) {};
Color (*effect2Plus)(int pixel) {};
Color (*effect3)(int pixel) {};
Color (*effect3Plus)(int pixel) {};
Color (*effect4)(int pixel) {};
Color (*effect4Plus)(int pixel) {};

uint8_t *effect1TransformMap1[272];
uint8_t *effect1TransformMap2[272];
uint8_t *effect2TransformMap1[272];
uint8_t *effect2TransformMap2[272];
uint8_t *effect3TransformMap1[272];
uint8_t *effect3TransformMap2[272];
uint8_t *effect4TransformMap1[272];
uint8_t *effect4TransformMap2[272];

Color getLedColorForFrame(int ledIndex)
{
    if (!isMusicDetected) return {0,0,0};
    if (switchMap)
    {
        Color color1 = getAudioIntensityRatio() < .98 ? effect1(ledIndex) : effect1Plus(ledIndex);
        Color color2 = getAudioIntensityRatio() < .5 ? effect2(ledIndex) : effect2Plus(ledIndex);
        Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2);
        if (switchMapBlendCounter)
        {
            Color color3 = getAudioIntensityRatio() < .98 ? effect3(ledIndex) : effect4Plus(ledIndex);
            Color color4 = getAudioIntensityRatio() < .5 ? effect4(ledIndex) : effect4Plus(ledIndex);
            Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4);
            return blendColorsUsingMixing(resultColor1, resultColor2, (switchMapBlendCounter / switchMapBlendCounterMax) * 65535);
        }
        return resultColor1;
    }
    else 
    {
        Color color1 = getAudioIntensityRatio() < .98 ? effect3(ledIndex) : effect4Plus(ledIndex);
        Color color2 = getAudioIntensityRatio() < .5 ? effect4(ledIndex) : effect4Plus(ledIndex);
        Color resultColor1 = blendIncorporatingOldMixingMode(color1, color2);
        if (switchMapBlendCounter)
        {
            Color color3 = getAudioIntensityRatio() < .98 ? effect3(ledIndex) : effect4Plus(ledIndex);
            Color color4 = getAudioIntensityRatio() < .5 ? effect4(ledIndex) : effect4Plus(ledIndex);
            Color resultColor2 = blendIncorporatingOldMixingMode(color3, color4);
            return blendColorsUsingMixing(resultColor1, resultColor2, (switchMapBlendCounter / switchMapBlendCounterMax) * 65535);
        }
        return resultColor1;
        return blendIncorporatingOldMixingMode(color1, color2);
    }
}

inline Color blendIncorporatingOldMixingMode(Color color1, Color color2)
{
    Color newColor = blendColorsUsingMixingMode(mixingMode, color1, color2, transition);
    if (oldMixingMode != mixingMode && mixingModeBlendCounter)
    {
        Color oldColor = blendColorsUsingMixingMode(oldMixingMode, color1, color2, transition);
        return blendColorsUsingMixing(newColor, oldColor, (mixingModeBlendCounter / mixingModeBlendCounterMax) * 65535);
    }
    return newColor;
}

inline Color blendColorsUsingMixingMode(int mixingMode, Color color1, Color color2, uint16_t transitionAmount)
{
    return blendColorsUsingMixing(color1, color2, transition);
    if (mixingMode == 0) return blendColorsUsingMixingGlitched(color1, color2, transition);//16ms
    if (mixingMode == 1) return blendColorsUsingMixing(color1, color2, transition); // 20ms
    if (mixingMode == 2) return blendColorsUsingAdd(color1, color2, transition); // 11ms
    if (mixingMode == 3) return blendColorsUsingOverlay(color1, color2, transition); // 12ms
    if (mixingMode == 4) return blendColorsUsingScreen(color1, color2, transition); // 12ms
    if (mixingMode == 5) return blendColorsUsingAverage(color1, color2, transition); // 11.5ms
    return blendColorsUsingSubtract(color1, color2, transition); // 11ms
}

void incrementEffectFrame()
{
    incrementTime();
    incrementTransition();

    int nextPeak = getCurrentPeakDetectorValue();
    if (nextPeak != lastPeakDetectorValue)
    {
        lastPeakDetectorValue = nextPeak;
        if (random(2) == 1) {
            mixingMode = random(7);
            Serial.print("Mixing mode: ");
            Serial.println(mixingMode);
            mixingModeBlendCounterMax = random(500);
            mixingModeBlendCounter = mixingModeBlendCounterMax;
        }
        if (random(100) > 10)
        {
            switchMap = !switchMap;
            if (switchMap)
            {
                if (random(4) == 1) *effect1TransformMap1 = transformMaps[random(transformMapsCount)];
                if (random(4) == 1) *effect1TransformMap2 = transformMaps[random(transformMapsCount)];
                if (random(4) == 1) *effect2TransformMap1 = transformMaps[random(transformMapsCount)];
                if (random(4) == 1) *effect2TransformMap2 = transformMaps[random(transformMapsCount)];
            }
            else
            {
                if (random(4) == 1) *effect3TransformMap1 = transformMaps[random(transformMapsCount)];
                if (random(4) == 1) *effect3TransformMap2 = transformMaps[random(transformMapsCount)];
                if (random(4) == 1) *effect4TransformMap1 = transformMaps[random(transformMapsCount)];
                if (random(4) == 1) *effect4TransformMap2 = transformMaps[random(transformMapsCount)];
            }
            switchMapBlendCounterMax = random(500) + 10;
            switchMapBlendCounter = switchMapBlendCounterMax;
        }
    }

    monitorAudioLevelsToToggleMusicDetection();
}

void setupEffects()
{
    effect1 = [](int ledIndex) { return wavePulse(currentTime / 2, audioScaledTime, ledIndex, *effect1TransformMap1, *effect1TransformMap2, rainbowPalette, getAudioIntensityRatio() * 65535); };
    effect1Plus = [](int ledIndex) { return wavePulse(currentTime, audioScaledTime, ledIndex, *effect1TransformMap1, *effect1TransformMap2, rainbowPalette, 65535); };
    effect2 = [](int ledIndex) { return wavePulse(audioScaledTime, audioScaledTime, ledIndex, *effect2TransformMap1, *effect2TransformMap2, rainbowPalette, 65535); };
    effect2Plus = [](int ledIndex) { return wavePulse(currentTime / 2, audioScaledTime, ledIndex, *effect2TransformMap1, *effect2TransformMap2, rainbowPalette, 65535); };
    effect3 = [](int ledIndex) { return wavePulse(audioScaledTime, audioScaledTime, ledIndex, *effect3TransformMap1, *effect3TransformMap2, rainbowPalette, 65535); };
    effect3Plus = [](int ledIndex) { return wavePulse(audioScaledTime, audioScaledTime, ledIndex, *effect3TransformMap1, *effect3TransformMap2, rainbowPalette, 65535); };
    effect4 = [](int ledIndex) { return wavePulse(audioScaledTime, audioScaledTime, ledIndex, *effect4TransformMap1, *effect4TransformMap2, rainbowPalette, 65535); };
    effect4Plus = [](int ledIndex) { return wavePulse(audioScaledTime, audioScaledTime, ledIndex, *effect4TransformMap1, *effect4TransformMap2, rainbowPalette, 65535); };

    *effect1TransformMap1 = normalTransformMapX;
    *effect1TransformMap2 = normalTransformMapY;
}

inline void monitorAudioLevelsToToggleMusicDetection()
{
    if (isMusicDetected)
    {
        if (getCurrentPeakRootMeanSquare() < MUSIC_DETECTION_RMS_THRESHOLD) incrementMusicDetectionToggle();
    }
    else 
    {
        if (getCurrentPeakRootMeanSquare() > MUSIC_DETECTION_RMS_THRESHOLD) incrementMusicDetectionToggle();
    }
    if (musicDetectionCountdown < MUSIC_DETECTION_COUNTDOWN) musicDetectionCountdown++;
}

inline void incrementMusicDetectionToggle()
{
    musicDetectionCountdown -= 2;
    if (musicDetectionCountdown <= 2)
    {
        isMusicDetected = !isMusicDetected;
        musicDetectionCountdown = MUSIC_DETECTION_COUNTDOWN;
    }
}

inline void incrementTime()
{
    int lastTime = currentTime;
    currentTime = millis();
    int timeDelta = currentTime - lastTime;
    audioScaledTime += getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta;
}

inline void incrementTransition()
{
    //Serial.println(loopCount);
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
    else oldMixingMode = mixingMode;  
}