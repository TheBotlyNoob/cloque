#include "FastLED.h"
#include <cstdint>

#define DATA_PIN 12
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8

extern CRGB leds[MATRIX_WIDTH * MATRIX_HEIGHT];

uint16_t XY(uint8_t x, uint8_t y);
