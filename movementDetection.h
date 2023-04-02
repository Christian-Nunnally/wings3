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

#endif