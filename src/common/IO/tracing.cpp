#ifdef RP2040
#include "../IO/serial.h"
#else
#include <stdint.h>
#include <iostream>
#endif

#include "../IO/tracing.h"
#include "../IO/standardOutputCommandOperationCodes.h"

#ifdef RP2040
inline void printOpCodeAndKeyRP2040(char* key)
{
    D_serialWrite(STANDARD_OUTPUT_OPERATION_CODE_SET_METRIC);
    D_serialWrite(",");
    D_serialWrite(key);
    D_serialWrite(",");  
}
#else
inline void printOpCodeAndKeyWindows(char* key)
{
    std::cout << STANDARD_OUTPUT_OPERATION_CODE_SET_METRIC;
    std::cout << ",";
    std::cout << key;
    std::cout << ",";
}
#endif

void emitMetric(char* key, char* value)
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

void emitMetricString(char* key, std::string value)
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

void emitIntegerMetric(char* key, int value)
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

void emitIntegerMetric(char* key, uint8_t id, int value)
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

void emitDoubleMetric(char* key, double value)
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

void emitFloatMetric(char* key, float value)
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