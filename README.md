# 🔥 Fire Alarm Tutorial using Seeed XIAO nRF52840 Sense  
This guide walks you through building a simple but functional fire-detection prototype using the Seeed XIAO nRF52840 Sense.
It’s a hands-on introduction to reading analog sensors, reacting to thresholds, and driving outputs like LEDs and buzzers. 
The tutorial is essentially a compact example of how embedded systems sense the world and respond to it.

<img height="400" alt="image" src="https://github.com/user-attachments/assets/57433d65-d407-47e3-ba9c-10353f4bfc80" />


> ⚠️ **Safety Disclaimer**  
> This is an educational prototype, not a certified fire detection system. Do not use it by any means.
> The sensor is triggered by infrared light, hence it might also trigger in direct sunlight. This is a common problem even for commercial infrared fire alarms.

# 1. Overview

This tutorial builds a simple prototype fire alarm using:

- Seeed XIAO nRF52840 Sense and its on-board LED  
- Analog flame sensor  
- Active buzzer  

# 2. Materials

## 2.1 Hardware

- [Seeed XIAO nRF52840 Sense](https://wiki.seeedstudio.com/XIAO_BLE/)
- Analog flame sensor (VCC / GND / AO)
- Active buzzer (3.3 V compatible)
- USB‑C data cable  
- Breadboard  
- Jumper wires  


> ⚠️ XIAO uses **3.3 V** logic. Never connect 5 V devices to GPIO pins.

## 2.2 Software Requirements

### 2.2.1 Install Python (Required for Seeed nRF52 Core)

Make sure python is installed on your machine.

#### macOS

Install Python 3:

```bash
brew install python
```

Install [brew](https://brew.sh) first, if it's not installed.

⚠️ Make sure `python` is in your $PATH and can be rund from your command line. Potentially create an Alias for `python3` (ask chatGPT).
⚠️ If you experience any issues on macOS with compiling, please scroll down until "Flash the nRF52840", there's troubleshooting steps

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

### Pin Overview
This pinout diagram shows all digital, analog, power, and communication pins of the XIAO nRF52840 Sense. For this project we use A0 for the flame sensor and D6 for the buzzer, along with 3V3 and GND for power.

<img height="400" alt="image" src="https://github.com/user-attachments/assets/613011af-589a-43a0-98b3-4fcdd56728c4" />


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

### Flash the nRF52840

To flash the code to your microcontroller, click the "Upload" button (arrow) in the top left corner of the Arduino IDE. 

⚠️ **Some people experience the following issue when flashing: The microcontroller might not show up anymore, the port disconnects, etc. as soon as there is some program running on it. When this happens, you have to double-press the tiny reset button (quite rapid "click-click") of the microcontroller to make the upload work and connect the port again. If you get notifications saying the board disconnected, don't worry as long as it says "upload complete". To connect to the Serial Monitor, you have to double click "reset" again, select the port again and then it should work.**

⚠️ **If you're running macOS and experience the following error**

```
Traceback (most recent call last): File "main.py", line 317, in File "click/core.py", line 1134, in call File "click/core.py", line 1040, in main File "click/_unicodefun.py", line 100, in _verify_python_env RuntimeError: Click will abort further execution because Python was configured to use ASCII as encoding for the environment. Consult https://click.palletsprojects.com/unicode-support/ for mitigation steps.
```
here's how fix it:
- download [this](https://github.com/teco-kit/fire-detector-tutorial/raw/refs/heads/main/adafruit-nrfutil) file
- go to `~/Library/Arduino15/packages/Seeeduino/hardware/nrf52/1.1.12/tools/adafruit-nrfutil/macos/`, e.g. by opening the Terminal app: paste `open ~/Library/Arduino15/packages/Seeeduino/hardware/nrf52/1.1.12/tools/adafruit-nrfutil/macos/` and hit enter
- replace the existing `adafruit-nrfutil` file with the downloaded file
- go back to the command line in the Terminal app and paste `chmod +x ~/Library/Arduino15/packages/Seeeduino/hardware/nrf52/1.1.12/tools/adafruit-nrfutil/macos/adafruit-nrfutil` and hit enter
- restart the Arduino IDE and try to flash again
- macOS will probably complain and tell you it can't execute the file because it's not trusted
- open the System Settings and head to "Privacy & Security", scroll down all the way and where it says "adafruit-nrfutil" click on "Open Anyway"
- go back to the Arduino IDE and try to flash again
- macOS will probably ask you if you're sure you want to execute this untrusted file; click "Open"
- now it should actually work 🤦🏼‍♂️



### What does the code do?

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
Now that the LED blink confirmed the board is alive, we can move to our first real input device: a simple analog flame sensor.
This sensor outputs a voltage that increases when it detects infrared light (the strong IR signature of a flame).
Our microcontroller then measures this voltage using its built-in analog-to-digital converter (ADC).

What is actually going on electrically?

The sensor continuously outputs a voltage between 0 V and ~3.3 V, depending on how much infrared light it sees.

- No flame → high voltage
- Strong flame → low voltage

Your XIAO board measures this voltage using the ADC and gives you a number typically from 0 to 1023 (10-bit resolution), where:

- 0 ≙ 0 V
- 1023 ≙ 3.3 V

This means you’re not detecting “fire yes/no,” but rather a continuous intensity value that you can later threshold to make decisions.

### Wiring

<img height="400" alt="sensor" src="https://github.com/user-attachments/assets/4e2aa8d6-5deb-4ded-8dca-245552db5915" />

You’ll notice a pair of red and black wires going to the breadboard rails. These “extend” the XIAO Sense’s power (red, 3.3V) and ground (black, GND) pins across the breadboard, making it easier to plug in additional components later and keeping everything neat and organized. From these rails, the flame sensor is powered by connecting to the `+` pin of the breakout board. Without this, the sensor wouldn’t turn on.

Next, we connect the ground (GND) of the XIAO Sense to the GND of the flame sensor. This shared ground creates a common electrical reference between the devices. Without a shared ground, the signal coming from the sensor would appear “floating” and the XIAO wouldn’t be able to interpret it correctly. 

Finally, the most important part is the signal wire. The flame sensor outputs an analog signal. This signal is routed from the flame sensor's A0 pin to the A0 analog input pin of the XIAO Sense.

With these three essential connections: power, ground, and signal; the flame sensor becomes fully functional and ready to decode infrared light coming from flames.

### Code

Replace the code inside your sketch with the code below.

```cpp
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
```

**What does this code do?**

`Serial.begin(115200);`
Opens a USB serial connection so we can print sensor values. 115200 is the baudrate, which configures the communication speed.

`while (!Serial);`
Waits until the computer actually opens the serial port—important for the XIAO platform. This ensures we are not sending any data before the computer connects to the board.

`analogRead(FIRE_SENSOR_PIN);`
Converts the analog voltage from the sensor into a number between 0–1023.

`Serial.println(sensorValue)`
You'll see the intensity change in the Serial Monitor when you bring a flame (or even a bright lighter LED) close to the sensor.

**How to receive the Serial output?**
The Arduino IDE has a built-in serial monitor. You can find it in the top-right corner (magnifying glass icon).

**How to calibrate the sensor?**

To calibrate the flame sensor, you use the small blue potentiometer (the tiny adjustable dial) on the sensor board. This dial controls the analog sensitivity by adjusting the internal reference voltage of the amplifier, which determines how much infrared light is required before the sensor output changes noticeably. 

<img height="300" alt="image" src="https://github.com/user-attachments/assets/d3c0c60d-caf9-4447-b404-5922c26ad83b" />

Turning the dial clockwise generally makes the sensor more sensitive, reacting to smaller amounts of IR light, while turning it counter-clockwise decreases sensitivity so that only stronger IR signals trigger a response. To calibrate it properly, open the Arduino Serial Monitor so you can observe the live sensor values. Then follow these steps:

- Point the sensor at a flame (lighter) and watch how the analog values change.
- If the numbers barely change when a flame is present, increase sensitivity by turning the dial clockwise in very small increments.
- If the sensor reacts even without a flame (for example from sunlight or reflections), decrease sensitivity by turning the dial counter-clockwise.

Stop adjusting when:
- No flame → readings stay stable
- Flame present → values change clearly and consistently

**⚠️ For best results, avoid calibrating in direct sunlight. In genral, the sensor will not properly detect flames in direct sunlight.**


# 6. Add the buzzer
When fire alarm detects something, you’ll want the system to react in a way that’s impossible to miss.
That’s where the active buzzer comes in. An active buzzer already contains its own oscillator, meaning you don’t have to generate tones or waveforms in software.
All you do is drive a pin HIGH to turn it on, and LOW to turn it off, just like controlling an LED.

This makes it perfect for alarms as it has simple wiring and no timing-critical code while it is loud and gives immediate feedback.

In our project, the buzzer becomes the “alarm output” that reacts whenever the flame sensor value drops below your fire threshold.
Later, in the full sketch, we combine LED flashing and buzzer activation to create a clear, attention-grabbing warning signal.


### Wiring

<img width="892" height="979" alt="image" src="https://github.com/user-attachments/assets/fc0f5a88-17fc-40eb-aac7-55f03f240626" />


### Code
```cpp
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

```


# 7. Putting it all together

By now, you’ve explored and tested every major component of your system individually:
- the heartbeat LED shows the device is alive
- the flame sensor provides continuous analog readings
- the buzzer can generate a loud alarm signal

You’ve seen how each part behaves on its own and now it’s time to bring them together.

In this step, you will build the complete fire alarm yourself.
That means writing the logic that links sensing, decision-making, and reacting into one continuous loop.
Your final program should:
- read the flame sensor value in real time
- compare the value against your chosen threshold
- trigger the buzzer and LED when a flame is detected
- keep the LED blinking normally when everything is safe
- (optional) print useful information over Serial so you can debug what’s happening

Use the knowledge from the previous sections to write your own combined sketch.
Once finished, you’ll have a fully working, responsive prototype fire alarm running on the XIAO nRF52840 Sense.
In the folder `Sketches` of this repository you can also find an example solution `7-simple-fire-alarm.ino`.

# 8. BONUS: Shake to disable fire alarm
In this optional extension, you add a basic user interaction to your fire alarm using the built-in IMU of the XIAO nRF52840 Sense.
The idea: when the alarm is active, only shaking the board will disable the alarm.

This teaches you how to:
- read acceleration values from the onboard IMU
- detect shaking
- implement a simple state machine (e.g., alarm active vs. alarm idle)

You can refer to the example solution code in the `Sketches` folder called `8-shake-to-disable.ino`.

> Hint: For this task you will have to install the `Seeed Arduino LSM6DS3` library using the Arduino library manager.

# 9. BONUS: Cascading fire alarm
Real fire-alarm systems often work in groups, where one alarm triggers others nearby.
In this bonus task, you implement a simple version of this behavior using the XIAO’s on-board microphone.

Your device should continuously listen for the characteristic sound pattern of another alarm (for example a buzzer tone).
If it detects such a sound, your device should activate its own alarm — even if the flame sensor has not yet been triggered.

You can refer to the example solution code in the `Sketches` folder called `9-full-system.ino`.

> Tip: You can assume that the surrounding sounds are ignored and you trigger an alarm if you hear loud enough beeps from another fire alarm.
> If you really want to go all in, you can add a cooldown for the alarm after shaking to reset.
