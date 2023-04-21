
#include <LSM6DSOXSensor.h>
#include "movementDetectionMlc.h"
#include "movementDetection.h"
#include "time.h"

#define ACCELEROMETER_SENSITIVITY 2 // Available values are: 2, 4, 8, 16 (G)
#define GYROSCOPE_SENSITIVITY 125 // Available values are: 125, 250, 500, 1000, 2000 (degrees per second)
#define ACCELEROMETER_OUTPUT_DATA_RATE 26.0f // Available values are: 1.6, 12.5, 26, 52, 104, 208, 417, 833, 1667, 3333, 6667 (Hz)
#define GYROSCOPE_OUTPUT_DATA_RATE 26.0f // Available values are: 12.5, 26, 52, 104, 208, 417, 833, 1667, 3333, 6667 (Hz)
#define INT_IMU 24
#define IMU_FIFO_FILL_THRESHOLD 50
#define MAX_STEP_DETECTED_SUBSCRIBERS 10

volatile int mems_event;
MovementType currentMovementType;
LSM6DSOXSensor imu(&Wire, (uint8_t)LSM6DSOX_I2C_ADD_L);
ucf_line_t *machineLearningCoreProgramPointer;
int32_t machineLearningCoreLineCounter;
int32_t machineLearningCoreTotalNumberOfLine;
uint16_t currentStepCount;
uint16_t newStepCount;
uint16_t oldStepCount;
unsigned long lastFifoPollTime;

void (*stepDetectedEventSubscribers[MAX_STEP_DETECTED_SUBSCRIBERS])();
byte currentSubscriberCount = 0;

void setupIc2();
void setupAccelerometerGyro();
void feedProgramIntoImu();
void setupInterrupt();
void movementDetectedCallback();
void updateMovementTypeFromMLCStatus(uint8_t status);
void checkFifo();
inline void notifyStepDetectedEventSubscribers();

#define MILLISECONDS_BETWEEN_FIFO_POLL 1000 

void setupImu() 
{
    setupIc2();
    delay(10);
    feedProgramIntoImu();
    delay(10);
    setupAccelerometerGyro();
    delay(10);
    setupInterrupt();
    delay(10);
}

void checkForMovement() 
{
    if (!mems_event) 
    {
        if (getTime() - lastFifoPollTime > MILLISECONDS_BETWEEN_FIFO_POLL)
        {
            lastFifoPollTime = getTime();
            checkFifo();
        }
        return;
    }
    mems_event=0;
    uint16_t newStepCount;
    imu.Get_Step_Count(&newStepCount);
    if (newStepCount != oldStepCount)
    {
        oldStepCount = newStepCount;
        notifyStepDetectedEventSubscribers();
    }

    LSM6DSOX_MLC_Status_t status;
    imu.Get_MLC_Status(&status);
    if (status.is_mlc1)
    {
        uint8_t mlc_out[8];
        imu.Get_MLC_Output(mlc_out);
        updateMovementTypeFromMLCStatus(mlc_out[0]);
    }
    checkFifo();
}

void checkFifo()
{
    uint16_t numSamples = 0; // number of samples in FIFO
    uint8_t Tag; // FIFO data sensor identifier
    int32_t acceleration[3]; // X, Y, Z accelerometer values in mg
    int32_t rotation[3]; // X, Y, Z gyroscope values in mdps
    int16_t dummy[3];
    uint8_t fullStatus = 0; // full status
    int32_t count_acc_samples = 0;
    int32_t count_gyro_samples = 0;
    int32_t count_dummy_samples = 0;

    // Get number of samples in buffer
    imu.Get_FIFO_Num_Samples(&numSamples);
    Serial.print("Samples in FIFO: ");
    Serial.println(numSamples);
    Serial.flush();

    imu.Get_FIFO_Full_Status(&fullStatus);

    if(fullStatus) {
        Serial.println("-- FIFO Watermark level reached!, fetching data.");
        Serial.flush();

        imu.Get_FIFO_Num_Samples(&numSamples);

        Serial.print("numSamples=");
        Serial.println(numSamples);
        Serial.flush();

        // fetch data from FIFO
        for (uint16_t i = 0; i < numSamples; i++) {

        imu.Get_FIFO_Tag(&Tag); // get data identifier
        
        // Get gyroscope data
        if (Tag == 1) { 
            imu.Get_FIFO_G_Axes(rotation);
            count_gyro_samples++;
            #if 0 // set to 1 for printing values
            Serial.print("mdps: "); Serial.print(rotation[0]); 
            Serial.print(", "); Serial.print(rotation[1]); 
            Serial.print(", "); Serial.print(rotation[2]); 
            Serial.println();
            Serial.flush();
            #endif
        } 
        
        // Get accelerometer data
        else if (Tag == 2) {
            imu.Get_FIFO_X_Axes(acceleration);
            count_acc_samples++; 
            #if 0 // set to 1 for printing values
            Serial.print("mG: "); Serial.print(acceleration[0]); 
            Serial.print(", "); Serial.print(acceleration[1]); 
            Serial.print(", "); Serial.print(acceleration[2]); 
            Serial.println();
            Serial.flush();
            #endif
        }

        // Flush unused tag
        else {
            imu.Get_FIFO_Data((uint8_t *)dummy);
            count_dummy_samples++;
        }
        }
        Serial.print("Acc Samples: ");
        Serial.println(count_acc_samples);
        Serial.print("Gyro Samples: ");
        Serial.println(count_gyro_samples);
        Serial.print("Dummy Samples: ");
        Serial.println(count_dummy_samples);
        Serial.flush();
    }
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
    Wire.setClock(400000);
}

void setupAccelerometerGyro()
{
    int result;
    result = imu.begin();
     delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Enable_X();
     delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Enable_G();
     delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Set_X_FS(ACCELEROMETER_SENSITIVITY);
     delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Set_G_FS(GYROSCOPE_SENSITIVITY);
     delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Set_X_ODR(ACCELEROMETER_OUTPUT_DATA_RATE);
     delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Set_G_ODR(GYROSCOPE_OUTPUT_DATA_RATE);
     delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Set_FIFO_X_BDR(ACCELEROMETER_OUTPUT_DATA_RATE);
     delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Set_FIFO_G_BDR(GYROSCOPE_OUTPUT_DATA_RATE);
     delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Set_FIFO_Mode(LSM6DSOX_BYPASS_MODE);
    delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    delay(10);
    result = imu.Set_FIFO_Mode(LSM6DSOX_STREAM_MODE);
    delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Set_FIFO_INT1_FIFO_Full(1); // enable FIFO full interrupt on sensor INT1 pin.
    delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Set_FIFO_Watermark_Level(IMU_FIFO_FILL_THRESHOLD);
    delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    result = imu.Set_FIFO_Stop_On_Fth(1); delay(10);
    delay(10);
    if (result != 0) Serial.println("Error in setupAccelerometerGyro");
    delay(10);
    if (imu.Enable_Pedometer())
    {
        Serial.println("Error: Enabling pedometer unsuccessful.");
    }
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

void subscribeToStepDetectedEvent(void (*subscriber)()) 
{
  if (currentSubscriberCount < MAX_STEP_DETECTED_SUBSCRIBERS) { // Check if there is still space in the array
    stepDetectedEventSubscribers[currentSubscriberCount] = subscriber;
    currentSubscriberCount++;
    printf("Subscriber added.\n");
  } else {
    printf("Maximum number of subscribers reached.\n");
  }
}

void unsubscribeFromStepDetectedEvent(void (*subscriber)()) 
{
    for (int i = 0; i < currentSubscriberCount; i++) 
    {
        if (stepDetectedEventSubscribers[i] == subscriber) 
        {
            stepDetectedEventSubscribers[i] = NULL;
            for (int j = i; j < currentSubscriberCount - 1; j++) 
            {
                stepDetectedEventSubscribers[j] = stepDetectedEventSubscribers[j + 1];
            }
            printf("Subscriber removed.\n");
            return;
        }
    }
    printf("Subscriber not found.\n");
}

inline void notifyStepDetectedEventSubscribers() 
{
    for (int i = 0; i < currentSubscriberCount; i++) 
    {
        if (stepDetectedEventSubscribers[i] != NULL) 
        {
            (*stepDetectedEventSubscribers[i])();
        }
    }
}