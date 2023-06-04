#include "../IO/analogInput.h"
#include "../Utility/time.h"
#include <Arduino.h>

#define MAX_ANALOG_INPUT_SUBSCRIBERS 10

#define BRIGHTNESS_ANALOG_INPUT_PIN (uint8_t)27U // This is dial 1
#define MODE_ANALOG_INPUT_PIN (uint8_t)28U // This is dial 2
#define ANALOG_INPUT_REFRESH_FREQUENCY 500
#define ANALOG_INPUT_CHANGE_THRESHOLD_TO_TRIGGER_EVENT 4

void (*brightnessAnalogInputChangedEventSubscribers[MAX_ANALOG_INPUT_SUBSCRIBERS])();
byte currentBrightnessAnalogInputChangedEventSubscribersCount = 0;

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
    analogReadResolution(8);
    pinMode(BRIGHTNESS_ANALOG_INPUT_PIN, INPUT);
    pinMode(MODE_ANALOG_INPUT_PIN, INPUT);
}

void readAnalogValues()
{
    if (getTime() - lastInputPollTime < ANALOG_INPUT_REFRESH_FREQUENCY) return;
    lastInputPollTime = getTime();

    currentBrightnessAnalogValue = analogRead(BRIGHTNESS_ANALOG_INPUT_PIN);
    if (abs(lastBrightnessAnalogValue - currentBrightnessAnalogValue) > ANALOG_INPUT_CHANGE_THRESHOLD_TO_TRIGGER_EVENT)
    {
        lastBrightnessAnalogValue = currentBrightnessAnalogValue;
        Serial.print("Brightness: ");
        Serial.println(currentBrightnessAnalogValue);
        notifyBrightnessAnalogInputChanged();
    }

    currentModeAnalogValue = analogRead(MODE_ANALOG_INPUT_PIN);
    if (abs(lastModeAnalogValue - currentModeAnalogValue) > ANALOG_INPUT_CHANGE_THRESHOLD_TO_TRIGGER_EVENT)
    {
        lastModeAnalogValue = currentModeAnalogValue;
        Serial.print("Mode: ");
        Serial.println(currentModeAnalogValue);
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
    printf("Subscriber added.\n");
  } else {
    printf("Maximum number of subscribers reached.\n");
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
            printf("Subscriber removed.\n");
            return;
        }
    }
    printf("Subscriber not found.\n");
}

inline void notifyBrightnessAnalogInputChanged() 
{
    for (int i = 0; i < currentBrightnessAnalogInputChangedEventSubscribersCount; i++) 
    {
        if (brightnessAnalogInputChangedEventSubscribers[i] != NULL) (*brightnessAnalogInputChangedEventSubscribers[i])();
    }
}