
#include <LSM6DSOXSensor.h>
#include "../Peripherals/movementDetectionMlc.h"
#include "../Peripherals/movementDetection.h"
#include "../Utility/time.h"
#include "../Observers/stepDectectedObserver.h"
#include "../Observers/movementDetectedObserver.h"

#define ACCELEROMETER_SENSITIVITY 2 // Available values are: 2, 4, 8, 16 (G)
#define GYROSCOPE_SENSITIVITY 125 // Available values are: 125, 250, 500, 1000, 2000 (degrees per second)
#define ACCELEROMETER_OUTPUT_DATA_RATE 26.0f // Available values are: 1.6, 12.5, 26, 52, 104, 208, 417, 833, 1667, 3333, 6667 (Hz)
#define GYROSCOPE_OUTPUT_DATA_RATE 26.0f // Available values are: 12.5, 26, 52, 104, 208, 417, 833, 1667, 3333, 6667 (Hz)
#define INT_IMU 24
#define IMU_FIFO_FILL_THRESHOLD 50
#define MAX_STEP_DETECTED_SUBSCRIBERS 10
#define MILLISECONDS_BETWEEN_FIFO_POLL 39 

volatile int mems_event;
uint8_t lastMovementTypeStatus;
MovementType currentMovementType;
LSM6DSOXSensor imu(&Wire, (uint8_t)LSM6DSOX_I2C_ADD_L);
uint16_t currentStepCount;
uint16_t newStepCount;
uint16_t oldStepCount;
unsigned long lastFifoPollTime;

const int ImuErrorSamplesCount = 200;
int accelerometerErrorSamplesRemaining = ImuErrorSamplesCount;
float xAccelerometerError;
float yAccelerometerError;
float zAccelerometerError;
float xAccelerometerValue;
float yAccelerometerValue;
float zAccelerometerValue;

int gyroscopeErrorSamplesRemaining = ImuErrorSamplesCount;
float xGyroscopeError;
float yGyroscopeError;
float zGyroscopeError;
float xGyroscopeAngle;
float yGyroscopeAngle;
float zGyroscopeAngle;
float xGyroscopeAcceleration;
float yGyroscopeAcceleration;
float zGyroscopeAcceleration;
float xAccelerometerAngle;
float yAccelerometerAngle;
uint32_t lastGyroscopeDataReceivedTime;
uint32_t lastAccelerometerDataReceivedTime;

float currentRoll, currentPitch, currentYaw;

void setupIc2();
void setupAccelerometerGyro();
void feedProgramIntoImu();
void setupInterrupt();
void movementDetectedCallback();
void updateMovementTypeFromMLCStatus(uint8_t status);
void checkFifo();
inline bool doesFifoNeedPolling();
inline void checkMachineLearningCore();
inline void checkPedometer();
void calibrateImu();
void setupCalibration();
bool isCalibrationRunning();
void runCalibrationStep();
void finalizeCalibration();
inline void addGyroscopeCalibrationStep();
inline void addAccelerometerCalibrationStep();
uint32_t getTimestampOfLastFifoEvent();

void setupImu() 
{
    setupIc2();
    feedProgramIntoImu();
    setupAccelerometerGyro();
    setupInterrupt();
    delay(1000);
    calibrateImu();
}

void checkForMovement() 
{
    if (!mems_event) 
    {
        if (doesFifoNeedPolling()) checkFifo();
        return;
    }
    mems_event=0;

    checkPedometer();
    checkMachineLearningCore();
    checkFifo();
}

inline void checkPedometer()
{
    uint16_t newStepCount;
    imu.Get_Step_Count(&newStepCount);
    if (newStepCount != oldStepCount)
    {
        oldStepCount = newStepCount;
        notifyStepDetectedEvent();
    }
}

inline bool doesFifoNeedPolling()
{
    if (getTime() - lastFifoPollTime > MILLISECONDS_BETWEEN_FIFO_POLL)
    {
        lastFifoPollTime = getTime();
        return true;
    }
    return false;
}

inline void checkMachineLearningCore()
{
    LSM6DSOX_MLC_Status_t status;
    imu.Get_MLC_Status(&status);
    if (status.is_mlc1)
    {
        uint8_t mlc_out[8];
        imu.Get_MLC_Output(mlc_out);
        updateMovementTypeFromMLCStatus(mlc_out[0]);
    }
}

void checkFifo()
{
    uint8_t FifoTag;
    int32_t acceleration[3];
    int32_t rotation[3];
    uint16_t numberOfSamplesInFifo;
    imu.Get_FIFO_Num_Samples(&numberOfSamplesInFifo);
    for (uint16_t i = 0; i < numberOfSamplesInFifo; i++) 
    {
        imu.Get_FIFO_Tag(&FifoTag);
        
        if (FifoTag == 1) { 
            imu.Get_FIFO_G_Axes(rotation);
            uint32_t newGyroscopeDataReceivedTime = getTimestampOfLastFifoEvent();
            uint32_t elapsedTime = newGyroscopeDataReceivedTime - lastGyroscopeDataReceivedTime;

            xGyroscopeAcceleration = rotation[0] - xGyroscopeError;
            yGyroscopeAcceleration = rotation[1] - yGyroscopeError;
            zGyroscopeAcceleration = rotation[2] - zGyroscopeError;
            xGyroscopeAngle = xGyroscopeAngle + xGyroscopeAcceleration * elapsedTime;
            yGyroscopeAngle = yGyroscopeAngle + yGyroscopeAcceleration * elapsedTime;
            currentYaw = currentYaw + zGyroscopeAcceleration * elapsedTime;
            currentRoll = 0.96 * xGyroscopeAngle + 0.04 * xAccelerometerAngle;
            currentPitch = 0.96 * yGyroscopeAngle + 0.04 * yAccelerometerAngle;
            // Serial.print(" yaw: ");
            // Serial.print(currentYaw);
            // Serial.print(" roll: ");
            // Serial.print(currentRoll);
            // Serial.print(" pitch: ");
            Serial.println(currentPitch);
            lastGyroscopeDataReceivedTime = newGyroscopeDataReceivedTime;
        } 
        else if (FifoTag == 2) {
            imu.Get_FIFO_X_Axes(acceleration);
            uint32_t newAccelerometerDataReceivedTime = getTimestampOfLastFifoEvent();
            uint32_t elapsedTime = newAccelerometerDataReceivedTime - lastAccelerometerDataReceivedTime;

            xAccelerometerAngle = ((atan((acceleration[1]) / sqrt(pow((acceleration[0]), 2) + pow((acceleration[3]), 2))) * 180 / PI)) - xAccelerometerError;
            yAccelerometerAngle = ((atan(-1 * (acceleration[0]) / sqrt(pow((acceleration[1]), 2) + pow((acceleration[3]), 2))) * 180 / PI)) - yAccelerometerError;
            lastAccelerometerDataReceivedTime = newAccelerometerDataReceivedTime;
        }
    }
}

uint32_t getTimestampOfLastFifoEvent()
{
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
    uint8_t byte4;
    imu.Read_Reg(LSM6DSOX_TIMESTAMP0, &byte1);
    imu.Read_Reg(LSM6DSOX_TIMESTAMP1, &byte2);
    imu.Read_Reg(LSM6DSOX_TIMESTAMP2, &byte3);
    imu.Read_Reg(LSM6DSOX_TIMESTAMP3, &byte4);
    return byte1 | byte2 << 8 |  byte3 << 16 | byte4 << 24; 
}

void calibrateImu()
{
    setupCalibration();
    while(isCalibrationRunning()) runCalibrationStep();
    finalizeCalibration();
}

void setupCalibration()
{
    xGyroscopeError = 0;
    yGyroscopeError = 0;
    zGyroscopeError = 0;
    xAccelerometerError = 0;
    yAccelerometerError = 0;
    accelerometerErrorSamplesRemaining = ImuErrorSamplesCount;
    gyroscopeErrorSamplesRemaining = ImuErrorSamplesCount;
}

void runCalibrationStep()
{
    uint16_t numberOfSamplesInFifo = 0;
    uint8_t FifoTag;
    int32_t acceleration[3];
    int32_t rotation[3];
    imu.Get_FIFO_Num_Samples(&numberOfSamplesInFifo);
    for (uint16_t i = 0; i < numberOfSamplesInFifo; i++) 
    {
        imu.Get_FIFO_Tag(&FifoTag);
        if (FifoTag == 1) addGyroscopeCalibrationStep();
        else if (FifoTag == 2) addAccelerometerCalibrationStep();
    }
}

inline void addGyroscopeCalibrationStep()
{
    int32_t rotation[3];
    imu.Get_FIFO_G_Axes(rotation);
    if (gyroscopeErrorSamplesRemaining)
    {
        gyroscopeErrorSamplesRemaining--;
        xGyroscopeError += rotation[0];
        yGyroscopeError += rotation[1];
        zGyroscopeError += rotation[2];
    }
}

inline void addAccelerometerCalibrationStep()
{
    int32_t acceleration[3];
    imu.Get_FIFO_X_Axes(acceleration);
    if (accelerometerErrorSamplesRemaining)
    {
        accelerometerErrorSamplesRemaining--;
        xAccelerometerError += ((atan((acceleration[1]) / sqrt(pow((acceleration[0]), 2) + pow((acceleration[3]), 2))) * 180 / PI));
        yAccelerometerError += ((atan(-1 * (acceleration[0]) / sqrt(pow((acceleration[1]), 2) + pow((acceleration[3]), 2))) * 180 / PI));;
    }
}

bool isCalibrationRunning()
{
    return accelerometerErrorSamplesRemaining || gyroscopeErrorSamplesRemaining;
}

void finalizeCalibration()
{
    xGyroscopeError = xGyroscopeError / ImuErrorSamplesCount;
    yGyroscopeError = yGyroscopeError / ImuErrorSamplesCount;
    zGyroscopeError = zGyroscopeError / ImuErrorSamplesCount;
    xAccelerometerError = xAccelerometerError / ImuErrorSamplesCount;
    yAccelerometerError = yAccelerometerError / ImuErrorSamplesCount;
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
    imu.begin();
    imu.Enable_X();
    imu.Enable_G();
    imu.Set_X_FS(ACCELEROMETER_SENSITIVITY);
    imu.Set_G_FS(GYROSCOPE_SENSITIVITY);
    imu.Set_X_ODR(ACCELEROMETER_OUTPUT_DATA_RATE);
    imu.Set_G_ODR(GYROSCOPE_OUTPUT_DATA_RATE);
    imu.Set_FIFO_X_BDR(ACCELEROMETER_OUTPUT_DATA_RATE);
    imu.Set_FIFO_G_BDR(GYROSCOPE_OUTPUT_DATA_RATE);
    imu.Set_FIFO_Mode(LSM6DSOX_BYPASS_MODE);
    imu.Set_FIFO_Mode(LSM6DSOX_STREAM_MODE);
    imu.Set_FIFO_INT1_FIFO_Full(1);
    imu.Set_FIFO_Watermark_Level(IMU_FIFO_FILL_THRESHOLD);
    imu.Set_FIFO_Stop_On_Fth(1); delay(10);
    imu.Enable_Pedometer();
    imu.Set_Timestamp_Status(1);
}

void feedProgramIntoImu()
{
    ucf_line_t* programPointer = (ucf_line_t *)lsm6dsox_activity_recognition_for_mobile;
    uint32_t programSize = sizeof(lsm6dsox_activity_recognition_for_mobile) / sizeof(ucf_line_t);
    for (uint32_t lineCounter = 0; lineCounter < programSize; lineCounter++) 
    {
        imu.Write_Reg(programPointer[lineCounter].address, programPointer[lineCounter].data);
    }
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
    if (status != lastMovementTypeStatus) return;
    else if (status == 0) currentMovementType = Stationary;
    else if (status == 1) currentMovementType = Walking;
    else if (status == 4) currentMovementType = Jogging;
    else if (status == 8) currentMovementType = Biking;
    else if (status == 12) currentMovementType = Driving;
    else currentMovementType = Unknown;
    notifyMovementDetectedEvent();  
}