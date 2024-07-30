#include <Arduino.h>
#include "history.hpp"

#include "domain.hpp"
#include "persistent.hpp"

namespace history {

using namespace domain;

namespace feed {

static void setup() {
}

void session_begin();
int find_empty();
void get(int i, FeedHistoryEntry &obj);
void set(int i, const FeedHistoryEntry &obj);
void session_end();

int count() {
  persistent::history::session_begin();
  int count = persistent::history::count();
  persistent::history::session_end();
  return count;
}

void get(int i, FeedHistoryEntry &obj) {
  persistent::history::session_begin();
  persistent::history::get(i, obj);
  persistent::history::session_end();
}

void add(FeedMode mode, unsigned long started_at, unsigned long ended_at) {
  FeedHistoryEntry entry_insert = { true, mode, started_at, ended_at };

  persistent::history::session_begin();
  int i = persistent::history::find_empty();
  persistent::history::set(i, entry_insert);
  persistent::history::session_end();
}

void clear() {
  persistent::history::session_begin();
  for (int i = 0; i < persistent::history::count(); i++) {
    persistent::history::remove(i);
  }
  persistent::history::session_end();
}

}  // namespace feed

void setup() {
  Serial.println("[history.cpp] setup");
  feed::setup();
}

}  // namespace history