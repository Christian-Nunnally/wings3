
#include "../settings.h"
#include "../Graphics/mixingModes.h"
#include "../Graphics/colorMixing.h"
#include "../Graphics/effectSettings.h"
#include "../Utility/fastRandom.h"
#include "../IO/tracing.h"

const int MaxNumberOfMixingModeBlendFunctions = 20;
int numberOfMixingModeBlendFunctions;
Color (*mixingModeBlendFunctions[MaxNumberOfMixingModeBlendFunctions])(Color color1, Color color2, uint8_t transitionAmount);
Color (*mixingModeBlendFunction)(Color color1, Color color2, uint8_t transitionAmount) {};
Color (*oldMixingModeBlendFunction)(Color color1, Color color2, uint8_t transitionAmount) {};
uint8_t percentOfOldMixingModeToMixIn8Bit;

void setupMixingModes()
{
    mixingModeBlendFunction = blendColorsUsingMixing;
    oldMixingModeBlendFunction = blendColorsUsingMixing;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixing;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAdd;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingScreen;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAverage;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixingGlitched;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixing;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAdd;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingScreen;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAverage;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingShimmer;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixingGlitched;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingMixing;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAdd;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingScreen;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingAverage;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingShimmer;
    mixingModeBlendFunctions[numberOfMixingModeBlendFunctions++] = blendColorsUsingOverlay;
}

Color blendIncorporatingOldMixingMode(Color color1, Color color2, uint8_t percentOfEffectBToShow)
{
    Color newColor = mixingModeBlendFunction(color1, color2, percentOfEffectBToShow);
    if (!effectSettings.millisecondsLeftInMixingModeBlend) return newColor;
    Color oldColor = oldMixingModeBlendFunction(color1, color2, percentOfEffectBToShow);
    return blendColorsUsingMixing(newColor, oldColor, percentOfOldMixingModeToMixIn8Bit);
}

void incrementMixingModeBlend(int frameTimeDelta)
{
    if (!effectSettings.millisecondsLeftInMixingModeBlend) return;
    effectSettings.millisecondsLeftInMixingModeBlend = effectSettings.millisecondsLeftInMixingModeBlend > frameTimeDelta ? effectSettings.millisecondsLeftInMixingModeBlend - frameTimeDelta : 0;
    percentOfOldMixingModeToMixIn8Bit = (effectSettings.millisecondsLeftInMixingModeBlend / effectSettings.desiredDurationOfMixingModeBlendInMilliseconds) * UINT8_MAX;
    D_emitIntegerMetric(METRIC_NAME_ID_MILLISECONDS_LEFT_IN_MIXING_MODE_BLEND, effectSettings.millisecondsLeftInMixingModeBlend);
    D_emitIntegerMetric(METRIC_NAME_ID_PERCENT_OF_OLD_MIXING_MODE, percentOfOldMixingModeToMixIn8Bit);
}

void pickRandomMixingMode(int minimumMillisecondsForTransition, int maximumMillisecondsForTransition)
{
    effectSettings.oldMixingModeBlendFunctionIndex = effectSettings.mixingModeBlendFunctionIndex;
    effectSettings.mixingModeBlendFunctionIndex = fastRandomInteger(numberOfMixingModeBlendFunctions);
    setMixingModeBlendFunctionFromIndex();
    effectSettings.desiredDurationOfMixingModeBlendInMilliseconds = fastRandomInteger(minimumMillisecondsForTransition, maximumMillisecondsForTransition) + 1;
    effectSettings.millisecondsLeftInMixingModeBlend = effectSettings.desiredDurationOfMixingModeBlendInMilliseconds;
    D_emitIntegerMetric(METRIC_NAME_ID_MIXING_MODE_BLEND_FUNCTION, effectSettings.mixingModeBlendFunctionIndex);
    D_emitIntegerMetric(METRIC_NAME_ID_MILLISECONDS_LEFT_IN_MIXING_MODE_BLEND_TOTAL, effectSettings.desiredDurationOfMixingModeBlendInMilliseconds);
}

void setMixingModeBlendFunctionFromIndex()
{
    if (effectSettings.mixingModeBlendFunctionIndex >= numberOfMixingModeBlendFunctions) return;
    mixingModeBlendFunction = mixingModeBlendFunctions[effectSettings.mixingModeBlendFunctionIndex];
    if (effectSettings.oldMixingModeBlendFunctionIndex >= numberOfMixingModeBlendFunctions) return;
    oldMixingModeBlendFunction = mixingModeBlendFunctions[effectSettings.oldMixingModeBlendFunctionIndex];
}