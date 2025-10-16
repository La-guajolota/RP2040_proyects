# 🌊 BPSK Carrier Signal Generator

![RP2040](https://img.shields.io/badge/MCU-RP2040-9cf) ![Language](https://img.shields.io/badge/Language-C-blue)

This project demonstrates how to generate the two essential carrier signals for Binary Phase Shift Keying (BPSK) modulation using the Raspberry Pi Pico's powerful PWM hardware.

## 📝 Description

Phase Shift Keying (PSK) is a digital modulation scheme that conveys data by changing, or modulating, the phase of a carrier signal. The simplest form is BPSK, which uses two phases separated by 180 degrees to represent the two binary digits, 0 and 1.

This program doesn't perform the actual data modulation, but it completes the crucial first step: **generating the two carrier signals.** It sets up two PWM channels to produce square waves that are:
- At the exact same frequency.
- Perfectly 180 degrees out of phase with each other.

This is achieved by configuring one PWM channel with normal polarity (active high) and the other with inverted polarity (active low).

## 🛠️ Hardware & Software Requirements

### Hardware
- Raspberry Pi Pico or any RP2040-based board
- An oscilloscope to visualize the signals

### Software
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [CMake](https://cmake.org/)
- [ARM GCC Compiler](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)

## ⚙️ Pinout

| Function                 | Pin (GPIO) | Description                                      |
|--------------------------|------------|--------------------------------------------------|
| 📈 Carrier (0° Phase)    | 2          | A 1kHz square wave with normal polarity.         |
| 📉 Carrier (180° Phase)  | 4          | A 1kHz square wave with inverted polarity.       |

## 🚀 How to Build and Run

1.  **Build and Flash:**
    - Compile the C code in the `telecomms/PSK` directory and flash the `.uf2` file to your Pico.

2.  **Observe the Output:**
    - Connect one oscilloscope probe to GPIO 2 and another to GPIO 4.
    - Set the trigger on one of the channels.
    - You will see two square waves of the same frequency, with the rising edge of one aligning with the falling edge of the other, clearly showing the 180° phase difference.

![BPSK Phases](https://i.imgur.com/d9s4v2o.png)  *(Example of two signals 180° out of phase)*

## 🧩 Next Steps: Building a Full BPSK Modulator

This project provides the foundation. To build a complete BPSK modulator, you would need to add a data input and a way to switch between the two carrier signals.

1.  **Data Input:** Use another GPIO as a digital input to read a stream of 1s and 0s.
2.  **Signal Switching:** Use the data input to control a high-speed analog multiplexer (like a 74HC4053) or logic gates to select which of the two carrier signals is sent to the output.
    - If the data bit is `1`, output the 0° phase signal.
    - If the data bit is `0`, output the 180° phase signal.

This creates a final, single-wire BPSK-modulated signal ready for transmission.

---

This project is an excellent starting point for exploring the fascinating world of digital communications. 📡
