#include <Arduino.h>
#include "cli.hpp"

#include "wifi_config.hpp"

namespace cli {

using namespace domain;

void setup() {
  Serial.println("[cli.cpp] setup");
  Serial.println("[cli.cpp] WiFi configuration commands: wifi-list, wiif-remove, wifi-set");
}

static void wait_until_available() {
  while (!Serial.available()) {
    delay(10);
  }
}

static String read_str(bool echo = true) {
  wait_until_available();
  String str = Serial.readStringUntil('\n');
  if (echo) {
    Serial.println(str);
  }
  return str;
}

static int read_int(bool echo = true) {
  return read_str(echo).toInt();
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    if (line == "wifi-list") {
      int n_entries = wifi_config::connections::count();
      WiFiConnectionEntry *entries = new WiFiConnectionEntry[n_entries];
      wifi_config::connections::list(entries, n_entries);
      for (int i = 0; i < n_entries; i++) {
        if (entries[i].is_valid) {
          Serial.printf(" - %d: %s, *****\n", i, entries[i].ssid);
        } else {
          Serial.printf(" - %d: <no data>\n", i);
        }
      }
      delete entries;
    } else if (line == "wifi-remove") {
      Serial.print("Enter number: ");
      int num = read_int();
      bool result = wifi_config::connections::remove(num);
      if (result) {
        Serial.println("Connection removed.");
      } else {
        Serial.println("Connection not found.");
      }
    } else if (line == "wifi-set") {
      Serial.print("Enter number: ");
      String num_str = Serial.readStringUntil('\n');
      int num = read_int();
      Serial.print("Enter SSID");
      String ssid = read_str();
      Serial.print("Enter password");
      String password = read_str();
      bool result = wifi_config::connections::put(num, ssid, password);
      if (result) {
        Serial.println("SSID and password set!");
      } else {
        Serial.println("Inavlid number");
      }
    } else {
      Serial.printf("Invalid CLI command: \"%s\"\n", line);
    }
  }
}

}