
// #include <Adafruit_NeoPXL8.h>

// int8_t pins[8] = {19, 20, 21, -1, -1, -1, -1, -1};
// #define COLOR_ORDER NEO_GRB
// #define NUM_LEDS    4 

// Adafruit_NeoPXL8HDR leds(NUM_LEDS, pins, COLOR_ORDER);

// void setup() {
//   if (!leds.begin(true, 4, true)) {
//     pinMode(LED_BUILTIN, OUTPUT);
//     for (;;) digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
//   }

//   leds.setBrightness(65535 / 8, 2.6);
// }

// int i =0;
// void loop() {
//   for (int j = 0; j < 12; j++)
//   {
//     leds.setPixelColor(j, leds.Color((255 + i + j) % 255, (128 + i + j) % 255, (64 + i + j) % 255));
//   }
//   i++;
//   leds.show();
//   delay(30);
// }

// void loop1() 
// {
//   leds.refresh();
// }

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

void setup() 
{
  #ifdef ENABLE_SERIAL 
  setupSerial();
  #endif
  setupLeds();
  setupMicrophone();
  //setupImu();
  setupEffects();
  // setupAnalogInputs();
}

void loop() 
{
  setTime(millis());
  incrementEffectFrame();
  renderLeds();
  processAudioStream();
  //checkForMovement();
  // readAnalogValues();
  #ifdef ENABLE_SERIAL 
  readSerial();
  #endif
}

void loop1() 
{
  refreshLeds();
}