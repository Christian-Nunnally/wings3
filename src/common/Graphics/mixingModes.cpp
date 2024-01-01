
#include "../settings.h"
#include "../Graphics/mixingModes.h"
#include "../Graphics/colorMixing.h"
#include "../Utility/fastRandom.h"
#include "../IO/tracing.h"

const int MaxNumberOfMixingModeBlendFunctions = 20;
int numberOfMixingModeBlendFunctions;
Color (*mixingModeBlendFunctions[MaxNumberOfMixingModeBlendFunctions])(Color color1, Color color2, uint8_t transitionAmount);
Color (*mixingModeBlendFunction)(Color color1, Color color2, uint8_t transitionAmount) {};
Color (*oldMixingModeBlendFunction)(Color color1, Color color2, uint8_t transitionAmount) {};
int mixingMode;
int millisecondsLeftInMixingModeBlend;
int millisecondsLeftInMixingModeBlendTotalDuration = 1;
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
    if (!millisecondsLeftInMixingModeBlend) return newColor;
    Color oldColor = oldMixingModeBlendFunction(color1, color2, percentOfEffectBToShow);
    return blendColorsUsingMixing(newColor, oldColor, percentOfOldMixingModeToMixIn8Bit);
}

void incrementMixingModeBlend(int frameTimeDelta)
{
    if (!millisecondsLeftInMixingModeBlend) return;
    millisecondsLeftInMixingModeBlend = millisecondsLeftInMixingModeBlend > frameTimeDelta ? millisecondsLeftInMixingModeBlend - frameTimeDelta : 0;
    percentOfOldMixingModeToMixIn8Bit = (millisecondsLeftInMixingModeBlend / millisecondsLeftInMixingModeBlendTotalDuration) * UINT8_MAX;
    D_emitIntegerMetric(METRIC_NAME_ID_MILLISECONDS_LEFT_IN_MIXING_MODE_BLEND, millisecondsLeftInMixingModeBlend);
    D_emitIntegerMetric(METRIC_NAME_ID_PERCENT_OF_OLD_MIXING_MODE, percentOfOldMixingModeToMixIn8Bit);
}

void pickRandomMixingMode(int minimumMillisecondsForTransition, int maximumMillisecondsForTransition)
{
    int mixingModeBlendFunctionIndex = fastRandomInteger(numberOfMixingModeBlendFunctions);
    mixingModeBlendFunction = mixingModeBlendFunctions[mixingModeBlendFunctionIndex];
    millisecondsLeftInMixingModeBlendTotalDuration = fastRandomInteger(minimumMillisecondsForTransition, maximumMillisecondsForTransition) + 1;
    millisecondsLeftInMixingModeBlend = millisecondsLeftInMixingModeBlendTotalDuration;
    D_emitIntegerMetric(METRIC_NAME_ID_MIXING_MODE_BLEND_FUNCTION, mixingModeBlendFunctionIndex);
    D_emitIntegerMetric(METRIC_NAME_ID_MILLISECONDS_LEFT_IN_MIXING_MODE_BLEND_TOTAL, millisecondsLeftInMixingModeBlendTotalDuration);
}