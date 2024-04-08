#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "FastLED.h"
#include "WiFiClient.h"
#include "WiFiUdp.h"
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
WiFiUDP udp;

struct ntp_timestamp_t {
  uint32_t seconds;
  uint32_t fraction;
};

void timestamp_ntoh(ntp_timestamp_t *t) {
  t->seconds = ntohl(t->seconds);
  t->fraction = ntohl(t->fraction);
}

#pragma pack(push, 1)
struct SNTP {
  uint8_t li_vi_mode;
  uint8_t stratum;
  int8_t poll_interval;
  int8_t precision;
  int32_t root_delay;
  uint32_t root_dispersion;
  uint32_t ref_clock_ident;
  ntp_timestamp_t ref_timestamp;
  ntp_timestamp_t originate_timestamp;
  ntp_timestamp_t recv_timestamp;
  ntp_timestamp_t transmit_timestamp;
};
#pragma pack(pop)

uint32_t timestamp = 0;

void print_c(unsigned char c);
void print_str(const char str[4]);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  Serial.setDebugOutput(true);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, MATRIX_WIDTH * MATRIX_HEIGHT);
  FastLED.setBrightness(10);

  WiFi.begin(NETWORK_NAME, NETWORK_PASS); // from conf.h
  udp.begin(123);

  Serial.print("Connecting to ");
  Serial.println(NETWORK_NAME);

  print_str("INIT"); // base time.
  FastLED.show();
}

bool timestamp_initial_recvd = false;

enum class ConnState { Receiving, Standby, Done };
ConnState conn_state = ConnState::Standby;

void while_wifi_connected() {
  SNTP packet;

  if (conn_state == ConnState::Standby) {
    udp.beginPacket(NIST_SERVER, 123);

    packet = {
        0x1B, 0, 0, 0, 0, 0, 0, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    };

    // this is fine- as long as the packet _looks_ right, the server will
    // probably take it.
    udp.write(reinterpret_cast<uint8_t *>(&packet), sizeof(packet));

    udp.endPacket();

    conn_state = ConnState::Receiving;
  } else {
    uint8_t packet_size = udp.parsePacket();

    uint8_t buf[sizeof(SNTP)];

    if (packet_size > 0) {
      packet_size = udp.read((unsigned char *)&buf, sizeof(SNTP));

      memcpy(&packet, &buf, sizeof(SNTP));

      timestamp_ntoh(&packet.transmit_timestamp);

      timestamp = packet.transmit_timestamp.seconds - millis() / 1000;

      timestamp_initial_recvd = true; // it doesn't matter if its past the first
                                      // recv- it'll still be true anyways

      conn_state = ConnState::Done;
    }
  }
}

bool timestamp_recv_loader_reset = false;

uint8_t prev_hours = 0;
uint8_t prev_mins = 0;

void loop() {
  dig_idx = 0;

  unsigned long curr_millis = millis();

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
    while_wifi_connected();
  }

  if (timestamp != 0) {
    uint32_t secs_in_day = (curr_millis / 1000 + timestamp + (TZ_HOUR_CHANGE * 60 * 60)) % 86400;

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
      if (!timestamp_recv_loader_reset) {
        reset_loading();
        timestamp_recv_loader_reset = true;
      }
      FastLED.show();
      FastLED.show();
    }
  } else if (wifi_initial_connected) {
    tick_loading(curr_millis, CRGB::Green);
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
