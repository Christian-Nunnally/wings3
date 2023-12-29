
#ifdef RP2040
#include <LSM6DSOXSensor.h>
#include <SingleEMAFilterLib.h>
#include "../Peripherals/movementDetectionMlc.h"
#else
#include <stdint.h>
#endif

#include "../Peripherals/movementDetection.h"
#include "../Utility/time.h"
#include "../IO/serial.h"
#include "../Observers/stepDectectedObserver.h"
#include "../Observers/movementDetectedObserver.h"
#include "../commonHeaders.h"
#include "../settings.h"

#define ACCELEROMETER_SENSITIVITY 2 // Available values are: 2, 4, 8, 16 (G)
#define GYROSCOPE_SENSITIVITY 125 // Available values are: 125, 250, 500, 1000, 2000 (degrees per second)
#define ACCELEROMETER_OUTPUT_DATA_RATE 26.0f // Available values are: 1.6, 12.5, 26, 52, 104, 208, 417, 833, 1667, 3333, 6667 (Hz)
#define GYROSCOPE_OUTPUT_DATA_RATE 26.0f // Available values are: 12.5, 26, 52, 104, 208, 417, 833, 1667, 3333, 6667 (Hz)
#define INT_IMU 9
#define INT_IMU_2 14
#define SDA_IMU_PIN 10
#define SCL_IMU_PIN 11
#define IMU_FIFO_FILL_THRESHOLD 50
#define MAX_STEP_DETECTED_SUBSCRIBERS 10
#define MILLISECONDS_BETWEEN_FIFO_POLL 39 
#define MOVEMENT_EMA_FILTER_ALPHA .2
#define ACCELERATION_SHIFT_AMOUNT 4
#define ACCELERATION_SHIFT_AMOUNT_POST 20

#ifdef RP2040
LSM6DSOXSensor imu(&Wire, (uint8_t)LSM6DSOX_I2C_ADD_L);
#endif

volatile int mems_event;
uint8_t lastMovementTypeStatus;
MovementType currentMovementType = Stationary;
uint16_t currentStepCount;
uint16_t newStepCount;
uint16_t oldStepCount;
unsigned long lastFifoPollTime;

const int ImuErrorSamplesCount = 200;
int accelerometerErrorSamplesRemaining = ImuErrorSamplesCount;
int xAccelerometerError;
int yAccelerometerError;
int zAccelerometerError;
int xAccelerometerValue;
int yAccelerometerValue;
int zAccelerometerValue;

int gyroscopeErrorSamplesRemaining = ImuErrorSamplesCount;
int xGyroscopeError;
int yGyroscopeError;
int zGyroscopeError;
int xGyroscopeAngle;
int yGyroscopeAngle;
int zGyroscopeAngle;
int xGyroscopeAcceleration;
int yGyroscopeAcceleration;
int zGyroscopeAcceleration;
int xAccelerometerAngle;
int yAccelerometerAngle;
uint32_t lastGyroscopeDataReceivedTime;
uint32_t lastAccelerometerDataReceivedTime;

int currentRoll;
int currentPitch;
int currentYaw;

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
    D_serialWriteNewLine("Begin IMU Initialization.");
    setupIc2();
    D_serialWriteNewLine("Ic2 Initialized.");
    //feedProgramIntoImu();
    D_serialWriteNewLine("Program fed to MLC.");
    setupAccelerometerGyro();
    D_serialWriteNewLine("Accel/Gyro Initialized");
    //setupInterrupt();
    D_serialWriteNewLine("Interrupt Initialized");
    #ifdef RP2040
    delay(1000);
    calibrateImu();
    D_serialWriteNewLine("IMU Initalized.");
    #endif
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
    uint16_t newStepCount = 0;
    
    #ifdef RP2040
    imu.Get_Step_Count(&newStepCount);
    #endif

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
    #ifdef RP2040
    LSM6DSOX_MLC_Status_t status;
    imu.Get_MLC_Status(&status);
    if (status.is_mlc1)
    {
        uint8_t mlc_out[8];
        imu.Get_MLC_Output(mlc_out);
        updateMovementTypeFromMLCStatus(mlc_out[0]);
    }
    #endif
}

void checkFifo()
{
    uint8_t FifoTag = 0;
    int32_t acceleration[3];
    int32_t rotation[3];
    uint16_t numberOfSamplesInFifo;
    #ifdef RP2040
    imu.Get_FIFO_Num_Samples(&numberOfSamplesInFifo);
    #else
    numberOfSamplesInFifo = 0;
    #endif
    for (uint16_t i = 0; i < numberOfSamplesInFifo; i++) 
    {
        #ifdef RP2040
        imu.Get_FIFO_Tag(&FifoTag);
        #endif
        
        if (FifoTag == 1) { 
            #ifdef RP2040
            imu.Get_FIFO_G_Axes(rotation);
            #endif
            uint32_t newGyroscopeDataReceivedTime = getTimestampOfLastFifoEvent();
            uint32_t elapsedTime = newGyroscopeDataReceivedTime - lastGyroscopeDataReceivedTime;
            if (!lastGyroscopeDataReceivedTime)
            {
                lastGyroscopeDataReceivedTime = newGyroscopeDataReceivedTime;
                continue;
            }
            lastGyroscopeDataReceivedTime = newGyroscopeDataReceivedTime;

            xGyroscopeAcceleration = (rotation[0] >> ACCELERATION_SHIFT_AMOUNT) - xGyroscopeError;
            yGyroscopeAcceleration = (rotation[1] >> ACCELERATION_SHIFT_AMOUNT) - yGyroscopeError;
            zGyroscopeAcceleration = (rotation[2] >> ACCELERATION_SHIFT_AMOUNT) - zGyroscopeError;
            xGyroscopeAngle = xGyroscopeAngle + xGyroscopeAcceleration * elapsedTime;
            yGyroscopeAngle = yGyroscopeAngle + yGyroscopeAcceleration * elapsedTime;
            currentYaw = currentYaw + zGyroscopeAcceleration * elapsedTime;
            currentRoll = 0.96 * xGyroscopeAngle + 0.04 * xAccelerometerAngle;
            currentPitch = 0.96 * yGyroscopeAngle + 0.04 * yAccelerometerAngle;
            D_serialWrite(" y: ");
            D_serialWrite((byte)(D_abs(currentYaw >> ACCELERATION_SHIFT_AMOUNT_POST) & 0xff));
            D_serialWrite(" roll: ");
            D_serialWrite((byte)(D_abs(currentRoll >> ACCELERATION_SHIFT_AMOUNT_POST) & 0xff));
            D_serialWrite(" pitch: ");
            D_serialWriteNewLine((byte)(D_abs(currentPitch >> ACCELERATION_SHIFT_AMOUNT_POST) & 0xff));
        } 
        else if (FifoTag == 2) {
            #ifdef RP2040
            imu.Get_FIFO_X_Axes(acceleration);
            #endif
            uint32_t newAccelerometerDataReceivedTime = getTimestampOfLastFifoEvent();
            if (!lastAccelerometerDataReceivedTime)
            {
                lastAccelerometerDataReceivedTime = newAccelerometerDataReceivedTime;
                continue;
            }
            lastAccelerometerDataReceivedTime = newAccelerometerDataReceivedTime;

            int accelX = acceleration[0] >> ACCELERATION_SHIFT_AMOUNT;
            int accelY = acceleration[1] >> ACCELERATION_SHIFT_AMOUNT;
            int accelZ = acceleration[2] >> ACCELERATION_SHIFT_AMOUNT;
            xAccelerometerAngle = ((atan((accelY) / sqrt(pow((accelX), 2) + pow((accelZ), 2))) * 180 / PI)) - xAccelerometerError;
            yAccelerometerAngle = ((atan(-1 * (accelX) / sqrt(pow((accelY), 2) + pow((accelZ), 2))) * 180 / PI)) - yAccelerometerError;
        }
    }
}

uint32_t getTimestampOfLastFifoEvent()
{
    #ifdef RP2040
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
    uint8_t byte4;
    imu.Read_Reg(LSM6DSOX_TIMESTAMP0, &byte1);
    imu.Read_Reg(LSM6DSOX_TIMESTAMP1, &byte2);
    imu.Read_Reg(LSM6DSOX_TIMESTAMP2, &byte3);
    imu.Read_Reg(LSM6DSOX_TIMESTAMP3, &byte4);
    return byte1 | byte2 << 8 |  byte3 << 16 | byte4 << 24; 
    #else 
    return 0;
    #endif
}

int getCurrentPitchPosition()
{
    return currentPitch;
}

int getCurrentYawPosition()
{
    return currentYaw;
}

int getCurrentRollPosition()
{
    return currentRoll;
}

void calibrateImu()
{
    D_serialWriteNewLine("Calibrating IMU.");
    setupCalibration();
    D_serialWriteNewLine("Calibration set up.");
    while(isCalibrationRunning()) runCalibrationStep();
    D_serialWriteNewLine("Finalizing calibration.");
    finalizeCalibration();
    D_serialWriteNewLine("Done calibrating IMU.");
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
    #ifdef RP2040
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
    #endif
}

inline void addGyroscopeCalibrationStep()
{
    #ifdef RP2040
    int32_t rotation[3];
    imu.Get_FIFO_G_Axes(rotation);
    if (gyroscopeErrorSamplesRemaining)
    {
        gyroscopeErrorSamplesRemaining--;
        xGyroscopeError += rotation[0] >> ACCELERATION_SHIFT_AMOUNT;
        yGyroscopeError += rotation[1] >> ACCELERATION_SHIFT_AMOUNT;
        zGyroscopeError += rotation[2] >> ACCELERATION_SHIFT_AMOUNT;
    }
    #endif
}

inline void addAccelerometerCalibrationStep()
{
    #ifdef RP2040
    int32_t acceleration[3];
    imu.Get_FIFO_X_Axes(acceleration);
    if (accelerometerErrorSamplesRemaining)
    {
        accelerometerErrorSamplesRemaining--;
        int accelX = acceleration[0] >> ACCELERATION_SHIFT_AMOUNT;
        int accelY = acceleration[1] >> ACCELERATION_SHIFT_AMOUNT;
        int accelZ = acceleration[2] >> ACCELERATION_SHIFT_AMOUNT;
        xAccelerometerError += ((atan((accelY) / sqrt(pow((accelX), 2) + pow((accelZ), 2))) * 180 / PI));
        yAccelerometerError += ((atan(-1 * (accelX) / sqrt(pow((accelY), 2) + pow((accelZ), 2))) * 180 / PI));;
    }
    #endif
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
    #ifdef RP2040
    pinMode(INT_IMU, INPUT_PULLDOWN);
    pinMode(INT_IMU_2, INPUT_PULLDOWN);
    delay(200);

    bool result = Wire1.setSDA(SDA_IMU_PIN);
    if (result) D_serialWriteNewLine("SDA set");
    result = Wire1.setSCL(SCL_IMU_PIN);
    if (result) D_serialWriteNewLine("SCL set");
    Wire1.begin();
    #endif
}

void setupAccelerometerGyro()
{
    #ifdef RP2040
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
    imu.Set_FIFO_INT2_FIFO_Full(1);
    imu.Set_FIFO_Watermark_Level(IMU_FIFO_FILL_THRESHOLD);
    imu.Set_FIFO_Stop_On_Fth(1); delay(10);
    imu.Enable_Pedometer();
    imu.Set_Timestamp_Status(1);
    #endif
}

void feedProgramIntoImu()
{
    #ifdef RP2040
    ucf_line_t* programPointer = (ucf_line_t *)lsm6dsox_activity_recognition_for_mobile;
    uint32_t programSize = sizeof(lsm6dsox_activity_recognition_for_mobile) / sizeof(ucf_line_t);
    for (uint32_t lineCounter = 0; lineCounter < programSize; lineCounter++) 
    {
        imu.Write_Reg(programPointer[lineCounter].address, programPointer[lineCounter].data);
    }
    #endif
}

void setupInterrupt()
{
    #ifdef RP2040
    pinMode(INT_IMU, INPUT_PULLDOWN);
    pinMode(INT_IMU_2, INPUT_PULLDOWN);
    D_serialWriteNewLine("Attaching interrupt");
    attachInterrupt(INT_IMU, movementDetectedCallback, RISING);
    D_serialWriteNewLine("Done attaching interrupt");
    #endif

}

void movementDetectedCallback() 
{
    mems_event = 1;
}

void updateMovementTypeFromMLCStatus(uint8_t status) 
{
    lastMovementTypeStatus = status;
    if (status == 0) currentMovementType = Stationary;
    else if (status == 1) currentMovementType = Walking;
    else if (status == 4) currentMovementType = Jogging;
    else if (status == 8) currentMovementType = Biking;
    else if (status == 12) currentMovementType = Driving;
    else currentMovementType = UnknownMovement;
    notifyMovementDetectedEvent();  
}