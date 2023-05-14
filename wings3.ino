#include <Arduino.h>
#include "src/Peripherals/microphone.h"
#include "src/Peripherals/movementDetection.h"
#include "src/IO/leds.h"
#include "src/IO/serial.h"
#include "src/IO/analogInput.h"
#include "src/Graphics/effectController.h"
#include "src/Graphics/colorMixing.h"
#include "src/Utility/time.h"
#include "src/Utility/fastRandom.h"
#include "src/settings.h"

unsigned long lastTime;

void setup() 
{
  #ifdef ENABLE_SERIAL 
  setupSerial();
  #endif
  setupLeds();
  setupMicrophone();
  setupImu();
  setupEffects();
  setupAnalogInputs();
}

void loop() 
{
  // int time = millis();
  // for (int i = 0; i < 100000; i++)
  // {
  //   blendColorsUsingMixing3({(uint16_t)fastRandomInteger(65536), (uint16_t)fastRandomInteger(65536), (uint16_t)fastRandomInteger(65536)}, {(uint16_t)fastRandomInteger(65536), (uint16_t)fastRandomInteger(65536), (uint16_t)fastRandomInteger(65536)}, (uint16_t)fastRandomInteger(65536));
  // }
  // int newTime = millis();
  // Serial.print("Blending time for 100k: ");
  // Serial.println(newTime - time);

  setTime(millis());
  incrementEffectFrame();
  renderLeds();
  processAudioStream();
  checkForMovement();
  readAnalogValues();
  #ifdef ENABLE_SERIAL 
  readSerial();
  #endif
}

void loop1() 
{
  refreshLeds();
}