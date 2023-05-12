#include "../IO/serial.h"
#include <SoftwareSerial.h>

#define SERIAL_BAUD_RATE 115200

void setupSerial()
{
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);
  Serial.println("Serial Initalized.");
}

void readSerial()
{
  if (Serial.available())
  {
    int data = Serial.read();  
    delay(2000);
  }
}