#include "ledmatrix.h"
#include "loading.h"
#include <cstdint>

unsigned long loading_prev_millis = 0;
uint16_t prev_loading_x = 0;

void tick_loading(unsigned long curr_millis, CRGB color) {
  // a little loading at the bottom.
  double percent_through_second =
      (double)(curr_millis - loading_prev_millis) / 1000.0;
  uint8_t bottom_row_x = (uint8_t)floor(percent_through_second * 32);

  if (bottom_row_x != prev_loading_x) {
    leds[XY(prev_loading_x, 7)] = CRGB::Black;
    leds[XY(bottom_row_x, 7)] = color;
    FastLED.show();

    prev_loading_x = bottom_row_x;
  }
}
void reset_loading() {
  leds[XY(prev_loading_x, 7)] = CRGB::Black;
  prev_loading_x = 0;
  FastLED.show();
}
