#ifndef __SERVER_HEADER__
#define __SERVER_HEADER__

#include <WiFi.h>

namespace server {

static WiFiServer server(80);

void setup();
void loop();

}

#endif /* __SERVER_HEADER__ */