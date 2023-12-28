#ifndef STEP_DETECTED_OBSERVER_H
#define STEP_DETECTED_OBSERVER_H

void subscribeToStepDetectedEvent(void (*listener)());
void unsubscribeFromStepDetectedEvent(void (*listener)());
void notifyStepDetectedEvent();

#endif