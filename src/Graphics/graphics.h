#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Arduino.h>
#include "../Graphics/color.h"

Color wavePulse(uint32_t brightnessFrame, uint32_t colorFrame, int pixelIndex, const uint8_t brightnessProjectionMap[], const uint8_t colorProjectionMap[], int paletteOffset, uint16_t globalBrightnessModifier);
Color starShimmer(int frameDelta, int fadeAmount, uint16_t newStarLikelihood, int pixelIndex, int paletteOffset, uint16_t globalBrightnessModifier);
Color meteorRain(int frameDelta, uint32_t animationFrame, uint32_t colorFrame, int pixelIndex, byte meteorSize, float meteorTrailDecay, const uint8_t projectionMap[], int paletteOffset, uint16_t globalBrightnessModifier);
Color meteorRain2(int frameDelta, uint32_t projectionMap1Frame, uint32_t projectionMap2Frame, uint32_t colorFrame, int pixelIndex, byte meteorSize, float meteorTrailDecay, const uint8_t projectionMap[], const uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier);
Color rainShower(int frameDelta, uint32_t rainAnimationFrame, int pixelIndex, byte rainLength, const uint8_t projectionMap[], const uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier);
Color lightningBug(int frameDelta, int pixelIndex, int paletteOffset);
Color colorOrb(int frameDelta, int pixelIndex, int paletteOffset, uint16_t globalBrightnessModifier);
Color fireAnimation(int frameDelta, int pixelIndex, int flameDecay, int sparks, const uint8_t projectionMap[], const uint8_t projectionMap2[], int paletteOffset, uint16_t globalBrightnessModifier);
Color simpleColor(int colorFrame, int paletteOffset, uint16_t globalBrightnessModifier);
Color fireworks(int frameDelta, uint32_t colorFrame, int pixelIndex, int fireworkProbability, int paletteOffset, uint16_t globalBrightnessModifier);
Color lightChase(uint32_t ledTestAnimationFrame, int pixelIndex, uint32_t colorAnimationFrame, int paletteOffset, int mod, uint16_t globalBrightnessModifier);

void boostRainChance();

#endif