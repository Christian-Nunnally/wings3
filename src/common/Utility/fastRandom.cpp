#include "../commonHeaders.h"
#include "../Utility/fastRandom.h"

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
	#ifdef RP2040
	return lowByte(xorShift96());
	#else
	return xorShift96() & 0xff;
	#endif
}

bool fastRandomBoolean()
{
	#ifdef RP2040
	return lowByte(xorShift96() & 0x1);
	#else
	return xorShift96() & 0x1;
	#endif

}
