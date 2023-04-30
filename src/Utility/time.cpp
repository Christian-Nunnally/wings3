
#include "../Utility/time.h"

unsigned long currentCachedTime;

unsigned long getTime()
{
    return currentCachedTime;
}

void setTime(unsigned long time)
{
    currentCachedTime = time;
}