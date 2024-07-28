#include "config.hpp"

namespace config {

static const char *ssid = "C48E8F37BA15-2G";    //  *** 書き換え必要 ***
static const char *password = "2215081330914";  //  *** 書き換え必要（8文字以上）***

const char *get_ssid() {
  return ssid;
}

const char *get_password() {
  return password;
}

}