#define RP2040
#define USE_SPECIAL_LED_ORDER_FOR_WINGS
//#define ENABLE_SERIAL_WRITE
//#define ENABLE_SERIAL_READ

#include "commonRP2040Headers.h"
#include "../commonHeaders.h"
#include "../settings.h"
#include "../Peripherals/microphone.h"
#include "../Peripherals/movementDetection.h"
#include "../IO/leds.h"
#include "../IO/serial.h"
#include "../IO/analogInput.h"
#include "../Graphics/effectController.h"
#include "../Graphics/colorMixing.h"
#include "../Utility/time.h"
#include "../Utility/fastRandom.h"
#include "../Control/initializeAndRun.h"


void setup() 
{
  D_serialBegin();
  initialize();
}

void loop() 
{
  run();
  D_serialRead();
}

void loop1() 
{
  refreshLeds();
}