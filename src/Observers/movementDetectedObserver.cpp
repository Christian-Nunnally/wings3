#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#define NULL 0
#endif
#include "movementDetectedObserver.h"

#define MAX_SUBSCRIBERS 10

void (*movementDetectedEventSubscribers[MAX_SUBSCRIBERS])();
uint8_t currentMovementDetectedSubscriberCount = 0;

void subscribeToMovementDetectedEvent(void (*listener)()) 
{
    if (currentMovementDetectedSubscriberCount < MAX_SUBSCRIBERS) {
        movementDetectedEventSubscribers[currentMovementDetectedSubscriberCount] = listener;
        currentMovementDetectedSubscriberCount++;
    }
}

void unsubscribeFromMovementDetectedEvent(void (*listener)()) 
{
    for (int i = 0; i < currentMovementDetectedSubscriberCount; i++) 
    {
        if (movementDetectedEventSubscribers[i] == listener) 
        {
            movementDetectedEventSubscribers[i] = NULL;
            for (int j = i; j < currentMovementDetectedSubscriberCount - 1; j++) movementDetectedEventSubscribers[j] = movementDetectedEventSubscribers[j + 1];
        }
    }
}

void notifyMovementDetectedEvent() 
{
    for (int i = 0; i < currentMovementDetectedSubscriberCount; i++) 
    {
        if (movementDetectedEventSubscribers[i] != NULL) (*movementDetectedEventSubscribers[i])();
    }
}