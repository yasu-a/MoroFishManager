#ifndef __WIFI_CONFIG_HEADER__
#define __WIFI_CONFIG_HEADER__

#include <Arduino.h>

#include "domain.hpp"

namespace wifi_config {

using namespace domain;

namespace connections {

int count();
void list(WiFiConnectionEntry *entries, int n);
bool put(int i, String ssid, String password);
bool remove(int i);

}

}

#endif /* __WIFI_CONFIG_HEADER__ */