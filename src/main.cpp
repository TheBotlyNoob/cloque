#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "FastLED.h"
#include "WiFiClient.h"
#include "conf.h" // you might have to make this yourself- copy conf.h.example to conf.h and set the proper values
#include "font.h"
#include "ledmatrix.h"
#include "loading.h"

#define NIST_SERVER "time.nist.gov"

#define PRINT_VAL(s, v)                                                        \
  Serial.print(s);                                                             \
  Serial.println(v);

uint8_t dig_idx = 0;

bool wifi_initial_connected = false;

WiFiClient client;

IPAddress dns(1, 1, 1, 1); // Cloudflare's DNS

void print_c(unsigned char c);
void print_str(const char str[4]);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  Serial.setDebugOutput(true);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, MATRIX_WIDTH * MATRIX_HEIGHT);
  FastLED.setBrightness(10);

  WiFi.begin(NETWORK_NAME, NETWORK_PASS); // from conf.h

  Serial.print("Connecting to ");
  Serial.println(NETWORK_NAME);

  print_str("0000"); // base time.
  FastLED.show();
}

enum class ConnState { Receiving, Standby };
ConnState conn_state = ConnState::Standby;

void while_wifi_connected() {
  if (conn_state == ConnState::Standby) {
    Serial.println("Making connection to example");

    if (client.connect("example.com", 80)) {
      client.println("GET / HTTP/1.1");
      client.println("Host: example.com");
      client.println("Connection: close");
      client.println();
    }

    conn_state = ConnState::Receiving;
  } else {
    tick_loading(millis(), CRGB::Red);

    while (client.available()) {
      char c = client.read();
      Serial.write(c);
      tick_loading(millis(), CRGB::Red);
    }

    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting from example");
      client.stop();

      reset_loading();

      conn_state = ConnState::Standby;

      delay(10000);
    }
  }
}

uint8_t prev_hours = 0;
uint8_t prev_mins = 0;

void loop() {
  dig_idx = 0;

  unsigned long curr_millis = millis();

  uint32_t secs_in_day = curr_millis / 1000 % 86400;

  uint8_t hours = round(secs_in_day / 60 / 60);
  uint8_t mins = round(secs_in_day / 60 % 60);

  if (hours != prev_hours || mins != prev_mins) {
    char stringified[4]; // we don't need a null byte here, right?

    stringified[3] = mins % 10 + '0';
    stringified[2] = mins / 10 % 10 + '0';
    stringified[1] = hours % 10 + '0';
    stringified[0] = hours / 10 % 10 + '0';

    prev_hours = hours;
    prev_mins = mins;

    print_str(stringified);
    FastLED.show();
  }

  // loader "clock"
  if (curr_millis - loading_prev_millis >= 1000)
    loading_prev_millis = curr_millis;

  if (WiFi.status() != WL_CONNECTED)
    tick_loading(curr_millis, CRGB::SkyBlue);
  else if (!wifi_initial_connected) {
    wifi_initial_connected = true;

    reset_loading();

    Serial.print("WiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
  }

  if (WiFi.status() == WL_CONNECTED) {
    // while_wifi_connected();
  }
}

/**
  Prints an up-to four character string. You must call `FastLED.show()`
  after calling this.
*/
void print_str(const char str[4]) {
  PRINT_VAL("PRINTED TO LED MATRIX: ", str);
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
      idx = XY(x + (FONT_WIDTH * dig_idx), y);
      // PRINT_VAL("X: ", x);
      // PRINT_VAL("Y: ", y);
      // PRINT_VAL("idx: ", idx);

      if ((bitmap[y] >> x) & 1) {
        leds[idx] = CRGB::Blue;
      } else {
        leds[idx] = CRGB::Black;
      }
    }
  }

  dig_idx = (dig_idx + 1) % 4;
}
