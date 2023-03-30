#include "microphone.h"
#include "leds.h"
#include "serial.h"

void setup() 
{
  setupSerial();
  setupLeds();
  setupMicrophone();
}

void loop() 
{
  renderLeds();
  processAudioStream();
}

void loop1() 
{
  refreshLeds();
}