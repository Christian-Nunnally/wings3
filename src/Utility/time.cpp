
#include "../commonHeaders.h"
#include "../Utility/time.h"
#include "../settings.h"

#ifdef RP2040
#else
#include <chrono>
using namespace std::chrono;
#endif

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

unsigned long getSystemTime()
{
    #ifdef RP2040
    return millis();
    #else
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    #endif
}