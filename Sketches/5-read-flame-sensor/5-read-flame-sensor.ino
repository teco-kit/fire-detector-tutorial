#include <Adafruit_TinyUSB.h>  // Required for USB Serial on XIAO boards

#define FIRE_SENSOR_PIN A0     // Analog pin where the flame sensor output is connected

void setup() {
  Serial.begin(115200);        // Start Serial communication at 115200 baud
  while (!Serial) delay(10);   // Wait for Serial connection (important for USB-based boards)

  pinMode(FIRE_SENSOR_PIN, INPUT);  // Configure the flame sensor pin as input
  Serial.println("Flame sensor test..."); // Initial message
}

void loop() {
  // Read analog value from the flame sensor
  int sensorValue = analogRead(FIRE_SENSOR_PIN);

  // Print the sensor value over Serial
  Serial.print("Value: ");
  Serial.println(sensorValue);

  delay(500); // Small delay between readings
}