#include <HardwareSerial.h>
#include <esp32-hal-gpio.h>

constexpr unsigned long SERIAL_BAUD_RATE = 115200;
constexpr uint32_t SERIAL_INIT_DELAY_MS = 250;
constexpr unsigned long BLINK_DELAY_MS = 1000;
constexpr uint8_t LED_OUTPUT_PIN = 6;

volatile unsigned long last_millis_blink = 0;

enum LedState { ON, OFF };

class Led {
private:
  LedState m_state;
  uint8_t pin;

public:
  Led() {
    m_state = OFF;
    pin = LED_OUTPUT_PIN;
  }

  void init() {
    pinMode(pin, OUTPUT);
  }

  void set(LedState state) {
    m_state = state;
    digitalWrite(pin, m_state == ON ? HIGH : LOW);
  }

  void toggle() {
    LedState new_state = m_state == ON ? OFF : ON;
    set(new_state);
  }
};

Led LED = Led();

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(SERIAL_INIT_DELAY_MS);

  LED.init();
}

void loop() {
  unsigned long current_millis = millis();

  if (current_millis - last_millis_blink >= BLINK_DELAY_MS) {
    LED.toggle();
    last_millis_blink = current_millis;
  }
}
