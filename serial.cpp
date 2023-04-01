#include "serial.h"
#include <SoftwareSerial.h>

#define SERIAL_BAUD_RATE 9600

void setupSerial()
{
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);
  Serial.println("Serial Initalized.");
}