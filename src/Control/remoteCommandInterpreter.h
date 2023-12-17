#ifndef REMOTE_COMMAND_INTERPRETER_H
#define REMOTE_COMMAND_INTERPRETER_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif

void interpretRemoteCommand(uint8_t operationCode, int16_t value, uint8_t flags);

#endif