#define RP2040
#define USE_SPECIAL_LED_ORDER_FOR_WINGS
//#define ENABLE_SERIAL_WRITE
//#define ENABLE_SERIAL_READ

#include <ADCInput.h>
#include "src/settings.h"
#include "src/commonHeaders.h"
#include "src/Peripherals/microphone.h"
#include "src/Peripherals/movementDetection.h"
#include "src/IO/leds.h"
#include "src/IO/serial.h"
#include "src/IO/analogInput.h"
#include "src/Graphics/effectController.h"
#include "src/Graphics/colorMixing.h"
#include "src/Utility/time.h"
#include "src/Utility/fastRandom.h"


void setup() 
{
  D_serialBegin();
  setupLeds();
  setupMicrophone();
  //setupImu();
  setupEffects();
  //setupAnalogInputs();
}

void loop() 
{
  setTime(millis());
  incrementEffectFrame();
  renderLeds();
  processAudioStream();
  //checkForMovement();
  //readAnalogValues();
  D_serialRead();
}

void loop1() 
{
  refreshLeds();
}