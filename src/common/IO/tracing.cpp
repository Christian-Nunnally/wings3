#ifdef RP2040
#include "../IO/serial.h"
#else
#include <stdint.h>
#include <iostream>
#endif

#include "../IO/tracing.h"
#include "../IO/standardOutputCommandOperationCodes.h"

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
    #ifdef RP2040
    printOpCodeAndKeyRP2040(metricNameId);
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(metricNameId);
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitMetricString(uint16_t metricNameId, std::string value)
{
    #ifdef RP2040
    printOpCodeAndKeyRP2040(metricNameId);
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(metricNameId);
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitIntegerMetric(uint16_t metricNameId, int value)
{
    #ifdef RP2040
    printOpCodeAndKeyRP2040(metricNameId);
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(metricNameId);
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitIntegerMetric(uint16_t metricNameId, uint8_t id, int value)
{
    #ifdef RP2040
    printOpCodeAndKeyRP2040(metricNameId);
    D_serialWrite(id);
    D_serialWrite(",");
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(metricNameId);
    std::cout << id;
    std::cout << ",";
    std::cout << (int)value;
    std::cout << "\n";
    #endif
}

void emitDoubleMetric(uint16_t metricNameId, double value)
{
    #ifdef RP2040
    printOpCodeAndKeyRP2040(metricNameId);
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(metricNameId);
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitFloatMetric(uint16_t metricNameId, float value)
{
    #ifdef RP2040
    printOpCodeAndKeyRP2040(metricNameId);
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(metricNameId);
    std::cout << value;
    std::cout << "\n";
    #endif
}