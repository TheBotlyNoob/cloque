#include "Arduino.h"
#include "FastLED.h"
#include "font.h"

#define DATA_PIN 12
#define NUM_LEDS_PER_STRIP 32
#define NUM_STRIPS 8
#define NUM_LEDS NUM_LEDS_PER_STRIP *NUM_STRIPS

CRGB leds[NUM_LEDS];
int dig_index = 0;

void print_c(const char c[FONT_WIDTH]);

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

  Serial.println("abcdef");
}

void loop() {
  Serial.println("tick");

  print_c(font[2]);

  FastLED.show();

  delay(5000);
}

/**
  Prints a character. You must call `FastLED.show()` after calling this.
*/
void print_c(const char c[FONT_WIDTH]) {
  uint8_t x, y;

  for (x = 0; x < FONT_WIDTH; x++) {
    for (y = 0; y < FONT_HEIGHT; y++) {
      if ((c[x] >> y) & 1) {
        leds[x + y * NUM_LEDS_PER_STRIP] = CRGB::Blue;
      };
    }
  }

  Serial.println("\n\n\n---------------------------------------\n\n");
}
