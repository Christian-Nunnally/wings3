#ifndef FAST_RANDOM_H
#define FAST_RANDOM_H

#include "../commonHeaders.h"

void fastRandomSeed(long seed);
int fastRandomInteger(int minimumValue, int maximumValue);
int fastRandomInteger(int maximumValue);
uint16_t fastRandomWord();
uint8_t fastRandomByte();
bool fastRandomBoolean();

#endif