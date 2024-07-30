#include "persistent.hpp"
#include "history.hpp"
#include "state.hpp"
#include "worker.hpp"
#include "server.hpp"
#include "cli.hpp"

void task_server(void *pvParameters) {
  Serial.println("[main] task_server begin");
  for (;;) {
    server::loop();
    delay(5);
  }
  Serial.println("[main] task_server end");
}

void task_worker(void *pvParameters) {
  Serial.println("[main] task_worker begin");
  for (;;) {
    worker::loop();
    delay(1);
  }
  Serial.println("[main] task_worker end");
}

void task_cli(void *pvParameters) {
  Serial.println("[main] task_cli begin");
  for (;;) {
    cli::loop();
    delay(5);
  }
  Serial.println("[main] task_cli end");
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("[main] Serial established!");

  persistent::setup();
  history::setup();
  state::setup();
  worker::setup();
  server::setup();
  cli::setup();

  xTaskCreateUniversal(
    task_server,
    "task_server",
    8192,
    NULL,
    2,
    NULL,
    APP_CPU_NUM);  //
  xTaskCreateUniversal(
    task_worker,
    "task_worker",
    8192,
    NULL,
    3,
    NULL,
    PRO_CPU_NUM);  //
  xTaskCreateUniversal(
    task_cli,
    "task_cli",
    8192,
    NULL,
    1,
    NULL,
    APP_CPU_NUM);  //
}


void loop() {
  delay(10);
}