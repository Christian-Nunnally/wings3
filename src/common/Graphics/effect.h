#ifndef EFFECT_H
#define EFFECT_H

#include "../settings.h"
#include "../commonHeaders.h"
#include "../Graphics/color.h"

typedef struct
{
    Color (*effectFunction)(int pixel) {};
    Color (*effectFunctionHighlight)(int pixel) {};
    void (*effectFunctionIncrement)() {};
    void (*effectFunctionBonusTrigger)() {};
    int effectFunctionIncrementUniqueId;

    uint8_t const *transformMap1[TOTAL_LEDS];
    uint8_t const *transformMap2[TOTAL_LEDS];

    uint8_t *globalBrightnessPointer;

    int *frameTimeDelta;
    int time1;
    int time2;

    uint8_t timeMode1;
    uint8_t timeMode2;

    int size;

    int currentPalette;
    int currentPaletteOffset; 
    int currentPaletteOffsetTarget; 

    uint8_t effectId;
}
Effect;

void initializeEffect(Effect *effect, int *frameTimeDeltaPointer, Color (*defaultEffect)(int), uint8_t effectId);

#endif