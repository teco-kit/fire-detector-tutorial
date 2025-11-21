#include <Adafruit_TinyUSB.h>

#define LED_PIN         LED_BUILTIN
#define FIRE_SENSOR_PIN A0
#define BUZZER_PIN      6

const int FIRE_THRESHOLD = 400; // adjust based on your readings

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  pinMode(LED_PIN, OUTPUT);
  pinMode(FIRE_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("Fire alarm running...");
}

void loop() {
  int val = analogRead(FIRE_SENSOR_PIN);
  Serial.print("Sensor: ");
  Serial.println(val);

  bool fire = val < FIRE_THRESHOLD;

  if (fire) {
    Serial.println(">>> FIRE DETECTED <<<");

    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);

      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, LOW);
      delay(100);
    }
    delay(500);

  } else {
    digitalWrite(BUZZER_PIN, LOW);

    digitalWrite(LED_PIN, LOW);
    delay(200);
    digitalWrite(LED_PIN, HIGH);
    delay(1800);
  }
}