#include "../Utility/fastRandom.h"
#include <Arduino.h>

static unsigned long x=132456789, y=362436069, z=521288629;

unsigned long xorShift96()
{
	unsigned long t;

	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

void fastRandomSeed(long seed)
{
	x=seed;
}

int fastRandomInteger(int minimumValue, int maximumValue)
{
	return (int) ((((xorShift96() & 0xFFFF) * (maximumValue-minimumValue))>>16) + minimumValue);
}

int fastRandomInteger(int maximumValue)
{
	return (int) (((xorShift96() & 0xFFFF) * maximumValue)>>16);
}

uint16_t fastRandomWord()
{
	return (uint16_t) (xorShift96() & 0xFFFF);
}

uint8_t fastRandomByte()
{
	return lowByte(xorShift96());
}

bool fastRandomBoolean()
{
	return lowByte(xorShift96() & 0x1);
}
