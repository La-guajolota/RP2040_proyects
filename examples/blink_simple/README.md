# ✨ Simple LED Blink Example

![RP2040](https://img.shields.io/badge/MCU-RP2040-green) ![Language](https://img.shields.io/badge/Language-C-blue)

This is a classic "Hello, World!" project for microcontrollers. It demonstrates the basic setup for a Raspberry Pi Pico project and how to control a GPIO pin to blink the onboard LED.

## 📝 Description

This program initializes the default LED pin on a Raspberry Pi Pico as an output. It then enters an infinite loop where it repeatedly turns the LED on, waits for a short delay, and then turns the LED off, creating a blinking effect.

This project is the perfect starting point for anyone new to the Raspberry Pi Pico or embedded systems in general.

## 🛠️ Hardware & Software Requirements

### Hardware
- Raspberry Pi Pico or any RP2040-based board with an onboard LED.

### Software
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [CMake](https://cmake.org/)
- [ARM GCC Compiler](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)

## ⚙️ Pinout

| Function      | Pin (GPIO) | Description                              |
|---------------|------------|------------------------------------------|
| 💡 Onboard LED | 25         | The default LED pin on the Raspberry Pi Pico. |

## 🚀 How to Build and Run

1.  **Clone the repository:**
    ```bash
    git clone <your-repo-link>
    cd <your-repo-link>/examples/blink_simple
    ```

2.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Run CMake and Make:**
    ```bash
    cmake ..
    make
    ```

4.  **Flash the firmware:**
    - Hold the `BOOTSEL` button on your Pico and connect it to your computer.
    - Copy the `blink_simple.uf2` file from the `build` directory to the Pico's mass storage device.

5.  **Observe the result:**
    - The onboard LED on your Pico should start blinking!

---

Happy blinking! 💡
