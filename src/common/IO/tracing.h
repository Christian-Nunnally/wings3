
#ifndef TRACING_H
#define TRACING_H

#include <string>
#include "../IO/standardOutputCommandOperationCodes.h"

#define METRIC_NAME_ID_UNKNOWN 0
#define METRIC_NAME_ID_USING_MIRRORED_TRANSFORM_MAPS_1 1 // usingMirroredTransformMaps1
#define METRIC_NAME_ID_USING_MIRRORED_TRANSFORM_MAPS_2 2 // usingMirroredTransformMaps2
#define METRIC_NAME_ID_PRIMARY_EFFECT_TOGGLE 3 // primaryEffectToggle
#define METRIC_NAME_ID_REMOTE_OP_CODE 4 // remoteOpCode
#define METRIC_NAME_ID_RENDER_TIME 5 // renderTime
#define METRIC_NAME_ID_AUDIO_THRESHOLD_TO_SHOW_INTENSE_EFFECT_A1 6 // fxA1AudioLevelThresholdToShowMoreIntenseEffect
#define METRIC_NAME_ID_AUDIO_THRESHOLD_TO_SHOW_INTENSE_EFFECT_B1 7 // fxB1AudioLevelThresholdToShowMoreIntenseEffect
#define METRIC_NAME_ID_TRANSITION_DIRECTION 8 // transitionDirection
#define METRIC_NAME_ID_DESIRED_MILLISECOND_TRANSITION_DIRECTION 9 // desiredMillisecondTransitionDuration
#define METRIC_NAME_ID_EFFECT_BRIGHTNESS_MODE 10 // fxBrightnessMode
#define METRIC_NAME_ID_EFFECT_SIZE 11 // fxSize
#define METRIC_NAME_ID_EXPONENTIAL_MOVING_AVERAGE 12 // exponentialMovingAverage
#define METRIC_NAME_ID_ROOT_MEAN_SQUARE 13 // currentRootMeanSquare
#define METRIC_NAME_ID_AUDIO_INTENSITY_RATIO 14 // currentAudioIntensityLevel
#define METRIC_NAME_ID_EFFECT_TIME_1 15 // fxTime1
#define METRIC_NAME_ID_EFFECT_TIME_2 16 // fxTime2

#define METRIC_NAME_ID_EFFECT_TIME_MODE_1 17 // fxTimeMode1
#define METRIC_NAME_ID_EFFECT_TIME_MODE_2 18 // fxTimeMode2
#define METRIC_NAME_ID_PERCENT_OF_OLD_MIXING_MODE 19 // percentOfOldMixingModeToMixIn8Bit
#define METRIC_NAME_ID_MILLISECONDS_LEFT_IN_MIXING_MODE_BLEND 20 // millisecondsLeftInMixingModeBlend
#define METRIC_NAME_ID_MIXING_MODE_BLEND_FUNCTION 21 // mixingModeBlendFunction
#define METRIC_NAME_ID_MILLISECONDS_LEFT_IN_MIXING_MODE_BLEND_TOTAL 22 // millisecondsLeftInMixingModeBlendTotalDuration
#define METRIC_NAME_ID_EFFECT_TRANSFORM_MAP_1 23 // fxTransformMap1
#define METRIC_NAME_ID_EFFECT_TRANSFORM_MAP_2 24 // remoteOpCode
#define METRIC_NAME_ID_TOTAL_FRAME_TIME 25 // totalFrameTime
#define METRIC_NAME_ID_PERCENT_OF_EFFECT_B_TO_SHOW 26 // percentOfEffectBToShow8Bit
#define METRIC_NAME_ID_MILLISECONDS_TO_TRANSITION_FROM_SECONDARY_TO_PRIMARY_EFFECT 27 // msFor2ndTo1stEffect
#define METRIC_NAME_ID_PERCENT_SECONDARY_EFFECT_TO_SHOW 28 // percentSecondaryEffectToShow
#define METRIC_NAME_ID_EFFECT_PALETTE_OFFSET 29 // fxPaletteOffset
#define METRIC_NAME_ID_PEAK_DETECTOR 30 // peakDetector
#define METRIC_NAME_ID_BEAT_COUNT 31 // beatCount
#define METRIC_NAME_ID_RANDOMIZATION_COUNT 32 // fxsRandomizedCount
#define METRIC_NAME_ID_CURRENT_SCREEN_MAP 33 // currentScreenMap
#define METRIC_NAME_ID_EFFECT_CURRENT_PALETTE_OFFSET_TARGET 34 // fxCurrentPaletteOffsetTarget
#define METRIC_NAME_ID_EFFECT_PALETTE 35 // fxPalette
#define METRIC_NAME_ID_PROFILE_TIME 36 // profileTime
#define METRIC_NAME_ID_FADE_SETTING_AMOUNT 37 // fadeSettingAmount
#define METRIC_NAME_ID_MISC_METRIC 38 // misc

#ifdef ENABLE_TRACING
#define D_emitMetric(...)    emitMetric(__VA_ARGS__)
#else
#define D_emitMetric(...)
#endif

void emitMetric(uint16_t metricNameId, std::string value);
void emitMetric(uint16_t metricNameId, int value);
void emitMetric(uint16_t metricNameId, float value);
void processMetrics();

#endif