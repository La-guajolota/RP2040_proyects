# 📡 Digital Modulation Techniques on RP2040

![RP2040](https://img.shields.io/badge/MCU-RP2040-blueviolet) ![Language](https://img.shields.io/badge/Language-C-blue)

This project is a hands-on demonstration of several fundamental digital modulation techniques, implemented on the Raspberry Pi Pico. It takes an analog signal as input and generates corresponding modulated outputs.

## 📝 Description

This program continuously samples an analog signal using the ADC and uses this value to generate the following modulations in real-time:

- **Pulse Width Modulation (PWM):** The duty cycle of a square wave is varied in proportion to the analog input signal's amplitude.
- **Pulse Code Modulation (PCM):** The analog signal is quantized into discrete levels (in this case, 8-bit) and transmitted as a serial stream of digital codes.
- **Pulse Amplitude Modulation (PAM):** The project generates a fixed-frequency, fixed-width pulse train that can serve as a carrier for PAM. *Note: True PAM would require an external circuit to vary the amplitude of these pulses based on the analog signal.*
- **Pulse Position Modulation (PPM):** The code includes a placeholder comment for PPM. *Note: A full PPM implementation, where the position of a pulse is shifted, would typically require using the Pico's Programmable I/O (PIO) and is not included in this C file.*

## 🛠️ Hardware & Software Requirements

### Hardware
- Raspberry Pi Pico or any RP2040-based board
- An analog signal source (e.g., potentiometer, function generator)
- An oscilloscope to view the PWM and PAM signals
- A computer to receive the PCM data

### Software
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [CMake](https://cmake.org/)
- [ARM GCC Compiler](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
- A serial terminal or data plotting software (e.g., `minicom`, Python script, `Saleae Logic Analyzer`) to view the PCM data.

## ⚙️ Pinout

| Function            | Pin (GPIO) | Description                                      |
|---------------------|------------|--------------------------------------------------|
| ⚡️ Analog Input     | 26         | The modulating signal (0-3.3V).                  |
| 📊 PWM Output       | 22         | The generated PWM signal.                        |
| 📈 PAM Carrier Out  | 29         | The fixed pulse train for PAM.                   |
| 💻 PCM Output (USB) | (internal) | 8-bit PCM data is sent via the USB serial connection. |
|  UART0 TX           | 0          | General purpose UART TX.                         |
|  UART1 TX           | 8          | General purpose UART TX.                         |

## 🚀 How to Build and Run

1.  **Build and Flash:**
    - Compile the C code in the `telecomms/digital_modulators` directory and flash the `.uf2` file to your Pico.

2.  **Connect your hardware:**
    - Connect your analog signal source to GPIO 26.
    - Connect an oscilloscope probe to GPIO 22 to observe the PWM signal.
    - Connect another oscilloscope probe to GPIO 29 to observe the PAM carrier.
    - Connect the Pico to your computer via USB.

## 👀 Observing the Modulations

- **PWM:**
    - On your oscilloscope, you will see a square wave on GPIO 22. As you vary the input voltage on GPIO 26, the width (duty cycle) of the pulses will change.

- **PAM Carrier:**
    - On GPIO 29, you will see a constant, high-frequency square wave with a fixed duty cycle. This serves as the pulse train for a PAM system.

- **PCM:**
    - The Pico will appear as a serial device on your computer.
    - Use a serial terminal or a custom script to read the raw byte stream. Each byte represents a single 8-bit sample of the analog input.
    - You can then plot this data to reconstruct the quantized analog waveform.

---

This project provides a clear, practical look at how analog information is encoded into different digital formats. 📶
