#include "ledmatrix.h"

CRGB leds[MATRIX_WIDTH * MATRIX_HEIGHT];

// "borrowed" from fastled.io/docs/_smart_matrix_8ino-example.html
uint16_t XY(uint8_t x, uint8_t y) {
  uint16_t i;

  if (x & 0x01) {
    // Odd rows run backwards
    uint8_t reverseY = (MATRIX_HEIGHT - 1) - y;
    i = (x * MATRIX_HEIGHT) + reverseY;
  } else {
    // Even rows run forwards
    i = (x * MATRIX_HEIGHT) + y;
  }

  return i;
}
