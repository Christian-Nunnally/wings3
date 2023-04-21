#include "analogInput.h"
#include "time.h"
#include <Arduino.h>

#define MAX_ANALOG_INPUT_SUBSCRIBERS 10

#define BRIGHTNESS_ANALOG_INPUT_PIN (uint8_t)26U
#define SPEED_ANALOG_INPUT_PIN (uint8_t)27U
#define COLOR_ANALOG_INPUT_PIN (uint8_t)28U
#define MODE_ANALOG_INPUT_PIN (uint8_t)29U
#define ANALOG_INPUT_REFRESH_FREQUENCY 500

void (*brightnessAnalogInputChangedEventSubscribers[MAX_ANALOG_INPUT_SUBSCRIBERS])();
byte currentBrightnessAnalogInputChangedEventSubscribersCount = 0;

int lastBrightnessAnalogValue;
int lastSpeedAnalogValue;
int lastColorAnalogValue;
int lastModeAnalogValue;

int currentBrightnessAnalogValue;
int currentSpeedAnalogValue;
int currentColorAnalogValue;
int currentModeAnalogValue;
unsigned long lastInputPollTime;

inline void notifyBrightnessAnalogInputChanged();

void setupAnalogInputs()
{
    pinMode(BRIGHTNESS_ANALOG_INPUT_PIN, INPUT);
    pinMode(SPEED_ANALOG_INPUT_PIN, INPUT);
    pinMode(COLOR_ANALOG_INPUT_PIN, INPUT);
    pinMode(MODE_ANALOG_INPUT_PIN, INPUT);
}

void readAnalogValues()
{
    if (getTime() - lastInputPollTime < ANALOG_INPUT_REFRESH_FREQUENCY) return;
    lastInputPollTime = getTime();

    currentBrightnessAnalogValue = analogRead(BRIGHTNESS_ANALOG_INPUT_PIN);
    if (lastBrightnessAnalogValue != currentBrightnessAnalogValue)
    {
        lastBrightnessAnalogValue = currentBrightnessAnalogValue;
        notifyBrightnessAnalogInputChanged();
    }

    currentSpeedAnalogValue = analogRead(SPEED_ANALOG_INPUT_PIN);
    currentColorAnalogValue = analogRead(COLOR_ANALOG_INPUT_PIN);
    currentModeAnalogValue = analogRead(MODE_ANALOG_INPUT_PIN);
}

int getAnalogBrightnessSelection()
{
    return 200;
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