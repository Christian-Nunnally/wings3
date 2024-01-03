/*
  MIT License

  Copyright (c) 2019 Leandro César

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  
  Acknowledgment: https://stackoverflow.com/questions/22583391/peak-peaknal-detection-in-realtime-timeseries-data
*/

#include "PeakDetection.h"
using namespace std;

#define TwoToThePowerOf14 16383

const int DEFAULT_LAG = 32;
const int DEFAULT_THRESHOLD = 2;
const float DEFAULT_INFLUENCE = 0.5;
const float DEFAULT_EPSILON = 0.01;

PeakDetection::PeakDetection() {
  index = 0;
  lag = DEFAULT_LAG;
  threshold = DEFAULT_THRESHOLD;
  influence = DEFAULT_INFLUENCE;
  EPSILON = DEFAULT_EPSILON;
  peak = 0;
}

PeakDetection::~PeakDetection() {
  delete data;
  delete avg;
  delete std;
}

void PeakDetection::begin() {
  data = (float *)malloc(sizeof(float) * (lag + 1));
  avg = (float *)malloc(sizeof(float) * (lag + 1));
  std = (float *)malloc(sizeof(float) * (lag + 1));
  for (int i = 0; i < lag; ++i) {
    data[i] = 0.0;
    avg[i] = 0.0;
    std[i] = 0.0;
  }
}

void PeakDetection::begin(int lag, int threshold, float influence) {
  this->lag = lag;
  this->threshold = threshold;
  this->influence = influence;
  data = (float *)malloc(sizeof(float) * (lag + 1));
  avg = (float *)malloc(sizeof(float) * (lag + 1));
  std = (float *)malloc(sizeof(float) * (lag + 1));
  for (int i = 0; i < lag; ++i) {
    data[i] = 0.0;
    avg[i] = 0.0;
    std[i] = 0.0;
  }
}

void PeakDetection::setEpsilon(float epsilon) {
  this->EPSILON = epsilon;
}

float PeakDetection::getEpsilon() {
  return(EPSILON);
}

float PeakDetection::add(float newSample) {
  peak = 0;
  int currentIndex = index % lag;
  int nextIndex = (index + 1) % lag;
  float deviation = newSample - avg[currentIndex];
  if (deviation > threshold * std[currentIndex]) {
    data[nextIndex] = influence * newSample + (1.0 - influence) * data[currentIndex];
    peak = 1;
  }
  else if (deviation < -threshold * std[currentIndex]) {
    data[nextIndex] = influence * newSample + (1.0 - influence) * data[currentIndex];
    peak = -1;
  }
  else
    data[nextIndex] = newSample;
  avg[nextIndex] = getAvg(nextIndex, lag);
  std[nextIndex] = getStd(nextIndex, lag);
  index++;
  if (index >= TwoToThePowerOf14)
    index = lag + nextIndex;
  return(std[nextIndex]);
}

float PeakDetection::getFilt() {
  int i = index % lag;
  return avg[i];
}

int PeakDetection::getPeak() {
  return peak;
}

float PeakDetection::getAvg(int start, int len) {
  float x = 0.0;
  for (int i = 0; i < len; ++i)
    x += data[(start + i) % lag];
  return x / len;
}

float PeakDetection::getPoint(int start, int len) {
  float xi = 0.0;
  for (int i = 0; i < len; ++i)
    xi += data[(start + i) % lag] * data[(start + i) % lag];
  return xi / len;
}

float PeakDetection::getStd(int start, int len) {
  float x1 = getAvg(start, len);
  float x2 = getPoint(start, len);
  float powx1 = x1 * x1;
  float std = x2 - powx1;
  if (std > -EPSILON && std < EPSILON)
    if(std < 0.0)
        return(-EPSILON);
    else
        return(EPSILON);
  else
    return sqrt(x2 - powx1);
}