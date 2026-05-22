# Adaptive Traffic Signal Control System for Real-Time Optimization of Vehicular Flow at Urban Intersections

**Developer:** Jemimah Asma'u Kyauta Thompson | Matric: 21CK029344  
**Institution:** Covenant University, Ota, Ogun State  
**Technical Implementation:** Deewansonic Engineering  
**Portfolio:** Damilare Lekan Adekeye  
**Status:** Completed

> Full build documentation, 134 build photos, circuit diagrams, and detailed project writeup:  
> **https://damilareadekeye.com/works/embedded-systems/adaptive-traffic-signal/**

---

## Overview

Traditional fixed-cycle traffic lights waste time on empty roads and cause unnecessary congestion on busy approaches. This project replaces that rigid pattern with a **real-time, rule-based adaptive controller** built on the Arduino Mega 2560.

Two HC-SR04 ultrasonic sensors per lane (front + back) measure vehicular queue length continuously. Each lane is classified as **EMPTY**, **NORMAL**, or **CROWDED**. Green-light duration is allocated dynamically:

| Density | Green Duration | Condition |
|---|---|---|
| EMPTY | 30 s (resting oscillation) | No vehicles on either sensor |
| NORMAL | 15 s | Front sensor occupied only (short queue) |
| CROWDED | 30 s | Both sensors occupied (heavy congestion) |

A **Smart Interrupt** feature instantly collapses a green phase the moment the active axis empties while the opposing axis detects waiting vehicles, completely eliminating idle waiting time.

---

## How It Works

```
Power On
   |
   v
All Lanes -> RED (safety state)
LCD Splash Screen: Developer credentials (5 s)
   |
   v
+----------------------- Autonomous Adaptive Mode -----------------------+
|                                                                        |
|   Background scan (every 300 ms):                                     |
|   HC-SR04 x8 -> getLaneDensity() -> scanAllLanes()                   |
|   axis1Status = max(Lane1_density, Lane4_density)                     |
|   axis2Status = max(Lane2_density, Lane3_density)                     |
|                                                                        |
|   State Machine (manageTrafficLogic()):                               |
|                                                                        |
|   AXIS1_EVAL -> set green duration by density -> AXIS1_GREEN          |
|        |                                             |                 |
|        |         Smart Interrupt check              |                 |
|        |    (axis1 EMPTY + axis2 > EMPTY?           |                 |
|        |     -> collapse currentDuration = 0)        |                 |
|        |                                             v                 |
|        +---> AXIS1_YELLOW (5 s) -> AXIS2_EVAL -> AXIS2_GREEN         |
|                                                      |                 |
|                                         Smart Interrupt check         |
|                                         -> AXIS2_YELLOW (5 s)        |
|                                              |                         |
|                                         AXIS1_EVAL  (loop)            |
|                                                                        |
|   LCD refresh (every 250 ms): active axis, density, mode, countdown  |
|                                                                        |
|   Emergency button check (every loop tick, 50 ms debounce):          |
|   -> Toggle emergencyMode: all RED, LCD alert, consoleLED flash       |
+------------------------------------------------------------------------+
```

---

## The Axis System

The 4-lane cross-intersection is paired into two opposing axes, mirroring real-world civil engineering standards:

| Axis | Lanes | Direction |
|---|---|---|
| Axis 1 (Main Road) | Lane 1 (Top) + Lane 4 (Bottom) | North-South |
| Axis 2 (Side Road) | Lane 2 (Left) + Lane 3 (Right) | East-West |

Both lanes in an active axis turn green simultaneously. The axis takes the **worst-case density** of its two lanes: if Lane 1 is NORMAL and Lane 4 is CROWDED, the entire Axis 1 runs for 30 s (CROWDED) to ensure the heavy lane clears.

---

## Smart Interrupt (Core Intelligence)

If Axis 1 holds the green light but all its lanes are empty (no vehicles detected), and a vehicle suddenly arrives at Axis 2 (currently red), the background sensor scan instantly detects it. Instead of forcing Axis 2 to wait for the full 30 s timer, the firmware **immediately sets `currentDuration = 0`**, triggering an instant yellow transition and handing the green to Axis 2.

This eliminates dead time at urban intersections without any manual intervention.

---

## Emergency Override

Pressing the hardware push button (Pin 12, INPUT_PULLUP) triggers a 50 ms debounce check. Once confirmed:

1. All 12 traffic LEDs snap to RED instantly.
2. The normal state machine is suspended.
3. The console LED (Pin 13) switches from solid to 200 ms rapid flash.
4. The LCD displays:
   ```
   !!! EMERGENCY !!!
   ALL LANES HALTED
   WAITING TO RESUME...
   PRESS BTN TO CANCEL
   ```
5. The system holds in this state indefinitely (ambulance, VIP convoy passage).
6. Pressing the button again resumes autonomous operation from where it left off.

---

## Non-Blocking Architecture

The firmware contains zero `delay()` calls in the main loop. All timing uses `millis()` comparisons so the Arduino can run five concurrent tasks simultaneously:

| Task | Interval | Function |
|---|---|---|
| Sensor scan | 300 ms | `scanAllLanes()` |
| State machine | Every loop tick | `manageTrafficLogic()` |
| LCD refresh | 250 ms | `updateLCD()` |
| Emergency check | Every loop tick | `checkEmergencyButton()` |
| Emergency LED flash | 200 ms | `handleEmergencyMode()` |

---

## Hardware

| Component | Model / Spec | Qty |
|---|---|---|
| Microcontroller | Arduino Mega 2560 | 1 |
| Distance Sensors | HC-SR04 Ultrasonic | 8 |
| Traffic LEDs | 5 mm Red / Yellow / Green | 12 |
| LCD Display | 20x4 I2C, PCF8574 backpack, address 0x27 | 1 |
| Emergency Button | Tactile push button, Pin 12 (INPUT_PULLUP) | 1 |
| Console LED | Pin 13 | 1 |
| Power Supply | 12 V DC mains adapter | 1 |
| Buck Converter | DC-DC step-down 12 V to 5 V | 1 |
| Perfboard | 9 x 15 cm copper perfboard | 1 |
| Resistors | 220 ohm LED current limiting | 12 |
| Cable Trunking | PVC strips (concealed wiring channels) | - |
| Model Base | MDF board + WPC perimeter frame | 1 |
| Road Surface | Black acrylic + green acrylic verge panels | 1 set |

---

## Pin Assignments

### Ultrasonic Sensors

| Sensor | Lane / Position | Trig | Echo |
|---|---|---|---|
| L1_Sens1 | Lane 1 Front (Axis 1 - Top) | 23 | 22 |
| L1_Sens2 | Lane 1 Back (Axis 1 - Top) | 24 | 25 |
| L4_Sens1 | Lane 4 Front (Axis 1 - Bottom) | 28 | 29 |
| L4_Sens2 | Lane 4 Back (Axis 1 - Bottom) | 30 | 31 |
| L2_Sens1 | Lane 2 Front (Axis 2 - Left) | 36 | 37 |
| L2_Sens2 | Lane 2 Back (Axis 2 - Left) | 34 | 35 |
| L3_Sens1 | Lane 3 Front (Axis 2 - Right) | 32 | 33 |
| L3_Sens2 | Lane 3 Back (Axis 2 - Right) | 26 | 27 |

### Traffic LED Outputs

| Pin | Signal | Lane |
|---|---|---|
| 9 | L1_R | Lane 1 (Top) Red |
| 10 | L1_Y | Lane 1 (Top) Yellow |
| 11 | L1_G | Lane 1 (Top) Green |
| 16 | L4_R | Lane 4 (Bottom) Red |
| 14 | L4_Y | Lane 4 (Bottom) Yellow |
| 15 | L4_G | Lane 4 (Bottom) Green |
| 6 | L2_R | Lane 2 (Left) Red |
| 8 | L2_Y | Lane 2 (Left) Yellow |
| 7 | L2_G | Lane 2 (Left) Green |
| 3 | L3_R | Lane 3 (Right) Red |
| 4 | L3_Y | Lane 3 (Right) Yellow |
| 5 | L3_G | Lane 3 (Right) Green |
| 12 | buttonPin | Emergency override (INPUT_PULLUP) |
| 13 | consoleLED | System status LED |
| SDA / SCL | I2C | 20x4 LCD via PCF8574 at 0x27 |

---

## Key Timing Constants

| Constant | Value | Purpose |
|---|---|---|
| `TIME_NORMAL` | 15 000 ms | Green duration for NORMAL density |
| `TIME_CROWDED` | 30 000 ms | Green duration for CROWDED density |
| `TIME_EMPTY_OSCILLATE` | 30 000 ms | Resting green when no vehicles present |
| `TIME_YELLOW` | 5 000 ms | Yellow transition before every red switch |
| Sensor scan interval | 300 ms | Background ultrasonic polling |
| LCD refresh interval | 250 ms | Display update rate |
| Emergency debounce | 50 ms | Button press verification window |
| Emergency LED flash | 200 ms | Console LED flash rate in override mode |

---

## Libraries Required

Install via Arduino Library Manager:

- **HCSR04** by Teckel (`UltraSonicDistanceSensor` class)
- **LiquidCrystal I2C** by Frank de Brabander

---

## Quick Start

1. Open `arduino-firmware/code_main/code_main.ino` in Arduino IDE
2. Install both libraries above via **Sketch > Include Library > Manage Libraries**
3. Select board: **Arduino Mega or Mega 2560**
4. Select the correct COM port
5. Upload the sketch
6. On power-up: 5 s splash screen then autonomous adaptive mode begins
7. To test Smart Interrupt: place a car on Axis 2 while Axis 1 is green and empty
8. To test Emergency Override: press Pin 12 button once (again to resume)

### Running Reference Sketches First

Before uploading the full firmware, verify each hardware section works in isolation:

```
arduino-firmware/reference-sketches/LCD2004-test/   -> verify I2C LCD address and display
arduino-firmware/reference-sketches/LED-test/        -> verify all 12 LED pins
arduino-firmware/reference-sketches/Ultrasonic/      -> verify a single HC-SR04 reading
```

---

## Demonstration Script (for Evaluation Panel)

| Step | Action | Expected Result |
|---|---|---|
| 1 | Power on with no cars | LCD shows splash, then all RED, then Axis 1 GREEN with EMPTY / 30 s |
| 2 | Do nothing | System oscillates between Axis 1 and Axis 2 with EMPTY label |
| 3 | Place 1 car at Lane 1 front sensor only | LCD: TRAFFIC: NORMAL, timer: 15 s |
| 4 | Block both sensors on a lane | LCD: TRAFFIC: CROWDED, timer: 30 s |
| 5 | While Axis 1 is green and empty, place car on Axis 2 | Axis 1 instantly terminates, goes yellow, Axis 2 gets green |
| 6 | Press emergency button during any phase | All RED, consoleLED flashes, LCD shows emergency message |
| 7 | Press emergency button again | System resumes autonomous operation |

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
  circuit-diagram-colour.pdf   # Colour Proteus schematic: full system wiring
  circuit-diagram-bw.pdf       # Monochrome Proteus schematic
  preview/
    circuit-diagram-colour.webp
    circuit-diagram-bw.webp

docs/
  MASTERPLAN.md                # Full project knowledge base, design rationale,
                               # operating instructions, academic context

README.md                      # This file
```

---

## Academic Context

This project demonstrates that **rule-based adaptive control significantly outperforms fixed-cycle timing** in urban intersections. The three-tier density model (EMPTY / NORMAL / CROWDED) was chosen over full fuzzy logic to maintain deterministic, real-time response on a resource-constrained microcontroller while preserving the core adaptive behaviour of fuzzy systems.

Key engineering outcomes:
- Elimination of idle green phases via Smart Interrupt
- Worst-case axis prioritisation ensures the heaviest lane always gets adequate clearance time
- Complete non-blocking architecture allows simultaneous sensor scanning, display update, and emergency response with zero missed events
- Hardware debouncing prevents false emergency triggers from electrical noise

---

## Portfolio and Full Documentation

For the complete build story, 134 build photographs, circuit diagrams, firmware walkthrough, and state machine documentation:

**https://damilareadekeye.com/works/embedded-systems/adaptive-traffic-signal/**
