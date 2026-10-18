#ifndef SSD1306_H
#define SSD1306_H

#include <stdbool.h>
#include <stdint.h>
#include "hardware/i2c.h"

// SSD1306 7-bit I2C address (0x3C on essentially every 0.91"/0.96" module;
// a few boards strap it to 0x3D instead).
#define SSD1306_ADDR 0x3C

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32
#define SSD1306_FRAMEBUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

/**
 * @brief Initialize the SSD1306 driver with the given I2C port and turn the
 *        display on, cleared.
 *
 * Call this once, after the port itself has been set up with i2c_init() +
 * gpio_set_function()/gpio_pull_up() on the SDA/SCL pins (same pattern as
 * AHT20_init()/sgp30_init()), and before any other ssd1306_*() call.
 *
 * @param i2c_port I2C instance (e.g. i2c0/i2c1) the display is on.
 * @return true on success, false if the init command sequence failed to
 *         transfer over I2C (wrong address, no ACK, bus not initialized).
 */
bool ssd1306_init(i2c_inst_t *i2c_port);

/**
 * @brief Blank the in-memory framebuffer and reset the text cursor to the
 *        top-left corner. Does not touch the display until ssd1306_flush()
 *        (or the next ssd1306_print()/ssd1306_printf()) is called.
 */
void ssd1306_clear(void);

/**
 * @brief Push the in-memory framebuffer to the display over I2C.
 *
 * @return true on success, false on an I2C transfer failure.
 */
bool ssd1306_flush(void);

/**
 * @brief Print a string to the display like you would to a serial console:
 *        characters advance the cursor left to right, '\n' starts a new
 *        line, and once the last line is full the whole screen scrolls up
 *        one line to make room. Flushes to the display before returning.
 *
 * @param str the text to print (plain ASCII; anything outside the printable
 *            range 0x20-0x7E is rendered as '?').
 */
void ssd1306_print(const char *str);

/**
 * @brief printf()-style version of ssd1306_print() - formats into an
 *        internal buffer, then prints it the same way. Drop-in replacement
 *        for a printf() call you want mirrored on the display.
 */
void ssd1306_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

//////////////////////////////////////////////// INTERNAL - exposed for on-target HIL tests only ////////////////////////////////////////////////
// Not part of the stable driver API - same rationale as the m_AHT20_*()
// functions in aht20.h: lets test/ssd1306/test_ssd1306.c check what
// ssd1306_print() actually drew and where the cursor ended up, without
// needing to read pixels back off the physical display.

/**
 * @brief Read-only view of the internal framebuffer (SSD1306_FRAMEBUFFER_SIZE
 *        bytes, page-major - same layout ssd1306_flush() sends over I2C).
 */
const uint8_t *ssd1306_test_framebuffer(void);

/** @brief Current text cursor position, in pixels. */
void ssd1306_test_cursor(int *x, int *y);

#endif // SSD1306_H
