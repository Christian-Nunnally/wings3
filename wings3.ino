#include "microphone.h"
#include "leds.h"
#include "serial.h"
#include "movementDetection.h"
#include "effectController.h"
#include "graphics.h"
#include "time.h"
#include "analogInput.h"

unsigned long lastTime;

void setup() 
{
  setupSerial();
  setupLeds();
  setupMicrophone();
  setupImu();
  setupEffects();
  setupAnalogInputs();
}

void loop() 
{
  setTime(millis());
  incrementEffectFrame();
  renderLeds();
  processAudioStream();
  checkForMovement();
  readAnalogValues();

  Serial.print("ft: ");
  Serial.println(getTime() - lastTime);
  lastTime = getTime();
}

void loop1() 
{
  refreshLeds();
}