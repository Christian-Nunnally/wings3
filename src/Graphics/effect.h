#ifndef EFFECT_H
#define EFFECT_H

#include <Arduino.h>
#include "../Graphics/color.h"

typedef struct
{
    Color (*effectFunction)(int pixel) {};
    Color (*effectFunctionHighlight)(int pixel) {};

    uint8_t const *transformMap1[272];
    uint8_t const *transformMap2[272];
}
Effect;

#endif