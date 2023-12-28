#ifndef MOVEMENT_DETECTED_OBSERVER_H
#define MOVEMENT_DETECTED_OBSERVER_H

void subscribeToMovementDetectedEvent(void (*listener)());
void unsubscribeFromMovementDetectedEvent(void (*listener)());
void notifyMovementDetectedEvent();

#endif