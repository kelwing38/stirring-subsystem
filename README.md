# Closed-Loop Bioreactor Stirring Control Subsystem

An embedded C/C++ control subsystem designed to regulate stirring speed (RPM) in a mini-bioreactor using an Arduino/ESP32, Hall effect sensor telemetry, and closed-loop PID control.

---

## System Overview

Maintaining uniform and controlled mixing of the liquid inside a bioreactor is critical to maintain a homogenous environment for cell cultivation. Thus, the stirring subsystem must be able to maintain exact rotational speeds.

This subsystem monitors motor rotational velocity via hardware interrupts from a digital Hall effect sensor and computes dynamic PWM corrections through a tuned PI controller to maintain target RPM under variable fluid viscosity and load conditions. 

---

## Technical Features

* **Hardware Interrupt-Driven Sensing:** Implements a non-blocking ISR (`pulseCounter()`) triggered on rising edges to capture high-frequency pulses from a Hall sensor (70 Pulses Per Revolution).
* **Race Condition Prevention:** Uses atomic read-and-reset blocks (`noInterrupts()` / `interrupts()`) around shared volatile pulse counters to eliminate multi-threaded/interrupt race conditions during loop execution.
* **Closed-Loop PI Tuning:** Employs a Proportional-Integral controller ($K_p = 0.1$, $K_i = 0.1$) with a 50 ms sample interval to eliminate steady-state error. Derivative gain ($K_d$) is deliberately omitted to prevent control signal saturation caused by discrete Hall sensor measurement jitter.
* **PWM Actuation:** Automatically maps computed output to an 8-bit PWM duty cycle (`0–255`) driving a power transistor/motor driver.
* **Real-Time Telemetry:** Streams target RPM, current RPM, and active PWM duty cycle over UART serial at 9600 baud for diagnostics and monitoring.

---

## Hardware & Pinout

| Component | Pin / Interface | Description |
| :--- | :--- | :--- |
| **PWM Actuator** | `Pin 9` | 8-bit PWM output to DC motor driver |
| **Hall Effect Sensor** | `Pin 4` (Interrupt) | Tachometer pulse input (`INPUT_PULLUP`) |
| **Telemetry** | `UART (Serial)` | Real-time CSV-like monitoring over USB |

* **Pulses Per Revolution (PPR):** 70.0
* **Sampling Window:** 50 ms (`LOOP_DELAY_MS`)

---

## Control Loop Mathematics

Rotational velocity is calculated across discrete time windows ($\Delta t$):

$$\text{RPM} = \left( \frac{\text{pulses}}{\text{PPR}} \right) \times \left( \frac{60000}{\Delta t_{\text{ms}}} \right)$$

---

## Dependencies & Setup

1. **Board Support:** Arduino AVR or ESP32 board definitions in Arduino IDE / PlatformIO.
2. **Libraries:**
   * `PID_v1` by Brett Beauregard (install via Arduino Library Manager).
3. **Flashing:**
   * Open `src/stirring_control.ino`.
   * Select your target board and COM port.
   * Compile, flash, and open the Serial Monitor at `9600` baud.
