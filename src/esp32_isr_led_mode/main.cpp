#include <HardwareSerial.h>
#include <esp32-hal-gpio.h>

constexpr uint8_t INPUT_PIN = 5;
constexpr uint8_t LED_OUTPUT_PIN = 6;
constexpr unsigned long SERIAL_BAUD_RATE = 115200;
constexpr uint32_t SERIAL_INIT_DELAY_MS = 250;
constexpr unsigned long BUTTON_PRESS_DELAY_MS = 200;
constexpr unsigned long BLINK_DELAY_MS = 1000;

volatile unsigned long press_count = 0;
volatile int button_pressed = 0;
volatile unsigned long last_millis_press = 0;
volatile unsigned long last_millis_blink = 0;

enum LedState { ON, OFF };
enum LedMode { ALWAYS_ON, ALWAYS_OFF, BLINK };

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
volatile LedMode CURRENT_LED_MODE = ALWAYS_OFF;

void buttonPressedInterruptHandler() {
  button_pressed = 1;
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(SERIAL_INIT_DELAY_MS);

  attachInterrupt(INPUT_PIN, buttonPressedInterruptHandler, FALLING);
  LED.init();
}

void loop() {
  unsigned long current_millis = millis();

  if (current_millis - last_millis_press >= BUTTON_PRESS_DELAY_MS) {
    if (button_pressed == 1) {
      press_count++;
      press_count %= 3;
      switch (press_count) {
        case 0:
          CURRENT_LED_MODE = ALWAYS_OFF;
          break;
        case 1:
          CURRENT_LED_MODE = BLINK;
          break;
        case 2:
          CURRENT_LED_MODE = ALWAYS_ON;
          break;
        default:
          break;
      }
      button_pressed = 0;
    }
    last_millis_press = current_millis;
  }

  if (CURRENT_LED_MODE == ALWAYS_ON) {
    LED.set(ON);
  }

  if (CURRENT_LED_MODE == ALWAYS_OFF) {
    LED.set(OFF);
  }

  if (CURRENT_LED_MODE == BLINK && current_millis - last_millis_blink >= BLINK_DELAY_MS) {
    LED.toggle();
    last_millis_blink = current_millis;
  }
}
