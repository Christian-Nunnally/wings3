#ifndef MOVEMENT_DETECTION_H
#define MOVEMENT_DETECTION_H

enum MovementType
{
    Stationary,
    Walking,
    Jogging,
    Biking,
    Driving,
    UnknownMovement,
};

void setupImu();
void checkForMovement();
MovementType getCurrentMovementType();
int getCurrentYAngle();
int getCurrentXAngle();
int getCurrentPitchPosition();
int getCurrentYawPosition();
int getCurrentRollPosition();
void subscribeToStepDetectedEvent(void (*subscriber)());
void unsubscribeFromStepDetectedEvent(void (*subscriber)());
void enableMovementTypeDetection();
void disableMovementTypeDetection();
void enableStepDetection();
void disableStepDetection();

#endif