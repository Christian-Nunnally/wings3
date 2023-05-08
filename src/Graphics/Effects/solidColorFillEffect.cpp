
#include "../../Graphics/Effects/solidColorFillEffect.h"
#include "../../Graphics/palettes.h"

Color simpleSolidColorFillEffect(int colorFrame, int paletteOffset, uint16_t globalBrightnessModifier)
{
    uint8_t colorFrame8bit = (colorFrame >> 4);
    return colorFromPalette(colorFrame8bit + paletteOffset, globalBrightnessModifier);
}