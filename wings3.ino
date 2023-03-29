#include <Adafruit_NeoPXL8.h>

#include <PDM.h>
#include "SingleEMAFilterLib.h"
#include <PeakDetection.h> 

#define COLOR_ORDER NEO_GRB
#define NUM_LEDS    136      // NeoPixels PER STRAND, total number is 8X this!
#define BAUD_RATE   9600
#define BATCH_SIZE 256
int8_t pins[8] = {0,1,2,3,4,5,6,7};

Adafruit_NeoPXL8HDR leds(NUM_LEDS, pins, COLOR_ORDER);

static const char channels = 1;
static const int frequency = 16000;
short sampleBuffer[512];
volatile int samplesRead;

SingleEMAFilter<int> singleEMAFilter(.25);

PeakDetection peakDetection;

short ledMap[13][13] = { 
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

short ledMap[13][13] = { 
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

  peakDetection.begin(48, 3, 0.6); 
}

int color = 0;
int count = 0;
int audioSamplesRead = 0;
float audioSampleTotal = 0.0;
float rms = 1.0;
float rmsMax = 1.0;

int temp2 = 1000;
int lastPeak = 0;
bool on = true;

int x;
int y;

void loop() {
  uint32_t now = millis(); // Get time once at start of each frame
  for(uint8_t r=4; r<5; r++) { // For each row...
    for(int p=0; p<NUM_LEDS;) { // For each pixel of row...
      short index = ledMap[x][y];
      if (index != 0 || y == 11)
      {
        leds.setPixelColor(r * NUM_LEDS + index, rain(now, r, p));
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
        peakDetection.add(rms);
        int peak = peakDetection.getPeak();
        double filtered = peakDetection.getFilt();
        if (lastPeak != peak)
        {
            lastPeak = peak;
            if (lastPeak == 1) on = !on;
        }
        if (rms > rmsMax)
        {
          rmsMax = rms;
        }
        if (rmsMax > 1) rmsMax = rmsMax - (rmsMax / 500);
        singleEMAFilter.AddValue(rms);
        Serial.print(rms);
        Serial.print(",");
        Serial.print(singleEMAFilter.GetLowPass());
        Serial.print(",");
        Serial.print(singleEMAFilter.GetHighPass());
        Serial.print(",");
        Serial.print(rmsMax);
        Serial.print(",");
        Serial.print(peak * 1000);
        Serial.print(",");
        Serial.print(on * 1000);
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
  uint16_t b2 = (256 - ((frame - row * 32 + pixelNum * 256 / NUM_LEDS) & 0xFF));
  if (on)
  {
    return leds.Color((colors[(color + pixelNum) % 8][1] * b2) >> 8,
                    (colors[(color + pixelNum) % 8][2] * b2) >> 8,
                    (colors[(color + pixelNum) % 8][0] * b2) >> 8);
  }
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
