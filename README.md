# Adaptive Traffic Signal Control System
### Real-Time Optimization of Vehicular Flow at Urban Intersections

![Arduino](https://img.shields.io/badge/Platform-Arduino%20Mega%202560-00979D?style=flat-square&logo=arduino&logoColor=white)
![Language](https://img.shields.io/badge/Language-C%2B%2B%20%2F%20Arduino-blue?style=flat-square)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen?style=flat-square)
![Portfolio](https://img.shields.io/badge/Portfolio-Damilare%20Lekan%20Adekeye-orange?style=flat-square)

> **Full build documentation, 134 build photographs, circuit diagrams, state machine walkthrough, and firmware deep-dive:**
> ### [damilareadekeye.com/works/embedded-systems/adaptive-traffic-signal](https://damilareadekeye.com/works/embedded-systems/adaptive-traffic-signal/)

---

## Overview

Traditional fixed-cycle traffic lights waste time on empty roads and create unnecessary congestion on busy approaches. This project replaces that rigid pattern with a **real-time, rule-based adaptive controller** built on the Arduino Mega 2560.

Two HC-SR04 ultrasonic sensors per lane — one near the stop-line (front) and one further back — measure vehicle queue length continuously across all four lanes. Each lane is classified as **EMPTY**, **NORMAL**, or **CROWDED**. Green-light duration is allocated dynamically based on actual traffic density, so no lane idles unnecessarily while vehicles wait on the opposing axis.

A **Smart Interrupt** feature instantly collapses an active green phase the moment the current axis empties while the opposing axis detects waiting vehicles, eliminating dead time entirely. A **hardware Emergency Override** button halts all traffic instantly for ambulances, fire trucks, or VIP convoys.

| Density State | Green Duration | Trigger Condition |
|:---|:---:|:---|
| `EMPTY` | 30 s (resting) | Neither front nor back sensor detects a vehicle |
| `NORMAL` | 15 s | Front sensor occupied, back sensor clear (short queue) |
| `CROWDED` | 30 s | Both sensors occupied (heavy congestion stretching back) |

---

## How It Works

```
                          ┌─ POWER ON ─────────────────────────┐
                          │  All lanes → RED (safety state)     │
                          │  LCD splash screen (5 s)            │
                          └────────────────┬────────────────────┘
                                           │
                          ┌────────────────▼────────────────────┐
                          │      AUTONOMOUS ADAPTIVE MODE        │
                          │                                      │
                          │  Background scan every 300 ms:      │
                          │  HC-SR04 ×8 → getLaneDensity()      │
                          │  axis1 = max(Lane1, Lane4)          │
                          │  axis2 = max(Lane2, Lane3)          │
                          └────────────────┬────────────────────┘
                                           │
             ┌─────────────────────────────▼──────────────────────────────┐
             │                    6-STATE MACHINE                         │
             │                                                             │
             │   AXIS1_EVAL ──► AXIS1_GREEN ──────────────────────────►  │
             │       ▲               │ Smart Interrupt:                   │
             │       │               │ axis1 EMPTY + axis2 waiting?       │
             │       │               │ → currentDuration = 0              │
             │       │               ▼                                    │
             │       │          AXIS1_YELLOW (5 s)                        │
             │       │               │                                    │
             │       │               ▼                                    │
             │       │          AXIS2_EVAL ──► AXIS2_GREEN ──────────►   │
             │       │                              │ Smart Interrupt     │
             │       │                              ▼                    │
             │       │                         AXIS2_YELLOW (5 s)        │
             │       └──────────────────────────────┘                    │
             └─────────────────────────────────────────────────────────────┘
                           │
             ┌─────────────▼──────────────────────────────────────────────┐
             │  LCD refresh every 250 ms: axis / density / mode / timer   │
             │  Emergency check every tick: 50 ms debounce on Pin 12      │
             │  → All RED + consoleLED flash + LCD emergency message       │
             └─────────────────────────────────────────────────────────────┘
```

---

## The Axis System

The 4-lane cross-intersection is paired into two opposing axes, mirroring real-world civil engineering standards:

```
                          Lane 1 (Top)
                              │
              Lane 2 ─────────┼───────── Lane 3
             (Left)           │          (Right)
                          Lane 4 (Bottom)

         ◄──── Axis 1 ────►       ◄──── Axis 2 ────►
         Lane 1 + Lane 4           Lane 2 + Lane 3
         (North–South)             (East–West)
```

Both lanes on the active axis turn green simultaneously. The axis adopts the **worst-case density** of its two lanes: if Lane 1 is NORMAL but Lane 4 is CROWDED, the entire Axis 1 gets 30 s to ensure the heavier lane clears.

---

## Key Features

- **Real-time density classification** — dual sensor per lane determines EMPTY / NORMAL / CROWDED every 300 ms
- **Smart Interrupt** — instantly collapses idle green phase when opposing axis detects waiting vehicles
- **Worst-case axis prioritisation** — heaviest lane always drives the timing decision
- **Non-blocking architecture** — zero `delay()` calls; all timing via `millis()` so five tasks run concurrently
- **Safety yellow transition** — mandatory 5 s yellow before every green-to-red switch
- **Hardware Emergency Override** — debounced push button forces all lanes to RED instantly
- **20×4 I2C LCD dashboard** — live axis, density label, phase mode, and countdown timer
- **Startup splash screen** — all lanes RED with 5 s credential display before autonomous mode

---

## Non-Blocking Concurrent Tasks

| Task | Interval | Function Called |
|:---|:---:|:---|
| Ultrasonic sensor scan | 300 ms | `scanAllLanes()` |
| Traffic state machine | Every loop tick | `manageTrafficLogic()` |
| LCD display refresh | 250 ms | `updateLCD()` |
| Emergency button check | Every loop tick | `checkEmergencyButton()` |
| Emergency LED flash | 200 ms | `handleEmergencyMode()` |

---

## Emergency Override Behaviour

When Pin 12 is pressed (INPUT_PULLUP, active LOW) and the 50 ms debounce confirms the press:

1. All 12 traffic LEDs immediately snap to **RED**
2. The state machine suspends
3. Console LED (Pin 13) switches from solid to **200 ms rapid flash**
4. LCD displays:
   ```
   !!! EMERGENCY !!!
   ALL LANES HALTED
   WAITING TO RESUME...
   PRESS BTN TO CANCEL
   ```
5. System holds indefinitely until the button is pressed again to resume

---

## Hardware Bill of Materials

| Component | Model / Specification | Qty |
|:---|:---|:---:|
| Microcontroller | Arduino Mega 2560 | 1 |
| Distance Sensors | HC-SR04 Ultrasonic (Trig + Echo) | 8 |
| Traffic Signal LEDs | 5 mm — Red, Yellow, Green | 12 |
| LCD Display | 20×4 I2C with PCF8574 backpack (address 0x27) | 1 |
| Emergency Button | Tactile push button — Pin 12, INPUT_PULLUP | 1 |
| Console Status LED | Pin 13 | 1 |
| Power Supply | 12 V DC mains adapter | 1 |
| Buck Converter | DC-DC step-down module (12 V → 5 V) | 1 |
| Prototyping PCB | 9×15 cm copper perfboard | 1 |
| Current-Limiting Resistors | 220 Ω | 12 |
| Cable Trunking | PVC strips for concealed wiring channels | — |
| Model Base | MDF board with WPC perimeter frame | 1 |
| Road Surface | Black acrylic + green acrylic verge panels | 1 set |

---

## Pin Assignments

### Ultrasonic Sensor Wiring

| Identifier | Lane / Position | Trig Pin | Echo Pin |
|:---|:---|:---:|:---:|
| L1_Sens1 | Lane 1 — Front (Axis 1, Top) | 23 | 22 |
| L1_Sens2 | Lane 1 — Back (Axis 1, Top) | 24 | 25 |
| L4_Sens1 | Lane 4 — Front (Axis 1, Bottom) | 28 | 29 |
| L4_Sens2 | Lane 4 — Back (Axis 1, Bottom) | 30 | 31 |
| L2_Sens1 | Lane 2 — Front (Axis 2, Left) | 36 | 37 |
| L2_Sens2 | Lane 2 — Back (Axis 2, Left) | 34 | 35 |
| L3_Sens1 | Lane 3 — Front (Axis 2, Right) | 32 | 33 |
| L3_Sens2 | Lane 3 — Back (Axis 2, Right) | 26 | 27 |

### Traffic LED and Control Pins

| Pin | Signal | Description |
|:---:|:---|:---|
| 9 | L1_R | Lane 1 (Top) — Red |
| 10 | L1_Y | Lane 1 (Top) — Yellow |
| 11 | L1_G | Lane 1 (Top) — Green |
| 16 | L4_R | Lane 4 (Bottom) — Red |
| 14 | L4_Y | Lane 4 (Bottom) — Yellow |
| 15 | L4_G | Lane 4 (Bottom) — Green |
| 6 | L2_R | Lane 2 (Left) — Red |
| 8 | L2_Y | Lane 2 (Left) — Yellow |
| 7 | L2_G | Lane 2 (Left) — Green |
| 3 | L3_R | Lane 3 (Right) — Red |
| 4 | L3_Y | Lane 3 (Right) — Yellow |
| 5 | L3_G | Lane 3 (Right) — Green |
| 12 | buttonPin | Emergency override button (INPUT_PULLUP, active LOW) |
| 13 | consoleLED | System status — solid in auto mode, flash in emergency |
| SDA / SCL | I2C bus | 20×4 LCD via PCF8574 backpack at address 0x27 |

---

## Timing Constants

| Constant | Value | Purpose |
|:---|:---:|:---|
| `TIME_NORMAL` | 15 000 ms | Green duration — NORMAL traffic density |
| `TIME_CROWDED` | 30 000 ms | Green duration — CROWDED traffic density |
| `TIME_EMPTY_OSCILLATE` | 30 000 ms | Resting green oscillation when no vehicles present |
| `TIME_YELLOW` | 5 000 ms | Yellow safety transition before every axis switch |
| Sensor scan interval | 300 ms | Background ultrasonic polling rate |
| LCD refresh interval | 250 ms | Display update rate |
| Emergency debounce | 50 ms | Button press verification window |
| Emergency LED flash | 200 ms | Console LED flash period during override |

---

## Libraries

Install both via **Arduino IDE → Sketch → Include Library → Manage Libraries**:

| Library | Author | Used For |
|:---|:---|:---|
| HCSR04 | Teckel | `UltraSonicDistanceSensor` abstraction for all 8 sensors |
| LiquidCrystal I2C | Frank de Brabander | 20×4 LCD control over I2C |

---

## Repository Structure

```
Adaptive-Traffic-Signal/
│
├── arduino-firmware/                      # All Arduino sketches
│   │
│   ├── code_main/                         # Production firmware
│   │   └── code_main.ino                  # Full adaptive traffic controller
│   │
│   └── reference-sketches/               # Standalone hardware verification tests
│       ├── LCD2004-test/
│       │   └── LCD2004-test.ino           # Scan I2C address, test LCD display output
│       ├── LED-test/
│       │   └── LED-test.ino               # Cycle through all 12 LED pins individually
│       └── Ultrasonic/
│           └── Ultrasonic.ino             # Read and print HC-SR04 distance over serial
│
├── circuit-diagrams/                      # Proteus schematics
│   ├── circuit-diagram-colour.pdf         # Full-colour wiring diagram (print ready)
│   ├── circuit-diagram-bw.pdf             # Monochrome version (clearer trace routing)
│   └── preview/                           # Web-optimised previews
│       ├── circuit-diagram-colour.webp
│       └── circuit-diagram-bw.webp
│
├── docs/
│   └── MASTERPLAN.md                      # Full knowledge base: design rationale,
│                                          # operating instructions, demo guide,
│                                          # academic context, and system logic
│
└── README.md                              # This file
```

---

## Quick Start

```bash
# 1. Clone the repository
git clone https://github.com/damilareadekeye/Adaptive-Traffic-Signal-Control-for-Real-Time-Vehicular-Flow-Optimization-at-Urban-Intersections.git

# 2. Open in Arduino IDE
#    File → Open → arduino-firmware/code_main/code_main.ino

# 3. Install libraries
#    Sketch → Include Library → Manage Libraries
#    Search and install: "HCSR04" and "LiquidCrystal I2C"

# 4. Select board and port
#    Tools → Board → Arduino Mega or Mega 2560
#    Tools → Port → (your COM port)

# 5. Upload and power on
#    5 s splash screen → autonomous adaptive mode begins
```

> **Tip — verify hardware first:** Run each sketch in `reference-sketches/` before flashing the full firmware to confirm your I2C address, all LED pins, and sensor readings are correct.

---

## Demonstration Guide

| Step | Action | What to Observe |
|:---:|:---|:---|
| 1 | Power on, no cars on track | LCD splash (5 s), all RED, Axis 1 turns GREEN — `TRAFFIC: EMPTY  30 s` |
| 2 | Do nothing | System oscillates Axis 1 → Axis 2 → Axis 1, LCD updates each cycle |
| 3 | Place one car at front sensor of any lane | LCD updates to `TRAFFIC: NORMAL  15 s` on next EVAL |
| 4 | Block both sensors on the same lane | LCD updates to `TRAFFIC: CROWDED  30 s` |
| 5 | While Axis 1 is green and empty, place a car on Axis 2 | Axis 1 collapses instantly to yellow — Smart Interrupt in action |
| 6 | Press emergency button during any phase | All RED, consoleLED flashes rapidly, LCD shows emergency screen |
| 7 | Press emergency button a second time | System resumes autonomous operation seamlessly |

---

## Academic Context

This project demonstrates that **deterministic, rule-based adaptive control outperforms fixed-cycle timing** in real urban intersections. The three-tier density model was chosen over full fuzzy-logic computation to preserve real-time determinism on a resource-constrained microcontroller while retaining the core adaptive behaviour of fuzzy systems.

Key engineering outcomes:

- **Smart Interrupt** eliminates idle green phases — no axis holds green on an empty road while vehicles wait elsewhere
- **Worst-case prioritisation** guarantees the heaviest lane always receives sufficient clearance time
- **Non-blocking architecture** achieves five concurrent tasks with zero missed sensor events or button presses
- **Hardware debouncing** prevents false emergency triggers from switch noise or bounce

---

## Full Documentation

Complete build story, 134 photographs, Proteus circuit diagrams, firmware deep-dive, and state machine documentation:

**[damilareadekeye.com/works/embedded-systems/adaptive-traffic-signal](https://damilareadekeye.com/works/embedded-systems/adaptive-traffic-signal/)**
