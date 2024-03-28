#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "FastLED.h"
#include "conf.h" // you might have to make this yourself- copy conf.h.example to conf.h and set the proper values
#include "font.h"

#define DATA_PIN 12
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8

// "borrowed" from fastled.io/docs/_smart_matrix_8ino-example.html
uint16_t XY(uint8_t x, uint8_t y) {
  uint16_t i;

  if (y & 0x01) {
    // Odd rows run backwards
    uint8_t reverseX = (MATRIX_WIDTH - 1) - x;
    i = (y * MATRIX_WIDTH) + reverseX;
  } else {
    // Even rows run forwards
    i = (y * MATRIX_WIDTH) + x;
  }

  return i;
}

uint16_t bottom_row(uint8_t x) {
  if (x & 1) {
    return x * MATRIX_HEIGHT;
  } else if (x == 0) {
    return 0;
  } else {
    return (x - 1) * MATRIX_HEIGHT;
  }
}

CRGB leds[MATRIX_WIDTH * MATRIX_HEIGHT];
uint8_t dig_idx = 0;

unsigned long wifi_prev_millis = 0;
uint16_t prev_wifi_loader = 0;
bool wifi_connected = false;

void print_c(unsigned char c);
void print_str(const char str[4]);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  Serial.setDebugOutput(true);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, MATRIX_WIDTH * MATRIX_HEIGHT);
  FastLED.setBrightness(5);

  WiFi.begin(NETWORK_NAME, NETWORK_PASS); // from conf.h

  Serial.print("Connecting to ");
  Serial.println(NETWORK_NAME);
}

void loop() {
  print_str("WIFI"); // FastLED.show() gets called eventually...
  FastLED.show();

  // dig_idx = 0;

  // unsigned long curr_millis = millis();

  // if (WiFi.status() != WL_CONNECTED) {
  //   if (curr_millis - wifi_prev_millis >= 1000) {
  //     Serial.println("WiFi not connected.");
  //     wifi_prev_millis = curr_millis;
  //   } else {
  //     double percent_through_second =
  //         (double)(curr_millis - wifi_prev_millis) / 1000.0;
  //     uint8_t bottom_row_x = (uint8_t)floor(percent_through_second * 32);

  //     if (bottom_row_x != prev_wifi_loader) {
  //       leds[bottom_row(prev_wifi_loader)] = CRGB::Black;
  //       leds[bottom_row(bottom_row_x)] = CRGB::SkyBlue;
  //       FastLED.show();

  //       prev_wifi_loader = bottom_row_x;
  //     }
  //   }
  // } else if (!wifi_connected) {
  //   wifi_connected = true;

  //   leds[bottom_row(prev_wifi_loader)] = CRGB::Black;
  //   prev_wifi_loader = 0;
  //   FastLED.show();

  //   Serial.print("WiFi connected. IP address: ");
  //   Serial.println(WiFi.localIP());
  // }
}

/**
  Prints an up-to four character string. You must call `FastLED.show()` after
  calling this.
*/
void print_str(const char str[4]) {
  Serial.print("PRINTED TO LED MATRIX: ");
  Serial.println(str);
  for (int i = 0; i < 4; i++) {
    print_c(str[i]);
  }
}

/**
  Prints a character. You must call `FastLED.show()` after calling this.
*/
void print_c(unsigned char c) {
  const uint8_t *bitmap = font[toupper(c)];

  uint8_t x, y, idx;

  for (x = 0; x < FONT_WIDTH; x++) {
    for (y = 0; y < FONT_HEIGHT - 1;
         y++) { // bottom row is reserved for other stuff
      idx = XY(x + (FONT_HEIGHT * dig_idx), y);

      if ((bitmap[x] >> y) & 1) {
        leds[idx] = CRGB::Blue;
      } else {
        leds[idx] = CRGB::Black;
      }
    }
  }

  dig_idx = (dig_idx + 1) % 4;
}
