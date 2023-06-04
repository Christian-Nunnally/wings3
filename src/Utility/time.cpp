
#include <Arduino.h>
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
    #ifdef ENABLE_SERIAL
    // Serial.print(currentCachedTime - lastCurrentCachedTime);
    // Serial.println("ms");
    // lastCurrentCachedTime = currentCachedTime;
    #endif
}