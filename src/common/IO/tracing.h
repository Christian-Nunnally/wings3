
#ifndef TRACING_H
#define TRACING_H

#include <string>
#include "../IO/standardOutputCommandOperationCodes.h"

#define METRIC_NAME_ID_X 0

#ifdef ENABLE_TRACING
#define D_emitMetricString(...)    emitMetricString(__VA_ARGS__)
#define D_emitIntegerMetric(...)   emitIntegerMetric(__VA_ARGS__)
#define D_emitDoubleMetric(...)   emitDoubleMetric(__VA_ARGS__)
#define D_emitFloatMetric(...)   emitFloatMetric(__VA_ARGS__)
#else
#define D_emitMetricString(...)
#define D_emitIntegerMetric(...)
#define D_emitDoubleMetric(...)
#define D_emitFloatMetric(...)
#endif

void emitMetricString(std::string key, std::string value);
void emitIntegerMetric(std::string key, int value);
void emitIntegerMetric(std::string key, uint8_t id, int value);
void emitDoubleMetric(std::string key, double value);
void emitFloatMetric(std::string key, float value);

#endif