#include "../IO/serial.h"
#include "../IO/leds.h"
#include <SoftwareSerial.h>

#define SERIAL_BAUD_RATE 115200

void setupSerial()
{
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);
  Serial.println("Serial Initalized.");
}

bool templol = false;
void readSerial()
{
  if (Serial.available())
  {
    int data = Serial.read();
    if (templol)
    {
      setGlobalLedBrightness(UINT16_MAX);
    }
    else 
    {
      setGlobalLedBrightness(0);
    }
    templol = !templol;
  }
}