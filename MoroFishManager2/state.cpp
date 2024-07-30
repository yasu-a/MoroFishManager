#include <Arduino.h>
#include "state.hpp"

#include "time.h"
#include "domain.hpp"
#include "history.hpp"

namespace state {

using namespace domain;

static volatile State state;

// Function that gets current epoch time
static unsigned long get_time() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("[state.cpp] Failed to obtain time");
    return 0;
  }
  time(&now);
  return now;
}

namespace feed {

static volatile SemaphoreHandle_t semaphore;

void setup() {
  semaphore = xSemaphoreCreateMutex();

  state.feed_state.is_active = false;
  state.feed_state.start_at = 0;
  state.feed_state.feed_mode = FeedMode::LED_ONLY;
}

static void lock() {
  xSemaphoreTake(semaphore, portMAX_DELAY);
}

static void unlock() {
  xSemaphoreGive(semaphore);
}

bool begin(FeedMode mode) {
  lock();
  bool result;
  if (state.feed_state.is_active) {
    result = false;
  } else {
    state.feed_state.feed_mode = mode;
    state.feed_state.start_at = get_time();
    state.feed_state.is_active = true;
    result = true;
  }
  unlock();
  return result;
}

bool is_active() {
  lock();
  bool result = state.feed_state.is_active;
  unlock();
  return result;
}

FeedMode get_mode() {
  lock();
  FeedMode mode = state.feed_state.feed_mode;
  unlock();
  return mode;
}

void end() {
  lock();
  state.feed_state.is_active = false;
  unsigned long ended_at = get_time();
  history::feed::add(state.feed_state.feed_mode, state.feed_state.start_at, ended_at);
  unlock();
}

}

void setup() {
  Serial.println("[state.cpp] setup");

  feed::setup();

  configTime(0, 0, "pool.ntp.org");
  setenv("TZ", "GMT0", 1);
  tzset();  // Assign the local timezone from setenv for mktime()
}


}