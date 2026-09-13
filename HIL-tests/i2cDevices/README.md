# 🧪 i2cDevices — I2C Sensor HIL Demo

![RP2040](https://img.shields.io/badge/MCU-RP2040-blue) ![Language](https://img.shields.io/badge/Language-C-blue)

## 📝 What this is

This is a **demo/practice project**, not a finished product. Its purpose is
to give me a concrete, real-hardware target for experimenting with a
**remote HIL (Hardware-In-the-Loop) home lab** setup: a Pico wired to a
handful of I2C sensors, sitting on a remote host, built/flashed/tested from
my dev machine over SSH — see [`../README-flash.md`](../README-flash.md)
and [`../remote-setup/`](../remote-setup/) for the plumbing that makes that
possible.

The point isn't the sensors themselves — it's exercising the full loop
(edit → build → flash remotely → read results back over USB serial →
on-target unit tests) the way I'd want it to work for future HIL setups.

## 📟 What it does

Reads three I2C/analog sensors on a single bus and prints the readings over
USB serial once a second:

- **AHT20** — temperature + relative humidity ([`src/aht20/aht20.c`](src/aht20/aht20.c))
- **SGP30** — IAQ (CO₂-equivalent + TVOC) ([`src/sgp30/sgp30.c`](src/sgp30/sgp30.c))
- **MQ135** — gas sensor, read via ADC (no dedicated driver, wired directly in [`src/main.c`](src/main.c))

It also probes for a 0x3C I2C device (e.g. an SSD1306 OLED) as a wiring
sanity check.

## ⚙️ Wiring

| Signal          | GPIO | Notes                              |
|------------------|------|-------------------------------------|
| I2C0 SDA         | 16   | AHT20 (0x38) + SGP30 (0x58) share this bus |
| I2C0 SCL         | 17   | 400 kHz, internal pull-ups enabled  |
| ADC0 (MQ135)     | 26   | Analog read, 3.3V reference         |

## 🚀 Build & test

Standard pico-sdk/CMake project — build normally, or use
[`../flash_pico.sh`](../flash_pico.sh) to flash a remote-hosted Pico over
SSH.

```bash
cmake -B build -S .
cmake --build build
```

This also builds a separate on-target test firmware,
`build/test/i2cDevices_tests.uf2` (see [`test/`](test/)), which scans the
I2C bus and exercises the AHT20/SGP30 drivers against the real sensors and
reports PASS/FAIL over USB serial. Flash it in place of the app to run it;
set `-DBUILD_TESTS=OFF` to skip building it.

## 📦 Vendored references

The drivers in [`src/`](src/)/[`include/`](include/) are **not** the
third-party libraries' source used as-is. [`externals/`](externals/) holds
the original vendor libraries (OLED, AHT20, SGP30) as a local reference
while learning each sensor's protocol; the actual code here is refactored
and rewritten from scratch for the RP2040/pico-sdk, keeping only the
firmware footprint this project needs instead of pulling in the full
vendor library (e.g. the Arduino-oriented SGP30 lib, or ESP-IDF component
structure of the AHT20 one).

`externals/` is **gitignored on purpose** — not part of this repo's
history — and will be credited/integrated properly later.
