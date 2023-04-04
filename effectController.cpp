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
float audioInfluenceFactorForAudioScaledTime = 1;

float transition = 0.0;
int loopCount = 0;
int loopCountDirection = 1;
int lastPeakDetectorValue;
bool isMusicDetected = false;
int musicDetectionCountdown = MUSIC_DETECTION_COUNTDOWN;
int mixingMode;

void monitorAudioLevelsToToggleMusicDetection();
void incrementMusicDetectionToggle();
void incrementTime();
void incrementTransition();

Color getLedColorForFrame(int ledIndex)
{
    if (!isMusicDetected) return {0,0,0};
    if (ledIndex == 67 || ledIndex == 79 || ledIndex == 102 || ledIndex == 108) return {0,0,0};
    Color color1 = wavePulse(currentTime / 2, ledIndex, coordsY, coordsY, rainbowPalette, getAudioIntensityRatio() * 65535);
    Color color2;
    if (getAudioIntensityRatio() > .5)
    {
        color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, rainbowPalette, 65535);
        // color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, starter_palette2, getAudioIntensityRatio());
    }
    else 
    {
        color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, rainbowPalette, 65535);
        // color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, starter_palette2, .3);
    }
    if (getAudioIntensityRatio() > .98)
    {
        color1 = wavePulse(currentTime, ledIndex, top_radii, top_angles, rainbowPalette, 65535);
    }

    if (mixingMode == 0)
    {
        return blendColorsUsingMixingGlitched(color1, color2, transition);
    }
    if (mixingMode == 1)
    {
        return blendColorsUsingMixing(color1, color2, transition);
    }
    if (mixingMode == 2)
    {
        return blendColorsUsingAdd(color1, color2, transition);
    }
    if (mixingMode == 3)
    {
        return blendColorsUsingOverlay(color1, color2, transition);
    }
    if (mixingMode == 4)
    {
        return blendColorsUsingScreen(color1, color2, transition);
    }
    if (mixingMode == 5)
    {
        return blendColorsUsingAverage(color1, color2, transition);
    }
    return blendColorsUsingSubtract(color1, color2, transition);
}

// Color getLedColorForFrame(int ledIndex)
// {
//     if (!isMusicDetected) return {0,0,0};
//     if (ledIndex == 67 || ledIndex == 79 || ledIndex == 102 || ledIndex == 108) return {0,0,0};
//     Color color1 = wavePulse(currentTime, ledIndex, coordsY, coordsY, rainbowPalette, getAudioIntensityRatio() * 65535);
//     Color color2;
//     if (getAudioIntensityRatio() > .5)
//     {
//         color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, rainbowPalette, 65535);
//         // color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, starter_palette2, getAudioIntensityRatio());
//     }
//     else 
//     {
//         color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, rainbowPalette, 65535);
//         // color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, starter_palette2, .3);
//     }
//     if (getAudioIntensityRatio() > .98)
//     {
//         color1 = wavePulse(currentTime, ledIndex, top_radii, top_angles, rainbowPalette, 65535);
//     }
//     return blendColorsUsingMixing(color1, color2, transition);
// }

void incrementEffectFrame()
{
    incrementTime();
    incrementTransition();

    int nextPeak = getCurrentPeakDetectorValue();
    if (nextPeak != lastPeakDetectorValue)
    {
        lastPeakDetectorValue = nextPeak;
        mixingMode = (mixingMode + 1) % 7;
    }

    monitorAudioLevelsToToggleMusicDetection();
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
    Serial.println(loopCount);
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
            if (transition >70) transition = transition - 35;
            else { 
                loopCountDirection = 1;
                loopCount = 0;
            }
        }
    }    
}