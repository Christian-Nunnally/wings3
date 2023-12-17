
#ifndef TRACING_H
#define TRACING_H

#ifdef ENABLE_TRACING
#define D_emitMetric(...)          emitMetric(__VA_ARGS__)
#define D_emitMetricString(...)    emitMetricString(__VA_ARGS__)
#define D_emitIntegerMetric(...)   emitIntegerMetric(__VA_ARGS__)
#else
#define D_emitMetric(...)
#define D_emitMetricString(...)
#define D_emitIntegerMetric(...)
#endif

void emitMetric(char* key, char* value);
void emitMetricString(char* key, std::string value);
void emitIntegerMetric(char* key, int value);

#endif