# 🚀 RP2040 MCU Projects Collection 🚀

![Microcontroller](https://img.shields.io/badge/MCU-RP2040-E40550?style=for-the-badge&logo=raspberrypi)

Welcome to this diverse collection of projects for the Raspberry Pi Pico and other RP2040-based microcontrollers. This repository is a personal playground for exploring the world of embedded systems, from basic I/O to more complex topics in digital signal processing, robotics, and telecommunications.

## 📂 Projects Overview

This repository is organized into several categories, each containing one or more projects. Each project folder is self-contained and includes its own detailed `README.md` with instructions, pinouts, and explanations.

| Category | Project | Description | Link |
| :--- | :--- | :--- | :--- |
| **General** | `adc_uart_transmit` | Reads the ADC and sends the value over UART. A great starting point. | [Go to Project](./adc_uart_transmit/README.md) |
| **DSP** | `DSP_pract1` | A foundational DSP project for sampling and transmitting ADC data for analysis. | [Go to Project](./DSP/DSP_pract1/README.md) |
| | `signal_adq` | A block-based signal acquisition system for spectral analysis with FFT. | [Go to Project](./DSP/signal_adq/README.md) |
| **Examples** | `blink_simple` | The classic "Hello, World!" of embedded systems: blinking an LED. | [Go to Project](./examples/blink_simple/README.md) |
| | `hello_uart` | An efficient, interrupt-driven UART bridge to pass data between two serial ports. | [Go to Project](./examples/hello_uart/README.md) |
| **Robotics** | `LiDAR_TFluna` | Creates a 2D LiDAR scanner using a TF-Luna sensor and a servo, with a live UI. | [Go to Project](./Robotics/LiDAR_TFluna/README.md) |
| **Telecomms** | `digital_modulators` | Demonstrates PWM, PCM, and PAM signal generation from an analog input. | [Go to Project](./telecomms/digital_modulators/README.md) |
| | `PSK` | Generates the two 180°-shifted carrier signals required for BPSK modulation. | [Go to Project](./telecomms/PSK/README.md) |
| | `Sample_Hold` | A driver for an external Sample and Hold circuit with variable frequency control. | [Go to Project](./telecomms/Sample_Hold/README.md) |

## 🛠️ General Build Instructions

Most projects in this repository follow the standard Pico SDK build process.

1.  **Navigate to a project directory:**
    ```bash
    cd path/to/your/chosen/project
    ```

2.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Run CMake and Make:**
    - You will need to specify the path to the Pico SDK. Replace `/path/to/pico-sdk` with your actual path.
    ```bash
    cmake -D PICO_SDK_PATH=/path/to/pico-sdk ..
    make
    ```

4.  **Flash the Firmware:**
    - Put your Pico into `BOOTSEL` mode.
    - Copy the generated `.uf2` file from the `build` directory to your Pico.

## ✨ Conclusion

Feel free to explore, modify, and learn from these projects. This repository is a living document of an embedded systems journey. Enjoy the process!

---

Happy Hacking! 💻