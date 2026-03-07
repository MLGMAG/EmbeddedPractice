#include <esp32-hal-gpio.h>

#define LED_OUTPUT 5
#define DELAY_SLOW 750
#define DELAY_FAST 250

void blink(uint32_t timeoutMs, int iterations);

void setup() {
  pinMode(LED_OUTPUT, OUTPUT);
}

void loop() {

  blink(DELAY_SLOW, 5);
  blink(DELAY_FAST, 10);

}

void blink(uint32_t timeoutMs, int iterations) {
    for (int i = 0; i < iterations; i++) {
    digitalWrite(LED_OUTPUT, HIGH);
    delay(timeoutMs);
    digitalWrite(LED_OUTPUT, LOW);
    delay(timeoutMs);
  }
}

