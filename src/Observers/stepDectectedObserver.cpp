#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#define NULL 0
#endif
#include "stepDectectedObserver.h"

#define MAX_SUBSCRIBERS 10

void (*stepDetectedEventSubscribers[MAX_SUBSCRIBERS])();
uint8_t currentSubscriberCount = 0;

void subscribeToStepDetectedEvent(void (*listener)()) 
{
    if (currentSubscriberCount < MAX_SUBSCRIBERS) {
        stepDetectedEventSubscribers[currentSubscriberCount] = listener;
        currentSubscriberCount++;
    }
}

void unsubscribeFromStepDetectedEvent(void (*listener)()) 
{
    for (int i = 0; i < currentSubscriberCount; i++) 
    {
        if (stepDetectedEventSubscribers[i] == listener) 
        {
            stepDetectedEventSubscribers[i] = NULL;
            for (int j = i; j < currentSubscriberCount - 1; j++) stepDetectedEventSubscribers[j] = stepDetectedEventSubscribers[j + 1];
        }
    }
}

void notifyStepDetectedEvent() 
{
    for (int i = 0; i < currentSubscriberCount; i++) 
    {
        if (stepDetectedEventSubscribers[i] != NULL) (*stepDetectedEventSubscribers[i])();
    }
}