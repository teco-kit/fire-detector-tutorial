#define BUZZER_PIN 6    // Digital pin used to control the buzzer

void setup() {
  // Configure the buzzer pin as an output
  pinMode(BUZZER_PIN, OUTPUT);

  // Make sure the buzzer starts OFF
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  // --- Turn the buzzer ON ---
  // Active buzzer: HIGH = sound, LOW = no sound
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);  // Keep the buzzer on for 1 second

  // --- Turn the buzzer OFF ---
  digitalWrite(BUZZER_PIN, LOW);
  delay(1000);  // Stay silent for 1 second
}
