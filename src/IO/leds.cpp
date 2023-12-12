#ifdef RP2040
#include <Adafruit_NeoPXL8.h>
#else
#include "../../tests/testLeds.h"
#endif
#include "../IO/leds.h"
#include "../Graphics/effectController.h"
#include "../Graphics/color.h"
#include "../settings.h"

#define COLOR_ORDER                 NEO_GRB
#define LED_COUNT_PER_PIN           136
#define NUMBER_OF_USED_PINS         2
#define START_LED_OFFSET            0
#define END_LED_OFFSET              LED_COUNT_PER_PIN * NUMBER_OF_USED_PINS
#define BLEND_FRAMES                true
#define TEMPORAL_DITHERING_AMOUNT   4
#define DOUBLE_BUFFER               true
#define PIO_CONTROLLER              pio1
#define GAMMA_EXPONENT              2.2
#define MAX_DUTY_CYCLE              110

#define USE_SPECIAL_LED_ORDER_FOR_WINGS
#ifdef USE_SPECIAL_LED_ORDER_FOR_WINGS
#define SPECIAL_LED_ORDER_OFFSET    63
inline void specialLedOrderForWings();
#endif

static int8_t LED_PINS[8] =         {19, 20, 21, -1, -1, -1, -1, -1};
#ifdef RP2040
Adafruit_NeoPXL8HDR ledDisplay(LED_COUNT_PER_PIN, LED_PINS, COLOR_ORDER);
#endif
uint8_t currentBrightness = 255;

inline void normalLedOrder();
inline void setPixelColor(int pixelIndex, uint8_t red, uint8_t green, uint8_t blue);
inline void setBrightness(uint8_t brightness);
inline void refresh();
inline void show();
inline bool setupLedsInternal();

void setupLeds()
{
  if (setupLedsInternal()) 
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
  show();
}

void setGlobalLedBrightness(uint8_t brightness)
{
  brightness = (brightness * MAX_DUTY_CYCLE) / UINT8_MAX;
  if (currentBrightness != brightness)
  {
      currentBrightness = brightness;
      setBrightness(brightness);
  }
}

void refreshLeds()
{
    refresh();
}

#ifdef USE_SPECIAL_LED_ORDER_FOR_WINGS
inline void specialLedOrderForWings()
{
  int pixelIndex = START_LED_OFFSET;
  int offsetPixelIndex = 0;
  for(; pixelIndex < LED_COUNT_PER_PIN;) 
  {
    Color color = getLedColorForFrame(pixelIndex++);
    setPixelColor(offsetPixelIndex++, color.red, color.green, color.blue);
  }
  offsetPixelIndex += LED_COUNT_PER_PIN;
  for(; pixelIndex < LED_COUNT_PER_PIN + SPECIAL_LED_ORDER_OFFSET;) 
  {
    Color color = getLedColorForFrame(pixelIndex++);
    setPixelColor(offsetPixelIndex++, color.red, color.green, color.blue);
  }
  for(; pixelIndex < END_LED_OFFSET;) 
  {
    Color color = getLedColorForFrame(pixelIndex++);
    setPixelColor(offsetPixelIndex++, color.green, color.red, color.blue);
  }
}
#endif

inline void normalLedOrder()
{
  int pixelIndex = START_LED_OFFSET;
  int offsetPixelIndex = 0;
  for(; offsetPixelIndex < END_LED_OFFSET;) 
  {
    Color color = getLedColorForFrame(pixelIndex++);
    setPixelColor(offsetPixelIndex++, color.red, color.green, color.blue);
  }
}

inline void setPixelColor(int pixelIndex, uint8_t red, uint8_t green, uint8_t blue)
{
  #ifdef RP2040
  ledDisplay.setPixelColor(pixelIndex, red, green, blue);
  #else
  setTestLedsPixelColor(pixelIndex, red, green, blue);
  #endif
}

inline void setBrightness(uint8_t brightness)
{
  #ifdef RP2040
  ledDisplay.SetBrightness(brightness << 8, GAMMA_EXPONENT);
  #else
  setTestLedsBrightness(brightness << 8);
  #endif
}

inline void refresh()
{
  #ifdef RP2040
  ledDisplay.refresh();
  #else
  // This only really applies to hardware and should "just work". Does not need to be tested.
  #endif
}

inline void show()
{
  #ifdef RP2040
  ledDisplay.show();
  #else
  showTestLeds();
  #endif
}

inline bool setupLedsInternal()
{
  #ifdef RP2040
  return ledDisplay.begin(BLEND_FRAMES, TEMPORAL_DITHERING_AMOUNT, DOUBLE_BUFFER, PIO_CONTROLLER);
  #else
  return true;
  #endif
}