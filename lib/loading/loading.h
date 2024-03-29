#include "FastLED.h"

extern unsigned long loading_prev_millis;

void tick_loading(unsigned long curr_millis, CRGB color);
void reset_loading();
