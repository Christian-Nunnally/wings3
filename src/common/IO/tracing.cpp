#ifdef RP2040
#include "../IO/serial.h"
#else
#include <stdint.h>
#include <iostream>
#endif

#include <queue>
#include "../commonHeaders.h"
#include "../IO/tracing.h"
#include "../IO/standardOutputCommandOperationCodes.h"

std::queue<uint16_t> integerMetricNameIdsQueue;
std::queue<int> integerMetricsQueue;

std::queue<uint16_t> floatMetricNameIdsQueue;
std::queue<float> floatMetricsQueue;

#ifdef RP2040
inline void printOpCodeAndKeyRP2040(uint16_t metricNameId)
{
    D_serialWrite(STANDARD_OUTPUT_OPERATION_CODE_SET_METRIC);
    D_serialWrite(",");
    D_serialWrite(metricNameId);
    D_serialWrite(",");  
}
#else
inline void printOpCodeAndKeyWindows(uint16_t metricNameId)
{
    std::cout << STANDARD_OUTPUT_OPERATION_CODE_SET_METRIC;
    std::cout << ",";
    std::cout << metricNameId;
    std::cout << ",";
}
#endif

void emitMetric(uint16_t metricNameId, int value)
{
    if (metricNameId != METRIC_NAME_ID_PROFILE_TIME) return;
    integerMetricNameIdsQueue.push(metricNameId);
    integerMetricsQueue.push(value);
}

void emitMetric(uint16_t metricNameId, float value)
{
    if (metricNameId != METRIC_NAME_ID_PROFILE_TIME) return;
    floatMetricNameIdsQueue.push(metricNameId);
    floatMetricsQueue.push(value);
}

void processMetrics()
{
    while (!integerMetricsQueue.empty()) {
        #ifdef RP2040
        printOpCodeAndKeyRP2040(integerMetricNameIdsQueue.front());
        D_serialWrite(integerMetricsQueue.front());
        D_serialWrite("\n");
        #else
        printOpCodeAndKeyWindows(integerMetricNameIdsQueue.front());
        std::cout << integerMetricsQueue.front();
        std::cout << "\n";
        #endif
        integerMetricNameIdsQueue.pop();
        integerMetricsQueue.pop();
    }

    while (!floatMetricsQueue.empty()) {
        #ifdef RP2040
        printOpCodeAndKeyRP2040(floatMetricNameIdsQueue.front());
        D_serialWrite(floatMetricsQueue.front());
        D_serialWrite("\n");
        #else
        printOpCodeAndKeyWindows(floatMetricNameIdsQueue.front());
        std::cout << floatMetricsQueue.front();
        std::cout << "\n";
        #endif
        floatMetricNameIdsQueue.pop();
        floatMetricsQueue.pop();
    }
}