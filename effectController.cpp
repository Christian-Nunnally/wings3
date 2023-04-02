#include <Adafruit_NeoPXL8.h>
#include "effectController.h"
#include "settings.h"
#include "graphics.h"
#include "microphone.h"
#include "movementDetection.h"
#include "transformMaps.h"
#include "palettes.h"

int currentTime;
int audioScaledTime;
float audioInfluenceFactorForAudioScaledTime = 1;

float transition = 0.0;
int loopCount = 0;
int loopCountDirection = 1;
int lastPeakDetectorValue;
bool isMusicDetected = false;
int musicDetectionEndCountdown = 3000;

Color16 getLedColorForFrame(int ledIndex)
{
    if (!isMusicDetected) return {0,0,0};
    if (ledIndex == 67 || ledIndex == 79 || ledIndex == 102 || ledIndex == 108) return {0,0,0};
    Color16 color1 = wavePulse(currentTime, ledIndex, coordsY, coordsY, rainbow_palette2, getAudioIntensityRatio());
    Color16 color2;
    if (getAudioIntensityRatio() > .5)
    {
        color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, starter_palette2, 1);
        // color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, starter_palette2, getAudioIntensityRatio());
    }
    else 
    {
        color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, starter_palette2, 1);
        // color2 = wavePulse(audioScaledTime, ledIndex, top_radii, top_angles, starter_palette2, .3);
    }
    if (getAudioIntensityRatio() > .98)
    {
        color1 = wavePulse(currentTime, ledIndex, top_radii, top_angles, starter_palette2, 1);
    }
    
    return blendColorsUsingMixing(color1, color2, transition);
}

void incrementEffectFrame()
{
    int lastTime = currentTime;
    currentTime = millis();
    int timeDelta = currentTime - lastTime;
    audioScaledTime += getAudioIntensityRatio() * audioInfluenceFactorForAudioScaledTime * timeDelta;

    loopCount += 1;
    if (loopCount > 1500)
    {
        if (loopCountDirection == 1)
        {
            if (transition < 1) transition = transition + 0.001;
            else {
                loopCountDirection = -1;
                loopCount = 0;
            }
        }
        else 
        {
            if (transition > 0.0) transition = transition - 0.001;
            else { 
                loopCountDirection = 1;
                loopCount = 0;
            }
        }
    }

    int nextPeak = getCurrentPeakDetectorValue();
    if (nextPeak != lastPeakDetectorValue)
    {
        lastPeakDetectorValue = nextPeak;
    }

    if (getCurrentPeakRootMeanSquare() < 800)
    {
        musicDetectionEndCountdown--;
        if (musicDetectionEndCountdown <= 0)
        {
            isMusicDetected = false;
        }
    }
    else 
    {
        isMusicDetected = true;
        musicDetectionEndCountdown = 3000;
    }
}