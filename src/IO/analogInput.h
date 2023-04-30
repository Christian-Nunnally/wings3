#ifndef ANALOG_INPUT_H
#define ANALOG_INPUT_H

void setupAnalogInputs();
void readAnalogValues();
int getAnalogBrightnessSelection();
int getAnalogSpeedSelection();
int getAnalogColorSelection();
int getAnalogModeSelection();

void subscribeToBrightnessAnalogInputChangedEvent(void (*subscriber)());
void unsubscribeFromBrightnessAnalogInputChangedEvent(void (*subscriber)());

#endif