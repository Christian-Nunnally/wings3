#ifdef RP2040
#include <Arduino.h>
#include <pgmspace.h>
#else
#include <stdint.h>
#include <iostream>
#include <cmath>
#define PROGMEM
#endif