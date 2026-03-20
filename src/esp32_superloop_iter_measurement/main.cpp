#include <HardwareSerial.h>
#include <esp32-hal-gpio.h>

constexpr unsigned long SERIAL_BAUD_RATE = 115200;
constexpr uint32_t SERIAL_INIT_DELAY_MS = 250;
constexpr unsigned long ITERATION_PRINT_TARGET = 1000000;

volatile unsigned long iteration_count = 0;
volatile unsigned long last_iter_execution_time = -1;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(SERIAL_INIT_DELAY_MS);
}

void loop() {
  iteration_count++;
  unsigned long current_millis = millis();

  if (last_iter_execution_time == -1) {
    last_iter_execution_time = current_millis;
  }

  if (iteration_count >= ITERATION_PRINT_TARGET) {
    unsigned long time_spend = current_millis - last_iter_execution_time;
    Serial.printf("%d (ms) time spend for %d iterations\n", time_spend, ITERATION_PRINT_TARGET);

    float time_spend_single_iter = ((double) (time_spend * 1000)) / ITERATION_PRINT_TARGET;
    Serial.printf("%f (micro seconds) time spend for 1 iterations\n", time_spend_single_iter);

    last_iter_execution_time = -1;
    iteration_count = 0;
  }

}
