#ifndef __SERVER_HEADER__
#define __SERVER_HEADER__

#include <WiFi.h>

namespace server {

#define WIFI_CONNECTION_TIMEOUT_MS 5000
#define WIFI_CONNECTION_RETRY_MAX 5
#define WIFI_CONNECTION_RETRY_DELAY_MS 2000
#define HOSTNAME "sakana-esp-main"
#define SERVER_PORT 8080

static WiFiServer server(SERVER_PORT);

void setup();
void loop();

}

#endif /* __SERVER_HEADER__ */