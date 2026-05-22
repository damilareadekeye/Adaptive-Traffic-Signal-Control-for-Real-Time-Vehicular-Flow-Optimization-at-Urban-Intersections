# Adaptive Traffic Signal Control System for Real-Time Optimization of Vehicular Flow at Urban Intersections

**Developer:** Jemimah Asma'u Kyauta Thompson  
**Supervisor / Portfolio:** Damilare Lekan Adekeye  
**Status:** Completed

---

## Overview

A rule-based intelligent traffic signal controller built on the Arduino Mega 2560. Two HC-SR04 ultrasonic sensors per lane (front + back) measure vehicle queue length in real time across a 4-lane cross intersection. Each lane is classified as **EMPTY**, **NORMAL**, or **CROWDED**, and green-light duration is allocated dynamically (0 s / 15 s / 30 s) to eliminate idle green phases.

A non-blocking `millis()`-based 6-state machine drives the controller. The **Smart Interrupt** feature immediately collapses a green phase when the active axis becomes empty while the opposing axis detects waiting vehicles. A hardware emergency override button (Pin 12) halts all traffic and displays an alert on the 20x4 I2C LCD.

---

## How It Works

```
HC-SR04 Sensors (x8, 300 ms scan)
        |
        v
Density Classification: EMPTY / NORMAL / CROWDED
        |
        v
Axis Aggregation: axis_status = max(lane_A, lane_B)
        |
        v
6-State Machine: AXIS1_EVAL -> AXIS1_GREEN -> AXIS1_YELLOW
                 -> AXIS2_EVAL -> AXIS2_GREEN -> AXIS2_YELLOW -> ...
        |
  Smart Interrupt check on every GREEN tick
        |
        v
12 Traffic LEDs (R/Y/G x 4 lanes)  +  20x4 I2C LCD (250 ms refresh)
        |
Emergency Override (Pin 12, 50 ms debounce) -> All RED + LCD alert
```

---

## Repository Structure

```
arduino-firmware/
  code_main/
    code_main.ino              # Production firmware (Arduino Mega 2560)
  reference-sketches/
    LCD2004-test/
      LCD2004-test.ino         # I2C LCD address scan and display test
    LED-test/
      LED-test.ino             # 12-LED individual pin verification
    Ultrasonic/
      Ultrasonic.ino           # HC-SR04 single-sensor distance read test

circuit-diagrams/
  circuit-diagram-colour.pdf   # Colour Proteus schematic (full system)
  circuit-diagram-bw.pdf       # Monochrome Proteus schematic
  preview/
    circuit-diagram-colour.webp
    circuit-diagram-bw.webp

docs/
  MASTERPLAN.md                # Full build guide and knowledge base
```

---

## Hardware

| Component | Model / Spec | Qty |
|---|---|---|
| Microcontroller | Arduino Mega 2560 | 1 |
| Distance Sensors | HC-SR04 Ultrasonic | 8 |
| Traffic LEDs | Red / Yellow / Green 5 mm | 12 |
| LCD Display | 20x4 I2C, PCF8574 backpack, address 0x27 | 1 |
| Emergency Button | Tactile push button, Pin 12 (INPUT_PULLUP) | 1 |
| Console LED | Pin 13 (solid = auto, flash = emergency) | 1 |
| Buck Converter | DC-DC step-down 12 V to 5 V | 1 |
| Perfboard | 9 x 15 cm copper perfboard | 1 |
| Resistors | 220 ohm (LED current limiting) | 12 |

---

## Pin Assignments

### Ultrasonic Sensors

| Sensor | Lane | Trig | Echo |
|---|---|---|---|
| L1_Sens1 | Lane 1 Front (Axis 1) | 23 | 22 |
| L1_Sens2 | Lane 1 Back (Axis 1) | 24 | 25 |
| L4_Sens1 | Lane 4 Front (Axis 1) | 28 | 29 |
| L4_Sens2 | Lane 4 Back (Axis 1) | 30 | 31 |
| L2_Sens1 | Lane 2 Front (Axis 2) | 36 | 37 |
| L2_Sens2 | Lane 2 Back (Axis 2) | 34 | 35 |
| L3_Sens1 | Lane 3 Front (Axis 2) | 32 | 33 |
| L3_Sens2 | Lane 3 Back (Axis 2) | 26 | 27 |

### LED Outputs

| Pin | Signal | Lane |
|---|---|---|
| 9 / 10 / 11 | R / Y / G | Lane 1 (Top) |
| 16 / 14 / 15 | R / Y / G | Lane 4 (Bottom) |
| 6 / 8 / 7 | R / Y / G | Lane 2 (Left) |
| 3 / 4 / 5 | R / Y / G | Lane 3 (Right) |
| 12 | Emergency button (INPUT_PULLUP) | |
| 13 | Console LED | |

---

## Key Timing Constants

| Constant | Value | Condition |
|---|---|---|
| `TIME_NORMAL` | 15 000 ms | NORMAL traffic density |
| `TIME_CROWDED` | 30 000 ms | CROWDED traffic density |
| `TIME_EMPTY_OSCILLATE` | 30 000 ms | EMPTY axis (resting green) |
| `TIME_YELLOW` | 5 000 ms | Yellow transition (all phases) |
| Sensor scan interval | 300 ms | Background polling loop |
| LCD refresh interval | 250 ms | Background display loop |
| Emergency debounce | 50 ms | Button debounce window |

---

## Libraries Required

Install via Arduino Library Manager:

- **HCSR04** by Teckel (`UltraSonicDistanceSensor`)
- **LiquidCrystal I2C** by Frank de Brabander

---

## Quick Start

1. Open `arduino-firmware/code_main/code_main.ino` in Arduino IDE
2. Install the two libraries above via Library Manager
3. Select **Arduino Mega 2560** as the board and correct COM port
4. Upload the sketch
5. On power-up the LCD shows a 5 s splash screen then enters autonomous control
6. Press Pin 12 button to toggle emergency override on/off

---

## Portfolio

Full build documentation, 134 build photos, circuit diagrams, and project detail page:  
https://damilareadekeye.com/works/embedded-systems/adaptive-traffic-signal/
