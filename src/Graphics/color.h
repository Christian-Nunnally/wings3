#ifndef COLOR_H
#define COLOR_H

#ifdef RP2040
#include <Arduino.h>
#else
#include <stdint.h>
#endif

typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
}
Color;

#endif