
#include "../commonHeaders.h"
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