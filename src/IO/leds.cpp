#include <Adafruit_NeoPXL8.h>
#include "../IO/leds.h"
#include "../Graphics/effectController.h"
#include "../Graphics/color.h"
#include "../settings.h"

#define COLOR_ORDER                 NEO_GRB
#define LED_COUNT_PER_PIN           136
#define FIRST_USED_LED_PIN          0
#define LAST_USED_LED_PIN           4
#define START_LED_OFFSET            0
#define END_LED_OFFSET              LED_COUNT_PER_PIN * 2
#define BLEND_FRAMES                true
#define TEMPORAL_DITHERING_AMOUNT   4
#define DOUBLE_BUFFER               true
#define PIO_CONTROLLER              pio1
#define GAMMA                       2.2
#define MAX_DUTY_CYCLE              110

#define USE_SPECIAL_LED_ORDER_FOR_WINGS
#ifdef USE_SPECIAL_LED_ORDER_FOR_WINGS
#define SPECIAL_LED_ORDER_OFFSET    63
inline void specialLedOrderForWings();
#endif

static int8_t LED_PINS[8] =         {19, 20, 21, -1, -1, -1, -1, -1};
Adafruit_NeoPXL8HDR ledDisplay(LED_COUNT_PER_PIN, LED_PINS, COLOR_ORDER);
byte currentBrightness = 255;

inline void normalLedOrder();
void analogBrightnessChangedHandler();

void setupLeds()
{
  if (ledDisplay.begin(BLEND_FRAMES, TEMPORAL_DITHERING_AMOUNT, DOUBLE_BUFFER, PIO_CONTROLLER)) 
  {
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

void setGlobalLedBrightness(uint8_t brightness)
{
  brightness = (brightness * MAX_DUTY_CYCLE) / UINT8_MAX;
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
  int offsetPixelIndex = 0;
  for(; pixelIndex < LED_COUNT_PER_PIN;) 
  {
    Color color = getLedColorForFrame(pixelIndex++);
    ledDisplay.setPixelColor(offsetPixelIndex++, color.red, color.green, color.blue);
  }
  offsetPixelIndex += LED_COUNT_PER_PIN;
  for(; pixelIndex < LED_COUNT_PER_PIN + SPECIAL_LED_ORDER_OFFSET;) 
  {
    Color color = getLedColorForFrame(pixelIndex++);
    ledDisplay.setPixelColor(offsetPixelIndex++, color.red, color.green, color.blue);
  }
  for(; pixelIndex < LED_COUNT_PER_PIN * 2;) 
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