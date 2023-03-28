#include <Adafruit_NeoPXL8.h>

#include <PDM.h>
#include "SingleEMAFilterLib.h"

#define COLOR_ORDER NEO_GRB
#define NUM_LEDS    136      // NeoPixels PER STRAND, total number is 8X this!
#define BAUD_RATE   9600
#define BATCH_SIZE 512
int8_t pins[8] = {0,1,2,3,4,5,6,7};

Adafruit_NeoPXL8HDR leds(NUM_LEDS, pins, COLOR_ORDER);

static const char channels = 1;
static const int frequency = 16000;
short sampleBuffer[512];
volatile int samplesRead;

SingleEMAFilter<int> singleEMAFilter(.25);

void loop1() {
  leds.refresh();
}

void setup() {
  Serial.begin(BAUD_RATE);
  while (!Serial);
  Serial.println("Serial Initalized.");
  initializeLEDs();
  PDM.onReceive(onPDMdata);
  if (!PDM.begin(channels, frequency)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
}

int color = 0;
int count = 0;
int audioSamplesRead = 0;
float audioSampleTotal = 0.0;
float rms = 1.0;
float rmsMax = 1.0;
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
  if (count > 80) {
    //Serial.println("l");
    count = 0;
    color = random(8);
  }

  if (samplesRead) {
    for (int i = 0; i < samplesRead; i++) {
      //Serial.println(sampleBuffer[i]);
      
      // singleEMAFilter.AddValue(sampleBuffer[i]);
      // Serial.print(sampleBuffer[i]);
      // Serial.print(",\t");
      // Serial.print(singleEMAFilter.GetLowPass());
      // Serial.print(",\t");
      // Serial.println(singleEMAFilter.GetHighPass());

      audioSampleTotal += sampleBuffer[i] * sampleBuffer[i];
      audioSamplesRead += 1;
      if (audioSamplesRead == BATCH_SIZE)
      {
        rms = sqrt(audioSampleTotal / audioSamplesRead);
        audioSamplesRead = 0;
        audioSampleTotal = 0;
        if (rms > rmsMax)
        {
          rmsMax = rms;
        }
        if (rmsMax > 1) rmsMax = rmsMax - 1;
        singleEMAFilter.AddValue(rms);
        Serial.print(rms);
        Serial.print(",");
        Serial.print(singleEMAFilter.GetLowPass());
        Serial.print(",");
        Serial.print(singleEMAFilter.GetHighPass());
        Serial.print(",");
        Serial.print(rmsMax);
        Serial.print(",");
        Serial.println(1200);
      }
    

    }
    samplesRead = 0;
  }
}

void initializeLEDs()
{
  bool blendFrames = true;
  uint8_t temporalDitheringAmount = 4; // 0-8
  bool doubleBuffer = true;
  if (!leds.begin(blendFrames, temporalDitheringAmount, doubleBuffer, pio1) && Serial) 
  {
    Serial.println("Error: Not enough RAM for LEDs.");
  }
  float gamma = 2.6;
  uint16_t dutyCycle = 65535;
  leds.setBrightness(dutyCycle, gamma);
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

uint32_t rain(uint32_t now, uint8_t row, int pixelNum) {
  uint8_t frame = now / 4; // uint8_t rolls over for a 0-255 range
  uint16_t b = (256 - ((frame - row * 32 + pixelNum * 256 / NUM_LEDS) & 0xFF)) * (rms / rmsMax);
  return leds.Color((colors[(color + pixelNum) % 8][0] * b) >> 8,
                    (colors[(color + pixelNum) % 8][1] * b) >> 8,
                    (colors[(color + pixelNum) % 8][2] * b) >> 8);
}

/**
   Callback function to process the data from the PDM microphone.
   NOTE: This callback is executed as part of an ISR.
   Therefore using `Serial` to print messages inside this function isn't supported.
 * */
void onPDMdata() {
  // Query the number of available bytes
  int bytesAvailable = PDM.available();

  // Read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
