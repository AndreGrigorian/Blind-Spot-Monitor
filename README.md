# Blind Spot Monitor using TM4C123G LaunchPad

> A model-scale automotive blind spot monitoring system built with the **Texas Instruments Tiva™ TM4C123G LaunchPad**, dual ultrasonic sensors, LEDs, and an active buzzer.

[![Demo Video](https://img.shields.io/badge/Watch-Demo-red?style=for-the-badge&logo=youtube)](https://youtu.be/9JBMQK3UJxU)

---

## Overview

This project simulates the functionality of a modern vehicle's blind spot monitoring system using a LEGO vehicle platform. Two HC-SR04 ultrasonic sensors continuously monitor the left and right sides of the vehicle, providing visual and audible warnings based on the proximity of nearby objects.

The project was developed as the final project for **ECE 425 – Microprocessor Systems**.

---

## Features

- Dual ultrasonic blind spot detection
- Three-stage warning system
  - GREEN = Safe
  - YELLOW = Caution
  - RED = Danger
- Distance-dependent buzzer that increases chirp rate as objects approach
- UART live debugging output
- Sequential sensor polling to reduce ultrasonic crosstalk
- Distance calibration for improved real-world accuracy

---

## Hardware

- Texas Instruments TM4C123G LaunchPad
- 2× HC-SR04 Ultrasonic Sensors
- 6× LEDs (2 Red, 2 Yellow, 2 Green)
- Active Buzzer (TMB12A05)
- Breadboard & Jumper Wires
- LEGO Vehicle Platform

---

## System Overview

The TM4C123G periodically triggers each ultrasonic sensor and measures the echo pulse width to estimate object distance.

Detected objects are classified into three warning zones:

| Distance State | Indicator |
|---------------|-----------|
| Safe | Green LED |
| Caution | Yellow LED |
| Danger | Red LED + Variable-Speed Buzzer |

To avoid ultrasonic interference, the sensors are sampled sequentially rather than simultaneously.

---

## Software Highlights

- GPIO control
- SysTick timing
- UART debugging
- Ultrasonic sensor interfacing
- Distance calibration
- State-based warning logic

A calibration factor was applied to compensate for timing overhead introduced by software-based pulse measurement, resulting in significantly more accurate distance readings.

---

## UART Debug Output

The firmware outputs live diagnostic information over UART, including:

- Left and right measured distances
- Simple top-down visualization of nearby objects
- Real-time debugging during testing and calibration

---

## Demo

**Video Demonstration**

https://youtu.be/9JBMQK3UJxU

---

## Images

### Complete System

![Complete System](images/system.jpg)

### Hardware Wiring

![Hardware](images/hardware.jpg)

### Block Diagram

![Block Diagram](images/block_diagram.png)

### Prototype in Operation

![Demo](images/demo.jpg)


---

## Repository Structure

```
Blind-Spot-Monitor/
│
├── src/                 # Source code
├── images/              # Project photos
├── README.md
└── Report.pdf
```

---

## Future Improvements

- Hardware timer input capture for higher timing precision
- Additional sensors for 360° obstacle detection
- OLED/LCD status display
- Adaptive warning thresholds
- Wireless logging and telemetry

---

## Skills Demonstrated

- Embedded Systems
- C Programming
- Microcontrollers (ARM Cortex-M4)
- GPIO Programming
- UART Communication
- Sensor Interfacing
- Real-Time Embedded Software
- System Calibration
- Debugging & Testing
- Hardware Integration

---

## Author

**Andre Grigorian**

ECE 425 – Microprocessor Systems  
California State University, Northridge

GitHub: https://github.com/AndreGrigorian

