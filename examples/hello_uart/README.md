# 🌉 Interrupt-Driven UART Bridge

![RP2040](https://img.shields.io/badge/MCU-RP2040-blue) ![Language](https://img.shields.io/badge/Language-C-blue)

This project is more than a simple "Hello, UART" example. It implements a full-duplex, interrupt-driven UART bridge on the Raspberry Pi Pico. It efficiently passes data between two separate UART peripherals without blocking the main CPU.

## 📝 Description

This program turns a Raspberry Pi Pico into a smart serial adapter. It initializes two UARTs (`uart0` and `uart1`) and sets up a bidirectional communication channel between them.

- **Interrupt-Driven:** The core of this project is its use of interrupts. Instead of constantly polling for new data (which wastes CPU cycles), the Pico's processor is only alerted when a character arrives on either UART. This is a highly efficient way to handle serial communication.

- **Low Latency:** By disabling the UART FIFOs, the interrupt is triggered for every single character received. This minimizes latency, making the bridge suitable for applications that require a quick response.

- **Use Cases:**
    - Connecting a computer (via USB-to-serial on `uart0`) to a peripheral device (like an RS485 module on `uart1`).
    - Snooping on communication between two other devices.
    - Protocol translation (by adding logic inside the interrupt handlers).

## 🛠️ Hardware & Software Requirements

### Hardware
- Raspberry Pi Pico or any RP2040-based board.
- Two serial devices to connect to the Pico. For example:
    - A USB-to-serial adapter for your computer.
    - An RS485 transceiver, another microcontroller, or a GPS module.

### Software
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [CMake](https://cmake.org/)
- [ARM GCC Compiler](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)

## ⚙️ Pinout

This example sets up two UART instances:

**UART 0 (`INTEL_N100` in code):**
| Function | Pin (GPIO) | Description                     |
|----------|------------|---------------------------------|
|  TX      | 1          | Transmit to Host (e.g., PC)     |
|  RX      | 0          | Receive from Host (e.g., PC)    |

**UART 1 (`RS485` in code):**
| Function | Pin (GPIO) | Description                         |
|----------|------------|-------------------------------------|
|  TX      | 20         | Transmit to Peripheral (e.g., RS485)|
|  RX      | 21         | Receive from Peripheral (e.g., RS485)|

## 🚀 How to Build and Run

1.  **Build and Flash:**
    - Compile the C code in the `examples/hello_uart` directory and flash the `.uf2` file to your Pico.

2.  **Connect your devices:**
    - Connect your first serial device to UART0 (GPIO 0, 1).
    - Connect your second serial device to UART1 (GPIO 20, 21).
    - **Important:** Ensure you have a common ground (GND) between all devices.

3.  **Test the Bridge:**
    - Open a serial terminal for each of the two devices (e.g., two `minicom` windows or two Arduino Serial Monitors).
    - Set the baud rate to **115200**.
    - Anything you type in one terminal should appear in the other, and vice-versa!

---

This project is a great example of the power and efficiency of interrupt-based design in embedded systems. ⚡️
