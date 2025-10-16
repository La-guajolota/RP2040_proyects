# 📡 2D LiDAR Scanner with TF-Luna and Servo

![RP2040](https://img.shields.io/badge/MCU-RP2040-red) ![Language](https://img.shields.io/badge/Language-C-blue) ![Python](https://img.shields.io/badge/Python-3-yellow)

This project transforms a simple, single-point LiDAR sensor into a full 2D scanning LiDAR system using a servo motor and a Raspberry Pi Pico. It includes a real-time graphical interface to visualize the scanned environment.

## 📝 Description

This project combines three main components:

1.  **TF-Luna LiDAR Sensor:** A compact, low-cost LiDAR sensor that provides distance measurements. It is read via I2C.
2.  **SG90 Servo Motor:** A standard hobby servo that sweeps the LiDAR sensor back and forth from 0 to 180 degrees.
3.  **Raspberry Pi Pico:** The brain of the operation. The Pico orchestrates the hardware, reads the sensor data, controls the servo, and sends the data to a host computer.

The system uses an interrupt connected to the TF-Luna's "data ready" pin. This allows the Pico to efficiently capture a new distance reading as soon as it's available. The Pico then prints the current servo angle and the measured distance to the serial console in a `angle:distance` format.

## 🖥️ Real-Time Radar UI

A Python script (`ui/radar.py`) provides a live, graphical representation of the LiDAR data. It reads the serial data from the Pico and plots the points on a polar grid, creating a radar-like display of the surrounding environment.

![Radar UI](https://i.imgur.com/5gY2XJd.png)  *(Example image of a similar radar UI)*

## 🛠️ Hardware & Software Requirements

### Hardware
- Raspberry Pi Pico or any RP2040-based board
- [TF-Luna LiDAR Sensor](https://www.benewake.com/en/tf-luna.html)
- [SG90 Servo Motor](https://www.towerpro.com.tw/product/sg90-7/)
- A 3D printed mount or other mechanism to attach the LiDAR to the servo is recommended.
- Jumper wires

### Software
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
- [CMake](https://cmake.org/)
- [ARM GCC Compiler](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
- [Python 3](https://www.python.org/) with `tkinter`, and `pyserial` libraries.

## ⚙️ Pinout

| Component       | Function      | Pin (GPIO) | Connection      |
|-----------------|---------------|------------|-----------------|
| サーボ (Servo)    | PWM Control   | 0          | SG90 Signal Pin |
| 🛰️ TF-Luna      | I2C SDA       | 4          | TF-Luna SDA Pin |
|                 | I2C SCL       | 5          | TF-Luna SCL Pin |
|                 | Data Ready IRQ| 15         | TF-Luna MUX Out |
|                 | VCC           | 5V         | TF-Luna 5V Pin  |
|                 | GND           | GND        | TF-Luna GND Pin |

## 🚀 How to Build and Run

1.  **Build and Flash:**
    - Compile the C code in the `Robotics/LiDAR_TFluna` directory and flash the `.uf2` file to your Pico.

2.  **Run the Python UI:**
    - Connect the Pico to your computer via USB.
    - Navigate to the `ui` directory.
    - Modify the `serial_port` variable in `radar.py` to match your Pico's serial port.
    - Run the script:
      ```bash
      python radar.py
      ```

3.  **Observe:**
    - The servo will start to scan, and the Python UI will display the LiDAR data in real-time!

---

This project is a fantastic introduction to robotics, sensor fusion, and data visualization. 🤖
