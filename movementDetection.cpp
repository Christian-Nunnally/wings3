#include <LSM6DSOXSensor.h>
#include "movementDetectionMlc.h"
#include "movementDetection.h"

#define INT_IMU 24

volatile int mems_event;
MovementType currentMovementType;
LSM6DSOXSensor imu(&Wire, (uint8_t)LSM6DSOX_I2C_ADD_L);
ucf_line_t *machineLearningCoreProgramPointer;
int32_t machineLearningCoreLineCounter;
int32_t machineLearningCoreTotalNumberOfLine;

void setupIc2();
void setupAccelerometerGyro();
void feedProgramIntoImu();
void setupInterrupt();
void movementDetectedCallback();
void updateMovementTypeFromMLCStatus(uint8_t status);

void setupImu() 
{
    setupIc2();
    setupAccelerometerGyro();
    feedProgramIntoImu();
    setupInterrupt();
}

void checkForMovement() 
{
    if (!mems_event) return; 
    mems_event=0;
    LSM6DSOX_MLC_Status_t status;
    imu.Get_MLC_Status(&status);
    if (!status.is_mlc1) return;
    uint8_t mlc_out[8];
    imu.Get_MLC_Output(mlc_out);
    updateMovementTypeFromMLCStatus(mlc_out[0]);
}

MovementType getCurrentMovementType()
{
    return currentMovementType;
}

void setupIc2()
{
    pinMode(INT_IMU, OUTPUT);
    digitalWrite(INT_IMU, LOW);
    delay(200);
    Wire.begin();
}

void setupAccelerometerGyro()
{
    imu.begin();
    imu.Enable_X();
    imu.Enable_G();
}

void feedProgramIntoImu()
{
    machineLearningCoreProgramPointer = (ucf_line_t *)lsm6dsox_activity_recognition_for_mobile;
    machineLearningCoreTotalNumberOfLine = sizeof(lsm6dsox_activity_recognition_for_mobile) / sizeof(ucf_line_t);
    if (Serial) Serial.println("Activity Recognition for LSM6DSOX MLC");
    if (Serial) Serial.print("UCF Number Line=");
    if (Serial) Serial.println(machineLearningCoreTotalNumberOfLine);
    for (machineLearningCoreLineCounter=0; machineLearningCoreLineCounter<machineLearningCoreTotalNumberOfLine; machineLearningCoreLineCounter++) 
    {
        if(imu.Write_Reg(machineLearningCoreProgramPointer[machineLearningCoreLineCounter].address, machineLearningCoreProgramPointer[machineLearningCoreLineCounter].data)) 
        {
            if (Serial) Serial.print("Error loading the Program to LSM6DSOX at line: ");
            if (Serial) Serial.println(machineLearningCoreLineCounter);
        }
    }
    if (Serial) Serial.println("Program loaded inside the LSM6DSOX MLC");
}

void setupInterrupt()
{
    pinMode(INT_IMU, INPUT);
    attachInterrupt(INT_IMU, movementDetectedCallback, RISING);
}

void movementDetectedCallback() 
{
    mems_event = 1;
}

void updateMovementTypeFromMLCStatus(uint8_t status) 
{
    switch(status) 
    {
        case 0:
            currentMovementType = Stationary;
            break;
        case 1:
            currentMovementType = Walking;
            break;
        case 4:
            currentMovementType = Jogging;
            break;
        case 8:
            currentMovementType = Biking;
            break;
        case 12:
            currentMovementType = Driving;
            break;
        default:
            currentMovementType = Unknown;
            break;
    } 
    if (!Serial) return;
    switch(status) 
    {
        case 0:
            Serial.println("Activity: Stationary");
            break;
        case 1:
            Serial.println("Activity: Walking");
            break;
        case 4:
            Serial.println("Activity: Jogging");
            break;
        case 8:
            Serial.println("Activity: Biking");
            break;
        case 12:
            Serial.println("Activity: Driving");
            break;
        default:
            Serial.println("Activity: Unknown");
            break;
    }   
}