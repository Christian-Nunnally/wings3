#include "microphone.h"
#include "leds.h"
#include "serial.h"
#include "movementDetection.h"
#include "effectController.h"
#include "graphics.h"

void setup() 
{
  setupSerial();
  setupLeds();
  setupMicrophone();
  setupImu();
  setupEffects();
}

void loop() 
{
  incrementEffectFrame();
  renderLeds();
  processAudioStream();
  checkForMovement();
}

void loop1() 
{
  refreshLeds();
}