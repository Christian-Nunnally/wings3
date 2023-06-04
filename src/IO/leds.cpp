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
#define GAMMA                       2.2
#define MAX_DUTY_CYCLE              128

#define USE_SPECIAL_LED_ORDER_FOR_WINGS
#ifdef USE_SPECIAL_LED_ORDER_FOR_WINGS
#define SPECIAL_LED_ORDER_OFFSET    63
inline void specialLedOrderForWings();
#endif

static int8_t LED_PINS[8] =         {0,1,2,3,4,5,6,7};
Adafruit_NeoPXL8HDR ledDisplay(LED_COUNT_PER_PIN, LED_PINS, COLOR_ORDER);
byte currentBrightness;

inline void normalLedOrder();
void analogBrightnessChangedHandler();

void setupLeds()
{
    if (ledDisplay.begin(BLEND_FRAMES, TEMPORAL_DITHERING_AMOUNT, DOUBLE_BUFFER, PIO_CONTROLLER)) 
    {
      subscribeToBrightnessAnalogInputChangedEvent(analogBrightnessChangedHandler);
      currentBrightness = MAX_DUTY_CYCLE;
      setGlobalLedBrightness(currentBrightness);
      #ifdef ENABLE_SERIAL 
      Serial.println("Leds Initalized.");
      #endif
    }
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

void analogBrightnessChangedHandler()
{
  uint8_t brightness = getAnalogBrightnessSelection();
  setGlobalLedBrightness(brightness);
}

void setGlobalLedBrightness(uint8_t brightness)
{
  if (currentBrightness != brightness)
  {
      currentBrightness = brightness;
      ledDisplay.setBrightness(brightness << 8, GAMMA);
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
    ledDisplay.setPixelColor(offsetPixelIndex++, color.red, color.green, color.blue);
  }
  for(; offsetPixelIndex < END_LED_OFFSET;) 
  {
    Color color = getLedColorForFrame(pixelIndex++);
    ledDisplay.setPixelColor(offsetPixelIndex++, color.green, color.red, color.blue);
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