#include <Adafruit_NeoPXL8.h>
#include "leds.h"
#include "microphone.h"

#define COLOR_ORDER NEO_GRB
#define LED_COUNT_PER_STRAND        136 // total number is 8x this
#define BLEND_FRAMES                true
#define TEMPORAL_DITHERING_AMOUNT   4
#define DOUBLE_BUFFER               true
#define PIO_CONTROLLER              pio1
#define GAMMA                       2.6
#define MAX_DUTY_CYCLE              65535 / 2
static int8_t LED_PINS[8] = {0,1,2,3,4,5,6,7};

Adafruit_NeoPXL8HDR ledDisplay(LED_COUNT_PER_STRAND, LED_PINS, COLOR_ORDER);

int x;
int y;
int color;

static uint8_t colors[8][3] = {
  255,   0,   0, // Row 0: Red
  255, 160,   0, // Row 1: Orange
  255, 255,   0, // Row 2: Yellow
    0, 255,   0, // Row 3: Green
    0, 255, 255, // Row 4: Cyan
    0,   0, 255, // Row 5: Blue
  192,   0, 255, // Row 6: Purple
  255,   0, 255  // Row 7: Magenta
};

static short ledMap[13][13] = { 
  {11, 10, 9 , 8 , 7 , 6 , 5 , 4 , 3 , 2 , 1 , 0 , 0 },
  {12, 13, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54},
  {14, 15, 65, 98, 97, 96, 95, 94, 93, 92, 91, 90, 53},
  {0 , 16, 66, 99,100,123,122,121,120,119,118, 89, 52},
  {0 , 17, 18, 68,101,124,125,126,135,134,117, 88, 51},
  {0 , 19, 20, 69, 70,103,104,127,128,133,116, 87, 50},
  {0 , 0 , 21, 22, 71, 72,105,106,129,132,115, 86, 49},
  {0 , 0 , 23, 24, 25, 73, 74,107,130,131,114, 85, 48},
  {0 , 0 , 0 , 26, 27, 28, 75, 76,109,110,113, 84, 47},
  {0 , 0 , 0 , 0 , 29, 30, 32, 77, 78,111,112, 83, 46},
  {0 , 0 , 0 , 0 , 0 , 31, 33, 34, 36, 80, 81, 82, 45},
  {0 , 0 , 0 , 0 , 0 , 0 , 0 , 35, 37, 38, 39, 41, 44},
  {66, 79,102,108, 0 , 0 , 0 , 0 , 0 , 0 , 40, 42, 43},
};

bool setupLeds()
{
  bool doubleBuffer = true;
  if (!ledDisplay.begin(BLEND_FRAMES, TEMPORAL_DITHERING_AMOUNT, DOUBLE_BUFFER, PIO_CONTROLLER)) return false;
  ledDisplay.setBrightness(MAX_DUTY_CYCLE, GAMMA);
  return true;
}

void refreshLeds()
{
    ledDisplay.refresh();
}

uint32_t rain(uint32_t now, uint8_t row, int pixelNum) {
  uint8_t frame = now / 4; // uint8_t rolls over for a 0-255 range
  uint16_t b = (256 - ((frame - row * 32 + pixelNum * 256 / LED_COUNT_PER_STRAND) & 0xFF)) * getAudioIntensityRatio();
  return ledDisplay.Color((colors[(color + pixelNum) % 8][0] * b) >> 8,
                    (colors[(color + pixelNum) % 8][1] * b) >> 8,
                    (colors[(color + pixelNum) % 8][2] * b) >> 8);
}

void renderLeds()
{
  int time = millis();
  for(uint8_t r=4; r<5; r++) { // For each row...
    for(int p=0; p<LED_COUNT_PER_STRAND;) { // For each pixel of row...
      short index = ledMap[x][y];
      if (index != 0 || y == 11)
      {
        ledDisplay.setPixelColor(r * LED_COUNT_PER_STRAND + index, rain(time, r, p));
        p++;
      }
      x = x + 1;
      if (x > 12)
      {
        x = 0;
        y = y + 1;
        if (y > 12) y = 0;
      }
    }
  }
  ledDisplay.show();
}