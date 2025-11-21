// Simple heartbeat blink

#define LED_PIN LED_BUILTIN // active LOW, built in LED of the nrf52840 Sense

// this function is called once during startup
void setup() {
  pinMode(LED_PIN, OUTPUT); // configure the LED_PIN to be an output pin
}

// this function is called periodically after the device has booted
// you can adjust the timing as you wish
void loop() {
  digitalWrite(LED_PIN, LOW);   // LED ON, pulls the LED_PIN to ground
  delay(200);
  digitalWrite(LED_PIN, HIGH);  // LED OFF, pulls the LED_PIN high
  delay(1800);
}