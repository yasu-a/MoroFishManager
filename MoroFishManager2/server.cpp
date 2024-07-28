#include "server.hpp"
#include "state.hpp"
#include "config.hpp"

namespace server {

void setup() {
  WiFi.begin(config::get_ssid(), config::get_password());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("connecting");
  }
  Serial.println(WiFi.localIP());
  server.begin();
}

void process_command(WiFiClient &client, const String command) {
  if (command == "#led_only") {
    bool result = state::begin_feed(state::FeedMode::LED_ONLY);
    if (result) {
      client.println("#OK");
    } else {
      client.println("#NG ActiveTaskExists");
    }
  } else if (command == "#led_and_feed") {
    bool result = state::begin_feed(state::FeedMode::LED_AND_FEED);
    if (result) {
      client.println("#OK");
    } else {
      client.println("#NG ActiveTaskExists");
    }
  } else if (command == "#get_feed_state") {
    if (state::is_feed_active()) {
      client.println("#OK true");
    } else {
      client.println("#OK false");
    }
  } else if (command == "#get_feed_history") {
    client.printf("#OK %d", state::get_history_count());
    for (int i = 0; i < state::get_history_count(); i++) {
      client.print(" ");
      if (state::get_history(i).feed_mode == state::FeedMode::LED_ONLY) {
        client.print("LED");
      } else if (state::get_history(i).feed_mode == state::FeedMode::LED_AND_FEED) {
        client.print("LED&FEED");
      }
      client.print(",");
      client.print(state::get_history(i).timestamp);
    }
    client.println();
  } else {
    client.printf("#NG InvalidCommand \"%s\"\n", command.c_str());
  }
}

void loop() {
  WiFiClient client = server.available();
  if (client) {  //接続PCが存在
    String command = client.readStringUntil('\n');
    process_command(client, command);
    client.println("\n\n\n\n\n");
    client.stop();
  }
}

}