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

std::queue<uint16_t> stringMetricNameIdsQueue;
std::queue<std::string> stringMetricsQueue;

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

void emitMetric(uint16_t metricNameId, std::string value)
{
    stringMetricNameIdsQueue.push(metricNameId);
    stringMetricsQueue.push(value);
}

void emitMetric(uint16_t metricNameId, int value)
{
    integerMetricNameIdsQueue.push(metricNameId);
    integerMetricsQueue.push(value);
}

void emitMetric(uint16_t metricNameId, float value)
{
    floatMetricNameIdsQueue.push(metricNameId);
    floatMetricsQueue.push(value);
}

void processMetrics()
{
    for (int i = 0; i < 1; i++)
    {
        if (!stringMetricsQueue.empty()) {
            #ifdef RP2040
            printOpCodeAndKeyRP2040(stringMetricNameIdsQueue.front());
            D_serialWrite(stringMetricsQueue.front());
            D_serialWrite("\n");
            #else
            printOpCodeAndKeyWindows(stringMetricNameIdsQueue.front());
            std::cout << stringMetricsQueue.front();
            std::cout << "\n";
            #endif
            stringMetricNameIdsQueue.pop();
            stringMetricsQueue.pop();
        }

        if (!integerMetricsQueue.empty()) {
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

        if (!floatMetricsQueue.empty()) {
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

    while (!stringMetricsQueue.empty()) {
        stringMetricNameIdsQueue.pop();
        stringMetricsQueue.pop();
    }

    while (!integerMetricsQueue.empty()) {
        integerMetricNameIdsQueue.pop();
        integerMetricsQueue.pop();
    }

    while (!floatMetricsQueue.empty()) {
        floatMetricNameIdsQueue.pop();
        floatMetricsQueue.pop();
    }
}