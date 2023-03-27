/*
*** FIRST TIME HERE? START W/THE NEOPXL8 (not HDR) strandtest INSTEAD! ***
That code explains and helps troubshoot wiring and NeoPixel color format.

Adafruit_NeoPXL8HDR is a subclass of Adafruit_NeoPXL8 that provides
16-bits-per-channel color, temporal dithering, and gamma correction.
This is a minimal demonstration of "HDR" vs regular NeoPXL8 to show the
extra setup required. It runs an animation loop similar to the non-HDR
strandtest example, with the color fills on startup removed...those are
for troubleshooting wiring and color order...work through that in the
easier non-HDR strandtest and then copy your findings here. You'll
find MUCH more documentation in that example.

Most important thing to note here is that different chips (RP2040, SAMD51)
require very different startup code. Animation loop is identical to NeoPXL8,
just the setup changes. SAMD21 is NOT supported; HDR is just too demanding.
SAMD51 requires the Adafruit_ZeroTimer library.
*/

#include <Adafruit_NeoPXL8.h>

int8_t pins[8] = {0,1,2,3,4,5,6,7};
#define COLOR_ORDER NEO_GRB // NeoPixel color format (see Adafruit_NeoPixel)
#define NUM_LEDS    136      // NeoPixels PER STRAND, total number is 8X this!

Adafruit_NeoPXL8HDR leds(NUM_LEDS, pins, COLOR_ORDER);

void loop1() {
  leds.refresh();
}

void setup() {

  // In Adafruit_NeoPXL8, begin() normally doesn't take arguments. The HDR
  // version has some extra flags to select frame blending, added "depth"
  // for temporal dithering, and whether to double-buffer DMA pixel writes:
  if (!leds.begin(true, 4, true)) {
    // If begin() returns false, either an invalid pin list was provided,
    // or requested too many pixels for available memory (HDR requires
    // INORDINATE RAM). Blink onboard LED to indicate startup problem.
    pinMode(LED_BUILTIN, OUTPUT);
    for (;;) digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
  }

  // NeoPXL8HDR can provide automatic gamma-correction, but it's disabled
  // by default; pixel values correlate to linear duty cycle. This is so
  // existing NeoPixel or NeoPXL8 code brought over to NeoPXL8HDR looks the
  // same, no surprises. When reworking a project for HDR, it's best to
  // enable, animation code then doesn't need to process this. Gamma curve
  // (2.6 in this example) is set at the same time as global brightness.
  // Notice the brightness uses a 16-bit value (0-65535), not 8-bit
  // (0-255) as in NeoPixel and NeoPXL8:
  leds.setBrightness(65535/8, 2.6); // 1/8 max duty cycle, 2.6 gamma factor
}

int color = 0;
int count = 0;
// ANIMATION LOOP (similar to "regular" NeoPXL8) ---------------------------

// The loop() function is identical to the non-HDR example. Although
// NeoPXL8HDR can handle 16-bits-per-channel colors, we're using 8-bit
// here (packed into a 32-bit value) to show how existing NeoPixel or
// NeoPXL8 code can carry over directly. The library will expand these
// to 16 bits behind the scenes.
void loop() {
  uint32_t now = millis(); // Get time once at start of each frame
  for(uint8_t r=0; r<8; r++) { // For each row...
    for(int p=0; p<NUM_LEDS; p++) { // For each pixel of row...
      leds.setPixelColor(r * NUM_LEDS + p, rain(now, r, p));
    }
  }
  leds.show();
  count++;
  if (count > 1200) {
    count = 0;
    color = random(8);
  }
}


// Pixel strand colors, same code as non-HDR strandtest:
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

// This function changes very slightly from the non-HDR strandtest example.
// That code had to apply gamma correction on is own. It's not necessary
// with the HDR class once configured (see setBrightness() in setup()),
// we get that "free" now.
uint32_t rain(uint32_t now, uint8_t row, int pixelNum) {
  uint8_t frame = now / 4; // uint8_t rolls over for a 0-255 range
  uint16_t b = 256 - ((frame - row * 32 + pixelNum * 256 / NUM_LEDS) & 0xFF);
  return leds.Color((colors[(color + pixelNum) % 8][0] * b) >> 8,
                    (colors[(color + pixelNum) % 8][1] * b) >> 8,
                    (colors[(color + pixelNum) % 8][2] * b) >> 8);
}
