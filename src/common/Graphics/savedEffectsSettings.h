#ifndef SAVED_EFFECT_SETTINGS_H
#define SAVED_EFFECT_SETTINGS_H

#include "../commonHeaders.h"
#include "../Graphics/palettes.h"
#include "../Graphics/effectSettings.h"
#include "../Graphics/savedEffect.h"

typedef struct
{
    SavedEffect savedEffectA1;
    SavedEffect savedEffectB1;
    SavedEffect savedEffectA2;
    SavedEffect savedEffectB2;
    EffectSettings effectSettings;
}
SavedEffectSettings;

#endif