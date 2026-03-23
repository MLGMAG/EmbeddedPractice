#include <Arduino.h>

constexpr uint8_t LED1_PIN = 32;
constexpr uint8_t LED2_PIN = 33;
constexpr uint8_t LED3_PIN = 25;
constexpr uint8_t LED4_PIN = 26;
constexpr uint8_t LED5_PIN = 27;

constexpr unsigned long LED1_TOGGLE_DELAY = 500;
constexpr unsigned long LED2_TOGGLE_DELAY = 1000;
constexpr unsigned long LED3_TOGGLE_DELAY = 2000;
constexpr unsigned long LED4_TOGGLE_DELAY = 4000;
constexpr unsigned long LED5_TOGGLE_DELAY = 8000;

volatile int led1_value = LOW;
volatile unsigned long last_led1_set = 0;
volatile int led2_value = LOW;
volatile unsigned long last_led2_set = 0;
volatile int led3_value = LOW;
volatile unsigned long last_led3_set = 0;
volatile int led4_value = LOW;
volatile unsigned long last_led5_set = 0;
volatile int led5_value = LOW;
volatile unsigned long last_led4_set = 0;

void setup() {
  Serial.begin(9600);
  delay(250);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  pinMode(LED5_PIN, OUTPUT);
}

void loop() {
  unsigned long current_millis = millis();

  if (current_millis - last_led1_set >= LED1_TOGGLE_DELAY) {
    led1_value = !led1_value;
    digitalWrite(LED1_PIN, led1_value);
    last_led1_set = current_millis;
  }

  if (current_millis - last_led2_set >= LED2_TOGGLE_DELAY) {
    led2_value = !led2_value;
    digitalWrite(LED2_PIN, led2_value);
    last_led2_set = current_millis;
  }

  if (current_millis - last_led3_set >= LED3_TOGGLE_DELAY) {
    led3_value = !led3_value;
    digitalWrite(LED3_PIN, led3_value);
    last_led3_set = current_millis;
  }

  if (current_millis - last_led4_set >= LED4_TOGGLE_DELAY) {
    led4_value = !led4_value;
    digitalWrite(LED4_PIN, led4_value);
    last_led4_set = current_millis;
  }

  if (current_millis - last_led5_set >= LED5_TOGGLE_DELAY) {
    led5_value = !led5_value;
    digitalWrite(LED5_PIN, led5_value);
    last_led5_set = current_millis;
  }
}
