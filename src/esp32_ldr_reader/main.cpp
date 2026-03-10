#include <HardwareSerial.h>
#include <esp32-hal-gpio.h>
#include <sstream>

#define LDR_INPUT_PIN 5

struct error_data {
   float value;
   float percentage;
};

error_data calculateError(float calculatedVoltage, float ldrVoltageValue);

static const float V_REF = 3.3;
static const adc_attenuation_t ATTENUATION = ADC_0db;
static const int MAX_ADC_BITS = 12;
static const int MAX_ADC_RANGE = std::pow(2, MAX_ADC_BITS);

void setup() {
  Serial.begin(115200);
  delay(250);

  analogReadResolution(MAX_ADC_BITS);
  analogSetPinAttenuation(LDR_INPUT_PIN, ATTENUATION);

  pinMode(LDR_INPUT_PIN, INPUT_PULLDOWN);
}

void loop() {
  int ldrRawValue = analogRead(LDR_INPUT_PIN);
  float calculatedVoltage = (((float) ldrRawValue) / MAX_ADC_RANGE) * V_REF;
  float ldrVoltageValue = (float) analogReadMilliVolts(LDR_INPUT_PIN) / 1000;
  error_data error = calculateError(calculatedVoltage, ldrVoltageValue);

  std::stringstream ss;
  ss << "LDR raw value = " << ldrRawValue
  << ", Calculated voltage = " << calculatedVoltage << "V"
  << ", LDR value = " << ldrVoltageValue << "V"
  << ", Error % = " << error.percentage << "%"
  << ", Error = " << error.value << "V";

  Serial.println(ss.str().c_str());

  delay(5000);
}

error_data calculateError(float calculatedVoltage, float ldrVoltageValue) {
  float lhs = ldrVoltageValue;
  float rhs = calculatedVoltage;
  int sign = -1;

  if (ldrVoltageValue < calculatedVoltage) {
    lhs = calculatedVoltage;
    rhs = ldrVoltageValue;
    sign = 1;
  }

  float percentage = sign * ((lhs / rhs * 100) - 100);
  float value = lhs - rhs;

  error_data ed;
  ed.percentage = percentage;
  ed.value = value;

  return ed;
}
