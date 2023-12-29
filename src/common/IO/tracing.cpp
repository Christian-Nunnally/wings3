#ifdef RP2040
#include "../IO/serial.h"
#else
#include <stdint.h>
#include <iostream>
#endif

#include "../IO/tracing.h"
#include "../IO/standardOutputCommandOperationCodes.h"

#ifdef RP2040
inline void printOpCodeAndKeyRP2040(std::string key)
{
    D_serialWrite(STANDARD_OUTPUT_OPERATION_CODE_SET_METRIC);
    D_serialWrite(",");
    D_serialWrite(key);
    D_serialWrite(",");  
}
#else
inline void printOpCodeAndKeyWindows(std::string key)
{
    std::cout << STANDARD_OUTPUT_OPERATION_CODE_SET_METRIC;
    std::cout << ",";
    std::cout << key;
    std::cout << ",";
}
#endif

void emitMetric(std::string key, std::string value)
{
    #ifdef RP2040
    printOpCodeAndKeyRP2040(key);
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(key);
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitMetricString(std::string key, std::string value)
{
    #ifdef RP2040
    printOpCodeAndKeyRP2040(key);
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(key);
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitIntegerMetric(std::string key, int value)
{
    #ifdef RP2040
    printOpCodeAndKeyRP2040(key);
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(key);
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitIntegerMetric(std::string key, uint8_t id, int value)
{
    #ifdef RP2040
    printOpCodeAndKeyRP2040(key);
    D_serialWrite(id);
    D_serialWrite(",");
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(key);
    std::cout << id;
    std::cout << ",";
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitDoubleMetric(std::string key, double value)
{
    #ifdef RP2040
    printOpCodeAndKeyRP2040(key);
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(key);
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitFloatMetric(std::string key, float value)
{
    #ifdef RP2040
    printOpCodeAndKeyRP2040(key);
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    printOpCodeAndKeyWindows(key);
    std::cout << value;
    std::cout << "\n";
    #endif
}