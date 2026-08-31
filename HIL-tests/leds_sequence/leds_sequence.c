/**
 * @file leds_sequence.c
 * @author Adrian Silva Palafox
 * @brief Example firmware demonstrating open-drain-style GPIO control on the
 *        RP2040 by driving three LEDs in a counting sequence, with UART
 *        logging of each state transition. LEDs areconnect for open-drain
 *        operation, so the firmware drives the GPIOs low to turn on the
 *        corresponding LED, and high to turn it off. The resulting LED states
 *        are logged over UART.
 * @version 0.1
 * @date 2026-08-30
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

/** @name Bit position masks
 *  Masks used to extract individual bits from the sequence counter.
 *  @{
 */
#define BIT_0 0x01 /**< Least significant bit. */
#define BIT_1 0x02 /**< Second bit. */
#define BIT_2 0x04 /**< Third bit. */
/** @} */

/** @name LED GPIO pin assignments
 *  @{
 */
#define LED_RED 15    /**< GPIO pin driving the red LED. */
#define LED_YELLOW 14 /**< GPIO pin driving the yellow LED. */
#define LED_GREEN 13  /**< GPIO pin driving the green LED. */
/** @} */

/** @name UART configuration
 *  Parameters used to initialize uart0 for debug logging.
 *  @{
 */
#define BAUD_RATE 115200        /**< UART baud rate, in bits per second. */
#define DATA_BITS 8             /**< Number of data bits per frame. */
#define STOP_BITS 1             /**< Number of stop bits per frame. */
#define PARITY UART_PARITY_NONE /**< Parity mode. */
/** @} */

/** @name UART pin assignments
 *  uart0 is typically connected to a PC or another host for debugging.
 *  @{
 */
#define TX_PIN 1 /**< GPIO pin used for UART TX. */
#define RX_PIN 0 /**< GPIO pin used for UART RX. */
/** @} */

/**
 * @brief Program entry point.
 *
 * Initializes stdio and uart0 for debug logging, configures the RED,
 * YELLOW and GREEN LED pins as outputs, then loops forever driving the
 * LEDs in a binary counting sequence: each LED reflects the complement of
 * one bit of a free-running counter, incremented once per second. The
 * resulting LED states are also printed over UART.
 *
 * @return int Never returns; the function runs an infinite loop.
 */
int main()
{
    // --- UART initialization and configuration ---

    // Initialize stdio for debugging (e.g., via USB serial)
    stdio_init_all();

    // Initialize uart0 with the specified baud rate
    uart_init(uart0, BAUD_RATE);

    // Set the GPIO pins to their UART function
    gpio_set_function(TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RX_PIN, GPIO_FUNC_UART);

    // Disable hardware flow control (CTS/RTS)
    uart_set_hw_flow(uart0, false, false);

    // Set the data format (8 data bits, 1 stop bit, no parity)
    uart_set_format(uart0, DATA_BITS, STOP_BITS, PARITY);

    // --- GPIO initialization for LEDs ---

    // Initialize the GPIO pins for the LEDs
    gpio_init(LED_RED);
    gpio_init(LED_YELLOW);
    gpio_init(LED_GREEN);

    // Set the GPIO pins as outputs
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_YELLOW, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    uint cont = 0;
    while (true)
    {
        // Drive the LEDs from the complement of the counter bits
        gpio_put(LED_RED, (~cont & BIT_0) >> 0);    // Toggle RED LED
        gpio_put(LED_YELLOW, (~cont & BIT_1) >> 1); // Toggle YELLOW LED
        gpio_put(LED_GREEN, (~cont & BIT_2) >> 2);  // Toggle GREEN LED

        // Equivalent low-level alternative using the SIO peripheral directly:
        // sio_hw->gpio_out = ((0x1l << LED_GREEN) & ((cont & BIT_0) >> 0)) |
        //                    ((0x1l << LED_YELLOW) & ((cont & BIT_1) >> 1)) |
        //                    ((0x1l << LED_RED) & ((cont & BIT_2) >> 2));

        printf("Hello, world! I'm the Raspberry Pi Pico\n");
        printf("LEDs state: RED=%d, YELLOW=%d, GREEN=%d\n",
               gpio_get(LED_RED), gpio_get(LED_YELLOW), gpio_get(LED_GREEN));

        cont++;
        sleep_ms(1000);
    }
}
