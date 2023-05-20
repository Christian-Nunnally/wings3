#include <Adafruit_NeoPXL8.h>
#include "../IO/leds.h"
#include "../IO/analogInput.h"
#include "../Peripherals/microphone.h"
#include "../Graphics/effectController.h"
#include "../Graphics/color.h"
#include "../settings.h"

#define COLOR_ORDER                 NEO_GRB
#define LED_COUNT_PER_PIN           136
#define FIRST_USED_LED_PIN          4
#define LAST_USED_LED_PIN           5
#define START_LED_OFFSET            LED_COUNT_PER_PIN * FIRST_USED_LED_PIN
#define END_LED_OFFSET              LED_COUNT_PER_PIN * (LAST_USED_LED_PIN + 1)
#define BLEND_FRAMES                true
#define TEMPORAL_DITHERING_AMOUNT   4
#define DOUBLE_BUFFER               true
#define PIO_CONTROLLER              pio1
#define GAMMA                       2.8
#define MAX_DUTY_CYCLE              65535 / 2

#define USE_SPECIAL_LED_ORDER_FOR_WINGS
#ifdef USE_SPECIAL_LED_ORDER_FOR_WINGS
#define SPECIAL_LED_ORDER_OFFSET    33
inline void specialLedOrderForWings();
#endif

static int8_t LED_PINS[8] =         {0,1,2,3,4,5,6,7};
Adafruit_NeoPXL8HDR ledDisplay(LED_COUNT_PER_PIN, LED_PINS, COLOR_ORDER);
byte currentBrightness;

inline void normalLedOrder();

bool setupLeds()
{
    if (!ledDisplay.begin(BLEND_FRAMES, TEMPORAL_DITHERING_AMOUNT, DOUBLE_BUFFER, PIO_CONTROLLER)) return false;
    setGlobalLedBrightness(currentBrightness);
    return true;
}

void renderLeds()
{
  #ifdef USE_SPECIAL_LED_ORDER_FOR_WINGS
  specialLedOrderForWings();
  #else
  normalLedOrder();
  #endif
  ledDisplay.show();
}

void setGlobalLedBrightness(uint16_t brightness)
{
  brightness = MAX_DUTY_CYCLE ;//(MAX_DUTY_CYCLE * brightness) >> 8;
  if (currentBrightness != brightness)
  {
      currentBrightness = brightness;
      ledDisplay.setBrightness(brightness, GAMMA);
  }
}

void refreshLeds()
{
    ledDisplay.refresh();
}

#ifdef USE_SPECIAL_LED_ORDER_FOR_WINGS
inline void specialLedOrderForWings()
{
  int pixelIndex = 0;
  int offsetPixelIndex = START_LED_OFFSET;
  for(; pixelIndex < LED_COUNT_PER_PIN;) 
  {
    Color color = getLedColorForFrame(pixelIndex++);
    ledDisplay.setPixelColor(offsetPixelIndex++, color.red, color.green, color.blue);
  }
  for(; pixelIndex < LED_COUNT_PER_PIN + SPECIAL_LED_ORDER_OFFSET;) 
  {
    Color color = getLedColorForFrame(pixelIndex++);
    ledDisplay.setPixelColor(offsetPixelIndex++, color.green, color.red, color.blue);
  }
  for(; offsetPixelIndex < END_LED_OFFSET;) 
  {
    Color color = getLedColorForFrame(pixelIndex++);
    ledDisplay.setPixelColor(offsetPixelIndex++, color.red, color.green, color.blue);
  }
}
#endif

inline void normalLedOrder()
{
  int pixelIndex = 0;
  int offsetPixelIndex = START_LED_OFFSET;
  for(; offsetPixelIndex < END_LED_OFFSET;) 
  {
    Color color = getLedColorForFrame(pixelIndex++);
    ledDisplay.setPixelColor(offsetPixelIndex++, color.red, color.green, color.blue);
  }
}