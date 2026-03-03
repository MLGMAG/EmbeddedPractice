#include <Arduino.h>

#define LED_RED_OUTPUT 14
#define LED_BLUE_OUTPUT 2
#define LED_GREEN_OUTPUT 42

void step_blink_pin(uint8_t pin);

void setup() {
  pinMode(LED_RED_OUTPUT , OUTPUT);
  pinMode(LED_BLUE_OUTPUT , OUTPUT);
  pinMode(LED_GREEN_OUTPUT , OUTPUT);
}

void loop() {
  step_blink_pin(LED_RED_OUTPUT);
  step_blink_pin(LED_BLUE_OUTPUT);
  step_blink_pin(LED_GREEN_OUTPUT);
}


void step_blink_pin(uint8_t pin) {
  for (int i = 0; i < 256; i++) {
    delay(15);
    analogWrite(pin, i);
  }

  for (int i = 0; i < 256; i++) {
    delay(15);
    int result = 255 - i;
    analogWrite(pin, result);
  }
}
