#ifdef RP2040
#include <Adafruit_NeoPXL8.h>
#else
#include "../../windows/testLeds.h"
#endif

#include "leds.h"
#include "serial.h"
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
#define MAX_DUTY_CYCLE              255

#ifdef USE_SPECIAL_LED_ORDER_FOR_WINGS
#define SPECIAL_LED_ORDER_OFFSET    63
inline void specialLedOrderForWings();
#endif

#ifdef RP2040
static int8_t LED_PINS[8] =         {19, 20, 21, -1, -1, -1, -1, -1};
Adafruit_NeoPXL8HDR ledDisplay(LED_COUNT_PER_PIN, LED_PINS, COLOR_ORDER);
#endif
uint8_t currentBrightness = 255;
uint8_t brightnessAdjustedForMaxDutyCycle = 255;
bool ledsEnabled = true;

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
    D_serialWrite("Leds Initalized.");
  }
}

void renderLeds()
{
  if (!ledsEnabled) return; 
  #ifdef USE_SPECIAL_LED_ORDER_FOR_WINGS
  specialLedOrderForWings();
  #else
  normalLedOrder();
  #endif
  show();
}

void setGlobalLedBrightness(uint8_t brightness)
{
  brightnessAdjustedForMaxDutyCycle = (brightness * MAX_DUTY_CYCLE) / UINT8_MAX;
  if (currentBrightness != brightness)
  {
      currentBrightness = brightness;
      setBrightness(brightnessAdjustedForMaxDutyCycle);
  }
}

uint8_t getGlobalLedBrightness()
{
  return currentBrightness;
}

void refreshLeds()
{
  refresh();
}

void disableLeds()
{
    clearLeds();
    show();
    ledsEnabled = false;
}

void enableLeds()
{
    ledsEnabled = true;
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
  setTestLedsBrightness(brightness);
  #endif
}

inline void refresh()
{
  #ifdef RP2040
  ledDisplay.refresh();
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

inline void clearLeds()
{
  #ifdef RP2040
  ledDisplay.clear();
  #else
  clearTestLeds();
  #endif
}