#ifndef SAVED_EFFECT_H
#define SAVED_EFFECT_H

#include "../commonHeaders.h"

typedef struct
{
    int effectFunctionIncrementUniqueId;

    uint8_t transformMap1Index;
    uint8_t transformMap2Index;

    uint8_t brightnessControlMode;
    uint8_t brightnessControlIndex;

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
SavedEffect;

#endif