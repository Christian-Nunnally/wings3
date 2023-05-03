#ifndef MOVEMENT_DETECTION_H
#define MOVEMENT_DETECTION_H

enum MovementType
{
    Stationary,
    Walking,
    Jogging,
    Biking,
    Driving,
    Unknown,
};

void setupImu();
void checkForMovement();
MovementType getCurrentMovementType();
// int32_t getCurrentXAcceleration();
// int32_t getCurrentYAcceleration();
// int32_t getCurrentZAcceleration();
// int32_t getCurrentPitchAcceleration();
// int32_t getCurrentYawAcceleration();
// int32_t getCurrentRollAcceleration();
// int32_t getCurrentXPosition();
// int32_t getCurrentYPosition();
// int32_t getCurrentZPosition();
int getCurrentPitchPosition();
int getCurrentYawPosition();
int getCurrentRollPosition();
void subscribeToStepDetectedEvent(void (*subscriber)());
void unsubscribeFromStepDetectedEvent(void (*subscriber)());

#endif