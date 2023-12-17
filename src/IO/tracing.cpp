#ifdef RP2040
#include "../IO/serial.h"
#else
//#include <stdint.h>
#include <iostream>
#endif

#include "../IO/tracing.h"

void emitMetric(char* key, char* value)
{
    #ifdef RP2040
    D_serialWrite("metric,");
    D_serialWrite(key);
    D_serialWrite(",");
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    std::cout << "metric,";
    std::cout << key;
    std::cout << ",";
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitMetricString(char* key, std::string value)
{
    #ifdef RP2040
    D_serialWrite("metric,");
    D_serialWrite(key);
    D_serialWrite(",");
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    std::cout << "metric,";
    std::cout << key;
    std::cout << ",";
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitIntegerMetric(char* key, int value)
{
    #ifdef RP2040
    D_serialWrite("metric,");
    D_serialWrite(key);
    D_serialWrite(",");
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    std::cout << "\n";
    std::cout << "metric,";
    std::cout << key;
    std::cout << ",";
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitIntegerMetric(char* key, uint8_t id, int value)
{
    #ifdef RP2040
    D_serialWrite("metric,");
    D_serialWrite(key);
    D_serialWrite(id);
    D_serialWrite(",");
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    std::cout << "\n";
    std::cout << "metric,";
    std::cout << key;
    std::cout << id;
    std::cout << ",";
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitDoubleMetric(char* key, double value)
{
    #ifdef RP2040
    D_serialWrite("metric,");
    D_serialWrite(key);
    D_serialWrite(",");
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    std::cout << "\n";
    std::cout << "metric,";
    std::cout << key;
    std::cout << ",";
    std::cout << value;
    std::cout << "\n";
    #endif
}

void emitFloatMetric(char* key, float value)
{
    #ifdef RP2040
    D_serialWrite("metric,");
    D_serialWrite(key);
    D_serialWrite(",");
    D_serialWrite(value);
    D_serialWrite("\n");
    #else
    std::cout << "\n";
    std::cout << "metric,";
    std::cout << key;
    std::cout << ",";
    std::cout << value;
    std::cout << "\n";
    #endif
}