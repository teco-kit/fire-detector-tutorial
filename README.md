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

⚠️ Make sure `python` is in your $PATH and can be rund from your command line. Potentially create an Alias for `python3` (ask chatGPT).

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

<img height="400" alt="arduino" src="https://github.com/user-attachments/assets/5f563fd4-3c65-4434-a0b5-8803135ecb2e" />
  
2. Connect XIAO nRF52840 Sense via USB‑C to your computer
3. Select:

```
Tools → Board → Seeed nRF52 Boards → Seeed XIAO nRF52840 Sense
Tools → Port → (Select XIAO USB port)
```

# 4. Make LED Blink to show that device is alive
Before we build any real functionality, we first need a simple sign that the device is alive, like a heartbeat in a human.
A blinking LED is our device saying:

> I’m running, powered, and ready for instructions.

It’s the very first diagnostic feature of almost every embedded system. If this step doesn’t work, nothing else will.

Then, copy the following sketch into your Arduino IDE. Per default, a *.ino sketch will already be open, which you can just use.

### Code

```cpp
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
```

**What is actually happening?**

When your board boots, the microcontroller starts executing the code you uploaded to it. In Arduino terminology:

`setup()` runs once, immediately after startup.
This is where we configure pins, sensors, communication interfaces, and anything that needs to be initialized.

`loop()` then runs over and over, forever, as long as the device has power.
This is where the device spends most of its life—doing tasks, reading sensors, reacting to inputs, and updating outputs.

To verify that the chip is truly running the `loop()` function, we toggle the built-in LED on and off. If the LED blinks, it proves:
- your firmware successfully uploaded
- the microcontroller is executing your program
- basic GPIO output works
- the timing functions (delay) work

This is why almost every embedded tutorial starts with blinking an LED: it’s the embedded version of "Hello World", often also called "Hello Blinky".

**Why does pulling the pin LOW turn the LED ON?**

On this specific nRF52840 Sense board, the LED is wired in an active-LOW configuration.
That means:
- LED turns ON when the microcontroller pin is pulled to GND (LOW)
- LED turns OFF when the pin is driven to HIGH (3.3V)

The schematic shows that the LED’s anode (positive side) is connected to VCC through a resistor, and the cathode (negative side) goes to the P0.26 pin.
So the microcontroller is essentially providing a path to ground to let current flow.

<img width="328" height="456" alt="image" src="https://github.com/user-attachments/assets/0a4a5979-5c3c-4890-bafd-ee0a91c0c030" />

If you imagine it like water flow:
- HIGH = valve closed → no flow → LED off
- LOW = valve open → current flows → LED on

Understanding this is important because many embedded boards use active-LOW LEDs, and beginners often get confused when setting a pin to LOW makes something turn on.




# 5. Connect the flame sensor and read sensor values

## Wiring

| XIAO Pin | Sensor Pin | Description |
|---------|-------------|-------------|
| 3V3     | VCC         | Power       |
| GND     | GND         | Ground      |
| A0      | AO          | Analog signal |

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

# 9. Step 3 – Add the Buzzer (Alarm Output)



## Wiring

| XIAO Pin | Buzzer Pin |
|----------|------------|
| D6       | +          |
| GND      | -          |

<img height="400" alt="full" src="https://github.com/user-attachments/assets/b53d7e43-6a69-45fe-bf40-aa0ba5254fd3" />


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
