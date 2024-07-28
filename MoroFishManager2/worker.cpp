#include "worker.hpp"
#include "state.hpp"
#include <Arduino.h>

namespace worker {

#define PIN_LED 13

void setup() {
  pinMode(PIN_LED, OUTPUT);
}

void loop() {
  if (state::is_feed_active()) {
    if (state::get_feed_mode() == state::FeedMode::LED_ONLY) {
      digitalWrite(PIN_LED, HIGH);
      delay(5000);
      digitalWrite(PIN_LED, LOW);
    } else if (state::get_feed_mode() == state::FeedMode::LED_AND_FEED) {
      for (int i = 0; i < 10; i++) {
        digitalWrite(PIN_LED, HIGH);
        delay(250);
        digitalWrite(PIN_LED, LOW);
        delay(250);
      }
    } else {
      Serial.println("[worker.cpp] Unknown feed mode!!!");
    }
    state::set_feed_end();
  }
}

}