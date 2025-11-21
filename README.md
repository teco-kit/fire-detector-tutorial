# Fire Alarm Tutorial using Seeed XIAO nRF52840 Sense  

> ⚠️ **Safety Disclaimer**  
> This is an educational prototype, not a certified fire detection system. Do not use it by any means.

# 1. Overview

This tutorial builds a simple prototype fire alarm using:

- Seeed XIAO nRF52840 Sense and its on-board LED  
- Analog flame sensor  
- Active buzzer  

# 2. Materials

## 2.1 Hardware

- **Seeed XIAO nRF52840 Sense**
- USB‑C data cable  
- Breadboard  
- Jumper wires  
- **Analog flame/light sensor** (VCC / GND / AO)
- **Active buzzer (3.3 V compatible)**

> ⚠️ XIAO uses **3.3 V** logic. Never connect 5 V devices to GPIO pins.

## 2.2 Software Requirements

### 2.2.1 Install Python (Required for Seeed nRF52 Core)

Make sure python is installed on your machine.

#### macOS

Install Python 3:

```bash
brew install python
```

Make sure `python` is in your $PATH and can be rund from your command line. Potentially create an Alias for `python3` (ask chatGPT).

#### Windows

Install from:  
https://www.python.org/downloads/windows/

**Check “Add Python to PATH”** during installation.

### 2.2.2 Arduino IDE  
Install from:  
https://www.arduino.cc/en/software

### 2.2.3 Seeed nRF52 Board Support  
In Arduino IDE: 
- File → Preferences → Additional Boards Manager URLs

Add: `https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json`

Then: 
- Tools → Board → Boards Manager → Search "Seeed nRF52"
- Install "Seeed nRF52 Boards"


# 3. Setup the Board

1. Take the board and put it onto the breadboard like shown in the image below.
  
2. Connect XIAO nRF52840 Sense via USB‑C  
3. Select:

```
Tools → Board → Seeed nRF52 Boards → Seeed XIAO nRF52840 Sense
Tools → Port → (Select XIAO USB port)
```

# 4. Step 1 – Make LED Blink to show that alarm is active
Before we build any real functionality, we first need a simple sign that the device is alive, like a heartbeat in a human.
A blinking LED is our device saying:

> I’m running, powered, and ready for instructions.

It’s the very first diagnostic feature of almost every embedded system. If this step doesn’t work, nothing else will.
It also gives immediate feedback to learners: “You uploaded code successfully — you’re in control.”

Copy the following sketch into your Arduino IDE. Per default, a *.ino sketch will already be open.

### Code

```cpp
// Simple heartbeat blink

#define LED_PIN LED_BUILTIN // active LOW, built in LED of the nrf52840 Sense

// this function is called once during startup
void setup() {
  pinMode(LED_PIN, OUTPUT);
}

// this function is called periodically after the device has booted
void loop() {
  digitalWrite(LED_PIN, LOW);   // LED ON
  delay(200);
  digitalWrite(LED_PIN, HIGH);  // LED OFF
  delay(1800);
}
```

---

# 7. Step 2 – Flame Sensor Reading

## Wiring

| XIAO Pin | Sensor Pin | Description |
|---------|-------------|-------------|
| 3V3     | VCC         | Power       |
| GND     | GND         | Ground      |
| A0      | AO          | Analog signal |

---

## Sketch 2 – Flame Sensor

Create folder:

```
02_Flame_Sensor_Read/02_Flame_Sensor_Read.ino
```

### Code

```cpp
#include <Adafruit_TinyUSB.h>  // IMPORTANT for Serial on XIAO

// 02_Flame_Sensor_Read.ino

#define FIRE_SENSOR_PIN A0

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  pinMode(FIRE_SENSOR_PIN, INPUT);
  Serial.println("Flame sensor test...");
}

void loop() {
  int sensorValue = analogRead(FIRE_SENSOR_PIN);
  Serial.print("Value: ");
  Serial.println(sensorValue);
  delay(500);
}
```

---

# 8. Step 2.5 – Heartbeat LED + Sensor

Create folder:

```
03_Heartbeat_And_Sensor/03_Heartbeat_And_Sensor.ino
```

### Code

```cpp
#include <Adafruit_TinyUSB.h>

#define LED_PIN LED_BUILTIN
#define FIRE_SENSOR_PIN A0

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  pinMode(LED_PIN, OUTPUT);
  pinMode(FIRE_SENSOR_PIN, INPUT);
  Serial.println("Heartbeat + Sensor running...");
}

void loop() {
  digitalWrite(LED_PIN, LOW);      // LED ON
  int sensorValue = analogRead(FIRE_SENSOR_PIN);
  Serial.println(sensorValue);
  delay(200);

  digitalWrite(LED_PIN, HIGH);     // LED OFF
  delay(1800);
}
```

Use this to determine your threshold:

Example values:

```
No flame: ~800
Flame:    ~200
-> Threshold: ~400
```

---

# 9. Step 3 – Add the Buzzer (Alarm Output)

## Wiring

| XIAO Pin | Buzzer Pin |
|----------|------------|
| D6       | +          |
| GND      | -          |

Use a **small active buzzer**.

---

# 10. Step 4 – Full Fire Alarm

Create folder:

```
04_Fire_Alarm/04_Fire_Alarm.ino
```

### Code

```cpp
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
```

---

# 11. Fritzing Diagram (Optional)

1. Install: https://fritzing.org/download  
2. Add:
   - XIAO nRF52840 Sense  
   - Flame sensor  
   - Buzzer  
   - Breadboard  
3. Wire exactly as described.  
4. Export PNG or `.fzz` for documentation.

---

# 12. Repo Structure

```
.
├── README.md
├── 01_Alive_LED
│   └── 01_Alive_LED.ino
├── 02_Flame_Sensor_Read
│   └── 02_Flame_Sensor_Read.ino
├── 03_Heartbeat_And_Sensor
│   └── 03_Heartbeat_And_Sensor.ino
└── 04_Fire_Alarm
    └── 04_Fire_Alarm.ino
```

---

# 13. Done!

This README is fully corrected for:

- Python requirements  
- Seeed nRF52 Serial fix  
- Working sketches  

If you want, I can generate a **PDF**, **ZIP with sketches**, or a **Fritzing file**.
