#ifndef SERIAL_H
#define SERIAL_H

#if ENABLE_SERIAL_WRITE
#define D_serialBegin(...)   setupSerial(__VA_ARGS__)
#define D_serialWrite(...)   writeSerial(__VA_ARGS__)
#define D_serialWriteNewLine(...)   writeSerialNewLine(__VA_ARGS__)
#else
#define D_serialBegin(...)
#define D_serialWrite(...)
#define D_serialWriteNewLine(...) 
#endif

#if ENABLE_SERIAL_READ
#define D_serialRead(...)   readSerial(__VA_ARGS__)
#else
#define D_serialRead(...)
#endif

void setupSerial();
void readSerial();
void writeSerial();
void writeSerialNewLine();

#endif