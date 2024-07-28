#include "server.hpp"
#include "worker.hpp"
#include "state.hpp"


// サーバータスク
void task_server(void *pvParameters) {
  while (1) {
    server::loop();
    delay(1);
  }
}

// ワーカータスク
void task_worker(void *pvParameters) {
  while (1) {
    worker::loop();
    delay(1);
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  server::setup();
  worker::setup();
  state::setup();

  xTaskCreateUniversal(
    task_server,
    "task_server",
    8192,
    NULL,
    1,
    NULL,
    APP_CPU_NUM);
  xTaskCreateUniversal(
    task_worker,
    "task_worker",
    8192,
    NULL,
    2,
    NULL,
    APP_CPU_NUM);
}


void loop() {
  delay(1);
}