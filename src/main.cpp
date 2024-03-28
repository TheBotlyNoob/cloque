#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "FastLED.h"
#include "font.h"

#define DATA_PIN 12
#define NUM_LEDS_PER_STRIP 32
#define NUM_STRIPS 8
#define NUM_LEDS NUM_LEDS_PER_STRIP *NUM_STRIPS

#define XY(x, y)                                                               \
  y & 1 ? y *NUM_STRIPS + (NUM_STRIPS - 1 - x) : (y * NUM_STRIPS) + x

CRGB leds[NUM_LEDS];
uint8_t dig_idx = 0;

void print_c(const char c[FONT_WIDTH]);

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

  FastLED.setBrightness(10);
}

void loop() {
  dig_idx = 0;

  Serial.println("tick");

  print_c(font[2]);
  print_c(font[3]);
  print_c(font[4]);
  print_c(font[5]);

  FastLED.show();

  delay(5000);

  Serial.println("again!");

  print_c(font[6]);
  print_c(font[7]);
  print_c(font[8]);
  print_c(font[9]);
  // print_c(font[0]);

  FastLED.show();

  delay(5000);
}

/**
  Prints a character. You must call `FastLED.show()` after calling this.
*/
void print_c(const char c[FONT_WIDTH]) {
  uint8_t x, y, moved_y, idx;

  for (x = 0; x < FONT_WIDTH; x++) {
    for (y = 0; y < FONT_HEIGHT; y++) {
      moved_y = y + (FONT_HEIGHT * dig_idx);
      idx = XY(x, moved_y);

      if ((c[x] >> y) & 1) {
        leds[idx] = CRGB::White;
      } else {
        leds[idx] = CRGB::Black;
      }
    }
  }

  dig_idx = (dig_idx + 1) % 4;
}
