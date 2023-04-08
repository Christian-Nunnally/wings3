#include "microphone.h"
#include "leds.h"
#include "serial.h"
#include "movementDetection.h"
#include "effectController.h"

void setup() 
{
  setupSerial();
  setupLeds();
  setupMicrophone();
  setupImu();
  setupEffects();
}

int loopCount2;
int differenceTotal = 0;
void loop() 
{
  incrementEffectFrame();
  unsigned long t1 = millis();
  renderLeds();
  unsigned long t2 = millis();
  processAudioStream();
  checkForMovement();
  differenceTotal += t2 - t1;
  if (loopCount2++ % 10 == 0)
  {
    Serial.println(differenceTotal / 10.0);
    differenceTotal = 0;
  }
}

void loop1() 
{
  refreshLeds();
}