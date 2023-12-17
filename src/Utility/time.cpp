
#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#define NULL 0
#endif

#include "../Utility/time.h"
#include "../settings.h"


unsigned long currentCachedTime;
unsigned long lastCurrentCachedTime;

unsigned long getTime()
{
    return currentCachedTime;
}

void setTime(unsigned long time)
{
    currentCachedTime = time;
}