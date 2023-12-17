#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#define NULL 0
#endif

#include "../IO/analogInput.h"
#include "../Utility/time.h"
#include "../Utility/fastMath.h"
#include "../IO/serial.h"

#define MAX_ANALOG_INPUT_SUBSCRIBERS 10

#define BRIGHTNESS_ANALOG_INPUT_PIN (uint8_t)27U // This is dial 1
#define MODE_ANALOG_INPUT_PIN (uint8_t)28U // This is dial 2
#define ANALOG_INPUT_REFRESH_FREQUENCY 500
#define ANALOG_INPUT_CHANGE_THRESHOLD_TO_TRIGGER_EVENT 4

void (*brightnessAnalogInputChangedEventSubscribers[MAX_ANALOG_INPUT_SUBSCRIBERS])();
uint8_t currentBrightnessAnalogInputChangedEventSubscribersCount = 0;

int lastBrightnessAnalogValue; // Global brightness setting
int lastModeAnalogValue; // global mode selection

int currentBrightnessAnalogValue;
int currentSpeedAnalogValue;
int currentColorAnalogValue;
int currentModeAnalogValue;
unsigned long lastInputPollTime;

inline void notifyBrightnessAnalogInputChanged();

void setupAnalogInputs()
{
    #ifdef RP2040
    analogReadResolution(8);
    pinMode(BRIGHTNESS_ANALOG_INPUT_PIN, INPUT);
    pinMode(MODE_ANALOG_INPUT_PIN, INPUT);
    #endif
}

void readAnalogValues()
{
    if (getTime() - lastInputPollTime < ANALOG_INPUT_REFRESH_FREQUENCY) return;
    lastInputPollTime = getTime();

    #ifdef RP2040
    currentBrightnessAnalogValue = analogRead(BRIGHTNESS_ANALOG_INPUT_PIN);
    #endif
    int difference = lastBrightnessAnalogValue - currentBrightnessAnalogValue;
    if (D_abs(difference) > ANALOG_INPUT_CHANGE_THRESHOLD_TO_TRIGGER_EVENT)
    {
        lastBrightnessAnalogValue = currentBrightnessAnalogValue;
        D_serialWrite("Brightness: ");
        D_serialWrite(currentBrightnessAnalogValue);
        D_serialWrite("\n");
        notifyBrightnessAnalogInputChanged();
    }

    #ifdef RP2040
    currentModeAnalogValue = analogRead(MODE_ANALOG_INPUT_PIN);
    #endif
    if (D_abs(lastModeAnalogValue - currentModeAnalogValue) > ANALOG_INPUT_CHANGE_THRESHOLD_TO_TRIGGER_EVENT)
    {
        lastModeAnalogValue = currentModeAnalogValue;
        D_serialWrite("Mode: ");
        D_serialWrite(currentModeAnalogValue);
        D_serialWrite("\n");
    }
}

int getAnalogBrightnessSelection()
{
    return currentBrightnessAnalogValue;
}

int getAnalogSpeedSelection()
{
    return currentSpeedAnalogValue;
}

int getAnalogColorSelection()
{
    return currentColorAnalogValue;
}

int getAnalogModeSelection()
{
    return currentModeAnalogValue;
}

void subscribeToBrightnessAnalogInputChangedEvent(void (*subscriber)()) 
{
  if (currentBrightnessAnalogInputChangedEventSubscribersCount < MAX_ANALOG_INPUT_SUBSCRIBERS) {
    brightnessAnalogInputChangedEventSubscribers[currentBrightnessAnalogInputChangedEventSubscribersCount] = subscriber;
    currentBrightnessAnalogInputChangedEventSubscribersCount++;
  }
}

void unsubscribeFromBrightnessAnalogInputChangedEvent(void (*subscriber)()) 
{
    for (int i = 0; i < currentBrightnessAnalogInputChangedEventSubscribersCount; i++) 
    {
        if (brightnessAnalogInputChangedEventSubscribers[i] == subscriber) 
        {
            brightnessAnalogInputChangedEventSubscribers[i] = NULL;
            for (int j = i; j < currentBrightnessAnalogInputChangedEventSubscribersCount - 1; j++) 
            {
                brightnessAnalogInputChangedEventSubscribers[j] = brightnessAnalogInputChangedEventSubscribers[j + 1];
            }
            return;
        }
    }
}

inline void notifyBrightnessAnalogInputChanged() 
{
    for (int i = 0; i < currentBrightnessAnalogInputChangedEventSubscribersCount; i++) 
    {
        if (brightnessAnalogInputChangedEventSubscribers[i] != NULL) (*brightnessAnalogInputChangedEventSubscribers[i])();
    }
}