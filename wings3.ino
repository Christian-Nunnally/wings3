#include "microphone.h"
#include "leds.h"
#include "serial.h"
#include "movementDetection.h"

void setup() 
{
  setupSerial();
  setupLeds();
  setupMicrophone();
  setupImu();
}

void loop() 
{
  renderLeds();
  processAudioStream();
  checkForMovement();
}

void loop1() 
{
  refreshLeds();
}