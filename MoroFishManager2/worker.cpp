#include <Arduino.h>
#include "worker.hpp"

#include "state.hpp"
#include "domain.hpp"

namespace worker {

using namespace domain;

static const int PIN_LED = 32;

void setup() {
  Serial.println("[worker.cpp] setup");
  pinMode(PIN_LED, OUTPUT);
}

void loop() {
  if (state::feed::is_active()) {
    switch (state::feed::get_mode()) {
      case FeedMode::LED_ONLY:
        digitalWrite(PIN_LED, HIGH);
        delay(5000);
        digitalWrite(PIN_LED, LOW);
        break;
      case FeedMode::LED_AND_FEED:
        for (int i = 0; i < 10; i++) {
          digitalWrite(PIN_LED, HIGH);
          delay(250);
          digitalWrite(PIN_LED, LOW);
          delay(250);
        }
        break;
      default:
        Serial.println("[worker.cpp] Unknown feed mode!!!");
    }
    state::feed::end();
  }
}

}