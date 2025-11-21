#include <Adafruit_TinyUSB.h>
#include "LSM6DS3.h"
#include "Wire.h"

#define LED_PIN         LED_BUILTIN
#define FIRE_SENSOR_PIN A0
#define BUZZER_PIN      6

// ----------------------- Fire detection config -----------------------
const int FIRE_THRESHOLD = 40;   // adjust based on your readings

// ----------------------- IMU / shake config -------------------------
LSM6DS3 myIMU(I2C_MODE, 0x6A);

// Shake detection based on | |a| - 1g |
const float        SHAKE_DIFF_THRESHOLD = 0.30f;   // sensitivity (lower = more sensitive)
const unsigned long SHAKE_COOLDOWN_MS  = 150;      // minimum time between counting two shake events
const int          REQUIRED_SHAKES      = 3;       // shake events needed to stop the alarm

enum AlarmState {
  IDLE,
  ALARM_ACTIVE
};

AlarmState alarmState = IDLE;

// Shake event tracking
int           shakeEvents   = 0;
unsigned long lastShakeTime = 0;

// --------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  pinMode(LED_PIN, OUTPUT);
  pinMode(FIRE_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, HIGH);   // LED off (active LOW)
  digitalWrite(BUZZER_PIN, LOW); // buzzer off

  Wire.begin();
  if (myIMU.begin() != 0) {
    Serial.println("IMU init failed! Shake-to-disable will not work.");
  } else {
    Serial.println("IMU ready.");
  }

  Serial.println("Fire alarm with shake-to-disable running...");
}

// --------------------------------------------------------------------
int readFireSensor() {
  return analogRead(FIRE_SENSOR_PIN);
}

// Detects shake based on acceleration magnitude and counts shake events.
// Returns true once REQUIRED_SHAKES valid shake events have been detected.
bool detectShake() {
  float ax = myIMU.readFloatAccelX();
  float ay = myIMU.readFloatAccelY();
  float az = myIMU.readFloatAccelZ();

  float aMag = sqrt(ax * ax + ay * ay + az * az);     // total acceleration in g
  float diff = fabs(aMag - 1.0f);                     // deviation from gravity

  bool strong = diff > SHAKE_DIFF_THRESHOLD;

  // Debug output to understand thresholds and events
  Serial.print("IMU | |a|-1g = ");
  Serial.print(diff, 3);
  Serial.print("  strong=");
  Serial.print(strong ? "YES" : "no");
  Serial.print("  shakes=");
  Serial.println(shakeEvents);

  if (strong) {
    unsigned long now = millis();
    if (now - lastShakeTime > SHAKE_COOLDOWN_MS) {
      lastShakeTime = now;
      shakeEvents++;
      Serial.print(">>> SHAKE EVENT ");
      Serial.print(shakeEvents);
      Serial.println(" <<<");
    }
  }

  if (shakeEvents >= REQUIRED_SHAKES) {
    Serial.println("*** SHAKE LIMIT REACHED — STOP ALARM ***");
    shakeEvents = 0;   // reset for next time
    return true;
  }

  return false;
}

// ----------------------- LED / buzzer patterns ----------------------

// Heartbeat when no fire is detected
void heartbeatLED() {
  digitalWrite(LED_PIN, LOW);   // ON
  delay(200);
  digitalWrite(LED_PIN, HIGH);  // OFF
  delay(1800);
}

// One small alarm “step” with frequent shake checks; returns true if shaking should stop
bool alarmPatternStep() {
  // Short ON burst (LED + buzzer)
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, HIGH);
  for (int t = 0; t < 10; t++) {       // 10 * 10 ms = 100 ms
    if (detectShake()) {
      return true;
    }
    delay(10);
  }

  // Short OFF period
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);
  for (int t = 0; t < 10; t++) {       // 10 * 10 ms = 100 ms
    if (detectShake()) {
      return true;
    }
    delay(10);
  }

  return false;
}

// ----------------------- Main loop ----------------------------------
void loop() {
  int sensorVal = readFireSensor();
  Serial.print("Sensor: ");
  Serial.print(sensorVal);
  Serial.print("  | State: ");
  Serial.println(alarmState);

  bool fireDetected = sensorVal < FIRE_THRESHOLD;

  switch (alarmState) {
    case IDLE:
      // Only condition to start the alarm: fire detected
      if (fireDetected) {
        Serial.println(">>> FIRE DETECTED – alarm ACTIVE <<<");
        alarmState  = ALARM_ACTIVE;
        shakeEvents = 0;
      }
      break;

    case ALARM_ACTIVE: {
      // Run short alarm steps, checking shake in small intervals
      bool shouldStop = alarmPatternStep();

      // Only shaking is allowed to stop the alarm
      if (shouldStop) {
        Serial.println("*** ALARM STOPPED BY SHAKE – back to IDLE ***");
        alarmState = IDLE;
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_PIN, HIGH);
      }
      }
      break;
  }

  // Output behavior depending on the current state
  switch (alarmState) {
    case IDLE:
      heartbeatLED();
      break;

    case ALARM_ACTIVE:
      // alarmPatternStep() is already called in the state machine
      break;
  }
}