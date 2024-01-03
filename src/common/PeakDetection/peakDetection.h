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

#ifndef PeakDetection_h
#define PeakDetection_h
 
#include <math.h>
#include <stdlib.h>

class PeakDetection {
  public:
    PeakDetection();
    ~PeakDetection();
    void begin();
    void begin(int, int, float); //lag, threshold, influence
    //void add(float);
    float add(float);
    float getFilt();
    int getPeak();
    void setEpsilon(float);
    float getEpsilon();

  private:
    int index, lag, threshold, peak;
    float influence, EPSILON, *data, *avg, *std;
    float getAvg(int, int);
    float getPoint(int, int);
    float getStd(int, int);
};

#endif