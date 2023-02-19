#include <Arduino.h>
# include <esp_deep_sleep.h>
void setup() {
  // Initialize the ADC
  analogReadResolution(12); // Set the ADC resolution to 12 bits (0-4095)
  analogSetAttenuation(ADC_11db); // Set the input attenuation to 11 dB (input voltage range 0-3.6V)
  
  // Set up the serial monitor
  Serial.begin(115200);
  
  // Set up the deep sleep mode
  esp_sleep_enable_timer_wakeup(10 * 60 * 1000000); // Wake up every 10 minutes
}

void loop() {
  // Read the battery voltage
  int raw_adc = analogRead(A0); // Read the voltage from analog pin A0
  float battery_voltage = raw_adc * 0.000805664; // Convert the ADC reading to voltage (3.3V / 4095)
  
  // Print the battery voltage to the serial monitor
  Serial.print("Battery voltage: ");
  Serial.print(battery_voltage);
  Serial.println("V");
  delay(100);
  // Put the ESP32 into deep sleep mode
  //esp_deep_sleep_start();
} 