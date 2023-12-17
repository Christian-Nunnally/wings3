
#ifndef TRACING_H
#define TRACING_H

#include <string>

#ifdef ENABLE_TRACING
#define D_emitMetric(...)          emitMetric(__VA_ARGS__)
#define D_emitMetricString(...)    emitMetricString(__VA_ARGS__)
#define D_emitIntegerMetric(...)   emitIntegerMetric(__VA_ARGS__)
#define D_emitDoubleMetric(...)   emitDoubleMetric(__VA_ARGS__)
#define D_emitFloatMetric(...)   emitFloatMetric(__VA_ARGS__)
#else
#define D_emitMetric(...)
#define D_emitMetricString(...)
#define D_emitIntegerMetric(...)
#define D_emitDoubleMetric(...)
#define D_emitFloatMetric(...)
#endif

void emitMetric(char* key, char* value);
void emitMetricString(char* key, std::string value);
void emitIntegerMetric(char* key, int value);
void emitIntegerMetric(char* key, uint8_t id, int value);
void emitDoubleMetric(char* key, double value);
void emitFloatMetric(char* key, float value);

#endif