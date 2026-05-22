# Adaptive Traffic Signal Control System for Real-Time Optimization of Vehicular Flow at Urban Intersections

**Developer:** Jemimah Asma'u Kyauta Thompson  
**Supervisor / Portfolio:** Damilare Lekan Adekeye  
**Status:** Completed

---

## Overview

A rule-based intelligent traffic signal controller built on the Arduino Mega 2560. Two HC-SR04 ultrasonic sensors per lane (front + back) measure vehicle queue length in real time across a 4-lane cross intersection. Each lane is classified as **EMPTY**, **NORMAL**, or **CROWDED**, and green-light duration is allocated dynamically (0 s / 15 s / 30 s) to eliminate idle green phases.

A non-blocking `millis()`-based 6-state machine drives the controller. The **Smart Interrupt** feature immediately collapses a green phase when the active axis becomes empty while the opposing axis detects waiting vehicles. A hardware emergency override button (Pin 12) halts all traffic and displays an alert on the 20×4 I2C LCD.

---

## Repository Structure

```
firmware/
  code_main/
    code_main.ino       # Main Arduino firmware (Arduino Mega 2560)

circuit-diagrams/
  circuit-diagram-colour.pdf   # Colour Proteus schematic (full system)
  circuit-diagram-bw.pdf       # Monochrome Proteus schematic
  preview/
    circuit-diagram-colour.webp
    circuit-diagram-bw.webp
```

---

## Hardware

| Component | Spec | Qty |
|---|---|---|
| Microcontroller | Arduino Mega 2560 | 1 |
| Distance Sensors | HC-SR04 Ultrasonic | 8 |
| Traffic LEDs | Red / Yellow / Green 5 mm | 12 |
| LCD Display | 20×4 I2C (PCF8574, 0x27) | 1 |
| Emergency Button | Tactile push button (Pin 12) | 1 |
| Buck Converter | DC-DC step-down (12 V → 5 V) | 1 |
| Perfboard | 9×15 cm copper perfboard | 1 |

---

## Key Timing Constants

| Constant | Value | Condition |
|---|---|---|
| `TIME_NORMAL` | 15 000 ms | NORMAL traffic density |
| `TIME_CROWDED` | 30 000 ms | CROWDED traffic density |
| `TIME_EMPTY_OSCILLATE` | 30 000 ms | EMPTY axis (resting green) |
| `TIME_YELLOW` | 5 000 ms | Yellow transition (all phases) |
| Sensor scan interval | 300 ms | Background loop |
| LCD refresh interval | 250 ms | Background loop |
| Emergency debounce | 50 ms | Button debounce |

---

## Libraries Required

Install via Arduino Library Manager:

- **HCSR04** by Teckel (UltraSonicDistanceSensor)
- **LiquidCrystal I2C** by Frank de Brabander

---

## Portfolio

Full build documentation, photos, and project detail page:  
[damilarelekanadekeye.github.io](https://damilarelekanadekeye.github.io/works/embedded-systems/adaptive-traffic-signal/)
