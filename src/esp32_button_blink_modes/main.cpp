#include <esp32-hal-gpio.h>

#define LED_GREEN_OUTPUT 4
#define LED_RED_OUTPUT 5

#define SLOW_MODE_INPUT 0
#define FAST_MODE_INPUT 6

#define BIG_DELAY 1500
#define NORMAL_DELAY 750
#define SHORT_DELAY 250

void switch_blink(uint32_t delay_ms);

int delay_ms = NORMAL_DELAY;

void setup() {
  delay(1000);

  pinMode(LED_GREEN_OUTPUT, OUTPUT);
  pinMode(LED_RED_OUTPUT, OUTPUT);
  pinMode(SLOW_MODE_INPUT, INPUT_PULLDOWN);
  pinMode(FAST_MODE_INPUT, INPUT_PULLDOWN);
}

void loop() {

  int slow_mode_input = !digitalRead(SLOW_MODE_INPUT);
  int fast_mode_input = digitalRead(FAST_MODE_INPUT);

  if (slow_mode_input == HIGH && fast_mode_input == LOW) {
    delay_ms = NORMAL_DELAY;
  }

  if (fast_mode_input == HIGH && slow_mode_input == LOW) {
    delay_ms = SHORT_DELAY;
  }

    if (fast_mode_input == HIGH && slow_mode_input == HIGH) {
    delay_ms = BIG_DELAY;
  }

  switch_blink(delay_ms);

  delay(50);
}

void switch_blink(uint32_t delay_ms) {
  digitalWrite(LED_GREEN_OUTPUT, HIGH);
  digitalWrite(LED_RED_OUTPUT, HIGH);
  delay(delay_ms);

  digitalWrite(LED_GREEN_OUTPUT, LOW);
  digitalWrite(LED_RED_OUTPUT, LOW);
  delay(delay_ms);
}
