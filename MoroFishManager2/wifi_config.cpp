#include <Arduino.h>
#include "wifi_config.hpp"

#include "domain.hpp"
#include "persistent.hpp"

namespace wifi_config {

using namespace domain;

//static const char *ssid = "C48E8F37BA15-2G";    //  *** 書き換え必要 ***
//static const char *password = "2215081330914";  //  *** 書き換え必要（8文字以上）***

namespace connections {

int count() {
  return WIFI_CONNECTION_ENTRY_SIZE;
}

void list(WiFiConnectionEntry *entries, int n) {
  persistent::wifi_config::session_begin();
  WiFiConfig config;
  persistent::wifi_config::read(config);
  memcpy(entries, config.connection_entries, sizeof(WiFiConnectionEntry) * n);
  persistent::wifi_config::session_end();
}

bool put(int i, String ssid, String password) {
  if (i < 0 || i >= count()) {
    return false;
  }

  WiFiConnectionEntry entry;
  entry.is_valid = true;
  strcpy(entry.ssid, ssid.c_str());
  strcpy(entry.password, password.c_str());

  persistent::wifi_config::session_begin();
  WiFiConfig config;
  persistent::wifi_config::read(config);
  memcpy(&config.connection_entries[i], &entry, sizeof(WiFiConnectionEntry));
  persistent::wifi_config::write(config);
  persistent::wifi_config::session_end();

  return true;
}

bool remove(int i) {
  if (i < 0 || i >= count()) {
    return false;
  }

  persistent::wifi_config::session_begin();
  WiFiConfig config;
  persistent::wifi_config::read(config);
  memset(&config.connection_entries[i], 0, sizeof(WiFiConnectionEntry));
  persistent::wifi_config::write(config);
  persistent::wifi_config::session_end();

  return true;
}

}

}