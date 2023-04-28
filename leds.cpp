#include <Adafruit_NeoPXL8.h>
#include "leds.h"
#include "microphone.h"
#include "graphics.h"
#include "settings.h"
#include "effectController.h"
#include "analogInput.h"

#define COLOR_ORDER                 NEO_GRB
#define LED_COUNT_PER_PIN           136
#define START_LED_OFFSET            LED_COUNT_PER_PIN * 4
#define BLEND_FRAMES                true
#define TEMPORAL_DITHERING_AMOUNT   4
#define DOUBLE_BUFFER               true
#define PIO_CONTROLLER              pio1
#define GAMMA                       2.6
#define MAX_DUTY_CYCLE              65535// / 3
static int8_t LED_PINS[8] =         {0,1,2,3,4,5,6,7};

Adafruit_NeoPXL8HDR ledDisplay(LED_COUNT_PER_PIN, LED_PINS, COLOR_ORDER);
byte currentBrightness;

bool setupLeds()
{
    bool doubleBuffer = true;
    if (!ledDisplay.begin(BLEND_FRAMES, TEMPORAL_DITHERING_AMOUNT, DOUBLE_BUFFER, PIO_CONTROLLER)) return false;
    ledDisplay.setBrightness(MAX_DUTY_CYCLE, GAMMA);
    //subscribeToBrightnessAnalogInputChangedEvent(setBrightness);
    return true;
}

void renderLeds()
{
  if (!isMusicDetected()) ledDisplay.fill(0, 0, 0);
  else 
  {
    for(int pixelIndex; pixelIndex < LED_COUNT_PER_PIN; pixelIndex++) 
    {
      Color color = getLedColorForFrame(pixelIndex);
      ledDisplay.set16(START_LED_OFFSET + pixelIndex, color.red, color.green, color.blue);
    }
  }
  ledDisplay.show();
}

void setBrightness()
{
  // int brightness = getAnalogBrightnessSelection();
  // if (currentBrightness != brightness)
  // {
  //     ledDisplay.setBrightness(brightness << 6, GAMMA);
  // }
}

void refreshLeds()
{
    ledDisplay.refresh();
}