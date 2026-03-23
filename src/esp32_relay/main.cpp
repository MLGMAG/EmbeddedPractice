#include <Arduino.h>

constexpr uint8_t SWITCH_CONTACT_PIN = 32;
constexpr uint8_t CONTACT_RISING_PIN = 33;

constexpr unsigned long PIN_RELAY_TOGGLE_DELAY = 2500;
constexpr unsigned long CONTACT_VALUE_SET_DELAY = 500;

volatile uint8_t contact_value = LOW;
volatile unsigned long last_contact_value_set = 0;
volatile uint8_t relay_value = LOW;
volatile unsigned long last_relay_value_toggle = 0;

void contactRisingInterrupt() {
  contact_value = HIGH;
}

void setup() {
  Serial.begin(9600);
  delay(250);

  pinMode(SWITCH_CONTACT_PIN, OUTPUT);
  pinMode(CONTACT_RISING_PIN, INPUT_PULLDOWN);

  attachInterrupt(CONTACT_RISING_PIN, contactRisingInterrupt, RISING);
}

void loop() {
  unsigned long current_millis = millis();

  if (contact_value == HIGH) {
    Serial.printf("Contact value is HIGH. Millis: %d\n", current_millis);
    contact_value = LOW;
  }

  if (current_millis - last_relay_value_toggle >= PIN_RELAY_TOGGLE_DELAY) {
    relay_value = !relay_value;
    Serial.printf("Relay value is %d. Millis: %d\n", relay_value, current_millis);
    digitalWrite(SWITCH_CONTACT_PIN, relay_value);
    last_relay_value_toggle = current_millis;
  }
}

