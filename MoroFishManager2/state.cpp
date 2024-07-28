#include "state.hpp"
#include <Arduino.h>
#include "time.h"

namespace state {

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

#define MAX_HISTORY 64

static History histories[MAX_HISTORY];
static int history_count = 0;

static void append_history_unlocked(History history) {
  if (history_count >= MAX_HISTORY) {
    for (int i = 1; i < MAX_HISTORY; i++) {
      // histories[i - 1] = histories[i];
      memcpy(&histories[i - 1], &histories[i], sizeof(History));
    }
    history_count--;
  }
  histories[history_count] = history;
  history_count++;
}

static volatile SemaphoreHandle_t semaphore;
static volatile bool feed_active = false;
static volatile FeedMode feed_mode = FeedMode::LED_ONLY;

void setup() {
  semaphore = xSemaphoreCreateMutex();

  configTime(0, 0, "pool.ntp.org");
  setenv("TZ", "GMT0", 1);
  tzset();  // Assign the local timezone from setenv for mktime()
}

static void lock() {
  xSemaphoreTake(semaphore, portMAX_DELAY);
}

static void unlock() {
  xSemaphoreGive(semaphore);
}

bool begin_feed(FeedMode feed_mode) {
  lock();
  bool result;
  if (feed_active) {
    result = false;
  } else {
    feed_active = true;
    state::feed_mode = feed_mode;
    result = true;
  }
  unlock();
  return result;
}

void set_feed_end() {
  lock();
  if (feed_active) {
    History history = { feed_mode, get_time() };
    append_history_unlocked(history);
  }
  feed_active = false;
  unlock();
}

bool is_feed_active() {
  lock();
  bool result = feed_active;
  unlock();
  return result;
}

FeedMode get_feed_mode() {
  lock();
  FeedMode result = feed_mode;
  unlock();
  return result;
}

int get_history_count() {
  return history_count;
}

History get_history(int i) {
  return histories[i];
}

}