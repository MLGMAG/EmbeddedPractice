#include <Arduino.h>
#include "esp_adc_cal.h"

#define ADC_PIN 35

void setup() {
  Serial.begin(9600);
  delay(250);
}

float get_calibrated_voltage(float adc_value) {
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  return ((float) esp_adc_cal_raw_to_voltage(adc_value, &adc_chars)) / 1000;
}

void loop() {
  int analog_read = analogRead(ADC_PIN);

  float voltage_calibrated = get_calibrated_voltage(analog_read);
  float voltage_manual = ((float) analog_read) / 4095 * 3.3;

  float error = abs((voltage_calibrated - voltage_manual) / 3.3 * 100);

  Serial.printf("%d\t%f\t%f\t%f\n", analog_read, voltage_manual, voltage_calibrated, error);
  delay(100);
}

