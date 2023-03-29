#include <Adafruit_NeoPXL8.h>
#include <PDM.h>
#include <PeakDetection.h> 
#include <SingleEMAFilterLib.h>

#define COLOR_ORDER NEO_GRB
#define LED_COUNT_PER_STRAND        136 // total number is 8x this
#define SERIAL_BAUD_RATE            9600
#define AUDIO_SAMPLE_BATCH_SIZE     256
#define MICROPHONE_SAMPLE_FREQUENCY 16000
static int8_t LED_PINS[8] = {0,1,2,3,4,5,6,7};

Adafruit_NeoPXL8HDR ledDisplay(LED_COUNT_PER_STRAND, LED_PINS, COLOR_ORDER);

short audioSampleBuffer[512];
volatile int numberOfAudioSamplesRead;

SingleEMAFilter<int> singleEMAFilter(.25);

PeakDetection peakDetector;

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

void loop1() {
  ledDisplay.refresh();
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);
  Serial.println("Serial Initalized.");
  initializeLEDs();
  PDM.onReceive(onPDMdata);
  static const int channels = 1;
  if (!PDM.begin(channels, MICROPHONE_SAMPLE_FREQUENCY)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }

  peakDetector.begin(48, 3, 0.6); 
}

int color = 0;
int audioSamplesRead = 0;
float audioSampleTotal = 0.0;
float rms = 1.0;
float rmsMax = 1.0;

int temp2 = 1000;
int lastPeak = 0;
bool on = true;

int x;
int y;

uint32_t animationTime;
uint32_t oldMillis;

void loop() {
  float scale = ((rms/rmsMax) + 1.0);
  animationTime = animationTime + (uint32_t)(((millis() - oldMillis) + rmsMax) * scale); // Get time once at start of each frame
  oldMillis = millis();
  for(uint8_t r=4; r<5; r++) { // For each row...
    for(int p=0; p<LED_COUNT_PER_STRAND;) { // For each pixel of row...
      short index = ledMap[x][y];
      if (index != 0 || y == 11)
      {
        ledDisplay.setPixelColor(r * LED_COUNT_PER_STRAND + index, rain(animationTime, r, p));
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

  if (numberOfAudioSamplesRead) {
    for (int i = 0; i < numberOfAudioSamplesRead; i++) {
      audioSampleTotal += audioSampleBuffer[i] * audioSampleBuffer[i];
      audioSamplesRead += 1;
      if (audioSamplesRead == AUDIO_SAMPLE_BATCH_SIZE)
      {
        rms = sqrt(audioSampleTotal / audioSamplesRead);
        audioSamplesRead = 0;
        audioSampleTotal = 0;
        singleEMAFilter.AddValue(rms);
        peakDetector.add(singleEMAFilter.GetLowPass());
        int peak = peakDetector.getPeak();
        double filtered = peakDetector.getFilt();
        if (lastPeak != peak)
        {
            lastPeak = peak;
            if (lastPeak == 1) on = !on;
            color = random(8);
        }
        if (rms > rmsMax)
        {
          rmsMax = rms;
        }
        if (rmsMax > 1) rmsMax = rmsMax - (rmsMax / 500);
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
    numberOfAudioSamplesRead = 0;
  }
}

void initializeLEDs()
{
  bool blendFrames = true;
  uint8_t temporalDitheringAmount = 4; // 0-8
  bool doubleBuffer = true;
  if (!ledDisplay.begin(blendFrames, temporalDitheringAmount, doubleBuffer, pio1) && Serial) 
  {
    Serial.println("Error: Not enough RAM for LEDs.");
  }
  float gamma = 2.6;
  uint16_t dutyCycle = 65535 / 2;
  ledDisplay.setBrightness(dutyCycle, gamma);
}

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
  uint16_t b = (256 - ((frame - row * 32 + pixelNum * 256 / LED_COUNT_PER_STRAND) & 0xFF)) * (rms / rmsMax);
  uint16_t b2 = (256 - ((frame - row * 32 + pixelNum * 256 / LED_COUNT_PER_STRAND) & 0xFF));
  if (on)
  {
    return ledDisplay.Color((colors[(color + pixelNum) % 8][1] * b2) >> 8,
                    (colors[(color + pixelNum) % 8][2] * b2) >> 8,
                    (colors[(color + pixelNum) % 8][0] * b2) >> 8);
  }
  return ledDisplay.Color((colors[(color + pixelNum) % 8][0] * b) >> 8,
                    (colors[(color + pixelNum) % 8][1] * b) >> 8,
                    (colors[(color + pixelNum) % 8][2] * b) >> 8);
}

void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(audioSampleBuffer, bytesAvailable);
  numberOfAudioSamplesRead = bytesAvailable / 2;
}
