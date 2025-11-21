#include <Adafruit_TinyUSB.h>
#include <PDM.h>              // Microphone (PDM) on XIAO nRF52840 Sense
#include "LSM6DS3.h"          // IMU
#include "Wire.h"

#define LED_PIN         LED_BUILTIN
#define FIRE_SENSOR_PIN A0
#define BUZZER_PIN      6

const int FIRE_THRESHOLD          = 40;     // adjust based on your readings
const int16_t MIC_ALARM_THRESHOLD = 2500;   // tune this
const float SHAKE_ACCEL_THRESHOLD = 2.0f;   // g, tune this

// ---------- State Machine ----------
enum AlarmState {
  STATE_IDLE,
  STATE_ALARM
};

AlarmState alarmState = STATE_IDLE;

// ---------- Mic / PDM ----------
short sampleBuffer[256];         // 16-bit samples
volatile int samplesRead = 0;

// ---------- IMU ----------
LSM6DS3 myIMU(I2C_MODE, 0x6A);   // I2C address for XIAO BLE Sense IMU

// PDM callback
void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(sampleBuffer, bytesAvailable);
  samplesRead = bytesAvailable / 2;  // 2 bytes per sample
}

// Compute a crude sound level from the mic
int16_t getMicLevel() {
  int16_t maxSample = 0;

  if (samplesRead) {
    for (int i = 0; i < samplesRead; i++) {
      int16_t s = sampleBuffer[i];
      if (s < 0) s = -s;
      if (s > maxSample) maxSample = s;
    }
    samplesRead = 0; // mark buffer as processed
  }

  return maxSample;
}

// Detect strong shake using accelerometer magnitude
bool isShaken() {
  float ax = myIMU.readFloatAccelX();
  float ay = myIMU.readFloatAccelY();
  float az = myIMU.readFloatAccelZ();

  float mag = sqrt(ax * ax + ay * ay + az * az); // in g
  return mag > SHAKE_ACCEL_THRESHOLD;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  pinMode(LED_PIN, OUTPUT);
  pinMode(FIRE_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("Fire alarm with mic + shake reset running...");

  // ---------- Init IMU ----------
  if (myIMU.begin() != 0) {
    Serial.println("IMU error");
  } else {
    Serial.println("IMU OK");
  }

  // ---------- Init PDM mic ----------
  PDM.onReceive(onPDMdata);

  // mono, 16 kHz
  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to start PDM!");
    while (1) {
      delay(1000);
    }
  }

  // Optional: adjust mic gain
  // PDM.setGain(30);

  alarmState = STATE_IDLE;
}

void loop() {
  // ----- Read sensors (fast) -----
  int val = analogRead(FIRE_SENSOR_PIN);
  bool fire = val < FIRE_THRESHOLD;

  int16_t micLevel = getMicLevel();
  bool heardExternalAlarm = (micLevel > MIC_ALARM_THRESHOLD);

  // Debug output (reduce if it gets too spammy)
  Serial.print("Fire: ");
  Serial.print(val);
  Serial.print("  Mic: ");
  Serial.println(micLevel);

  switch (alarmState) {

    case STATE_IDLE: {
      // ---- TRIGGERS TO ENTER ALARM ----
      if (fire) {
        Serial.println(">>> FIRE DETECTED (sensor) <<<");
        alarmState = STATE_ALARM;
        break;
      } else if (heardExternalAlarm) {
        Serial.println(">>> EXTERNAL ALARM DETECTED (mic) <<<");
        alarmState = STATE_ALARM;
        break;
      }

      // ---- IDLE PATTERN, BUT MIC/FIRE CHECKED OFTEN ----
      // We simulate the old 200ms ON / 1800ms OFF pattern
      // as many 50 ms steps where we re-check mic + fire.

      const int stepMs = 50;

      // 200 ms LED OFF (4 x 50 ms)
      for (int i = 0; i < 4; i++) {
        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
        delay(stepMs);

        // re-check triggers every 50 ms
        int val2 = analogRead(FIRE_SENSOR_PIN);
        bool fire2 = val2 < FIRE_THRESHOLD;
        int16_t micLevel2 = getMicLevel();
        bool heardExternalAlarm2 = (micLevel2 > MIC_ALARM_THRESHOLD);

        if (fire2 || heardExternalAlarm2) {
          if (fire2) {
            Serial.println(">>> FIRE DETECTED (sensor, idle step) <<<");
          } else {
            Serial.println(">>> EXTERNAL ALARM DETECTED (mic, idle step) <<<");
          }
          alarmState = STATE_ALARM;
          break;
        }
      }
      if (alarmState != STATE_IDLE) break;

      // 1800 ms LED ON (36 x 50 ms)
      for (int i = 0; i < 36; i++) {
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, LOW);
        delay(stepMs);

        int val2 = analogRead(FIRE_SENSOR_PIN);
        bool fire2 = val2 < FIRE_THRESHOLD;
        int16_t micLevel2 = getMicLevel();
        bool heardExternalAlarm2 = (micLevel2 > MIC_ALARM_THRESHOLD);

        if (fire2 || heardExternalAlarm2) {
          if (fire2) {
            Serial.println(">>> FIRE DETECTED (sensor, idle step) <<<");
          } else {
            Serial.println(">>> EXTERNAL ALARM DETECTED (mic, idle step) <<<");
          }
          alarmState = STATE_ALARM;
          break;
        }
      }

    } break;

    case STATE_ALARM: {
      Serial.println(">>> ALARM STATE <<<");

      // Alarm pattern, but check for shake to reset
      for (int i = 0; i < 5; i++) {
        if (isShaken()) {
          Serial.println("Shake detected -> reset alarm");
          alarmState = STATE_IDLE;
          break;
        }

        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);

        if (isShaken()) {
          Serial.println("Shake detected -> reset alarm");
          alarmState = STATE_IDLE;
          break;
        }

        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, LOW);
        delay(100);
      }

      if (alarmState == STATE_ALARM) {
        delay(200);  // short pause before next alarm burst
      } else {
        // just reset
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_PIN, HIGH);
        delay(200);
      }

    } break;
  }
}
