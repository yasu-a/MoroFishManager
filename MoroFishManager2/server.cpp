#include <Arduino.h>
#include "server.hpp"

#include "domain.hpp"
#include "state.hpp"
#include "wifi_config.hpp"
#include "history.hpp"
#include "protocol.hpp"

namespace server {

using namespace domain;

static bool connect_to_valid_entry(const WiFiConnectionEntry &entry) {
  WiFi.mode(WIFI_MODE_STA);
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(entry.ssid, entry.password);
  unsigned long t_begin = millis();
  while (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();
    if (now - t_begin >= WIFI_CONNECTION_TIMEOUT_MS) {
      Serial.printf("[server.cpp] TIMEOUT %s\n", entry.ssid);
      return false;
    }
    delay(100);
    if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.printf("[server.cpp] FAILURE %s\n", entry.ssid);
      return false;
    }
  }
  Serial.printf("[server.cpp] SUCCESS %s\n", entry.ssid);
  return true;
}

static bool try_wifi_valid_entry(const WiFiConnectionEntry &entry) {
  for (int i = 0; i < WIFI_CONNECTION_RETRY_MAX; i++) {
    Serial.printf("[server.cpp] Connecting to %s [%d/%d]\n", entry.ssid, i + 1, WIFI_CONNECTION_RETRY_MAX);
    bool result = connect_to_valid_entry(entry);
    if (result) {
      return true;
    }
    WiFi.disconnect();
    delay(WIFI_CONNECTION_RETRY_DELAY_MS);
  }
  return false;
}

static void connect_wifi() {
  int n_entries = wifi_config::connections::count();
  WiFiConnectionEntry *entries = new WiFiConnectionEntry[n_entries];
  wifi_config::connections::list(entries, n_entries);
  bool no_valid_entries = true;
  bool connected = false;
  for (int i = 0; i < n_entries; i++) {
    WiFiConnectionEntry &entry = entries[i];
    if (entry.is_valid) {
      no_valid_entries = false;
      bool result = try_wifi_valid_entry(entry);
      if (result) {
        connected = true;
        break;
      }
    }
  }
  delete entries;
  if (no_valid_entries) {
    Serial.println("[server.cpp] No valid WiFi connection setting exists");
  }
  if (!connected) {
    Serial.println("[server.cpp] Failed to establish WiFi connection");
  }
}

void setup() {
  Serial.println("[server.cpp] setup");
  connect_wifi();
  Serial.println(WiFi.localIP());
  server.begin();
}

static void process_command(
  const protocol::Command &command,
  String &title,  // leave title empty if the result is OK
  String &content) {
  if (command.name == "#led_only") {
    bool result = state::feed::begin(FeedMode::LED_ONLY);
    if (result) {
      return;
    } else {
      title = "ActiveTaskExists";
      return;
    }
  } else if (command.name == "#led_and_feed") {
    bool result = state::feed::begin(FeedMode::LED_AND_FEED);
    if (result) {
      return;
    } else {
      title = "ActiveTaskExists";
      return;
    }
  } else if (command.name == "#get_feed_state") {
    if (state::feed::is_active()) {
      content = "true";
      return;
    } else {
      content = "false";
      return;
    }
  } else if (command.name == "#get_feed_history") {
    FeedHistoryEntry entry;
    for (int i = 0; i < history::feed::count(); i++) {
      history::feed::get(i, entry);
      if (!entry.is_valid) {
        continue;
      }
      if (content != "") {
        content += " ";
      }
      content += feed_history_entry_to_string(entry);
    }
    return;
  } else if (command.name == "#clear_history") {
    history::feed::clear();
    return;
  } else {
    title = "InvalidCommand";
    content = command.name;
    return;
  }
}

void process_client(WiFiClient &client, protocol::Command &command) {
  if (!protocol::receive_command(client, command)) {
    Serial.println("[server.cpp] Failed to receive command");
    return;
  }

  Serial.println("[server.cpp] Command received:");
  Serial.printf(" - command: %s\n", command.name.c_str());
  Serial.printf(" - n_args: %d\n", command.n_args);
  for (int i = 0; i < command.n_args; i++) {
    Serial.printf(" - args[%d]: %s\n", i, command.args[i].c_str());
  }

  delay(50);

  Serial.println("[server.cpp] Processing command...");
  String title;
  String content;
  process_command(command, title, content);

  delay(50);

  Serial.println("[server.cpp] Sending result...");
  if (title == "") {  // OK
    if (!protocol::send_result_ok(client, content)) {
      Serial.println("[server.cpp] Failed to send result");
      return;
    }
  } else {  // NG
    if (!protocol::send_result_ng(client, title, content)) {
      Serial.println("[server.cpp] Failed to send result");
      return;
    }
  }

  Serial.println("[server.cpp] Result sent:");
  if (title == "") {
    Serial.println(" - title: (OK)");
  } else {
    Serial.printf(" - title: %s(NG)\n", title.c_str());
  }
  Serial.printf(" - content: %s\n", content.c_str());
}

void loop() {
  WiFiClient client = server.available();
  if (client) {  //接続PCが存在
    Serial.println("[server.cpp] Client connected");
    protocol::Command command;
    process_client(client, command);
    client.stop();
    Serial.println("[server.cpp] Client stopped");
  }
}

}