
#include "../../Graphics/Effects/solidColorFillEffect.h"
#include "../../Graphics/palettes.h"

Color simpleSolidColorFillEffect(int pixelIndex, Effect *effect)
{
    uint8_t colorFrame8bit = (effect->time1 >> 5);
    return colorFromPalette(colorFrame8bit + effect->currentPaletteOffset, (*effect->globalBrightnessPointer));
}