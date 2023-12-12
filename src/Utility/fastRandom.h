#ifndef FAST_RANDOM_H
#define FAST_RANDOM_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif

void fastRandomSeed(long seed);
int fastRandomInteger(int minimumValue, int maximumValue);
int fastRandomInteger(int maximumValue);
uint16_t fastRandomWord();
uint8_t fastRandomByte();
bool fastRandomBoolean();

#endif