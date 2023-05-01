#include "src/Peripherals/microphone.h"
#include "src/Peripherals/movementDetection.h"
#include "src/IO/leds.h"
#include "src/IO/serial.h"
#include "src/IO/analogInput.h"
#include "src/Graphics/effectController.h"
#include "src/Graphics/graphics.h"
#include "src/Utility/time.h"

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

  // Serial.print("ft: ");
  // Serial.println(getTime() - lastTime);
  // lastTime = getTime();
}

void loop1() 
{
  refreshLeds();
}