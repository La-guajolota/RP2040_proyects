/**
 * @file blink_simple.c
 * @brief A simple LED blinking example for the Raspberry Pi Pico.
 * @author Raspberry Pi (Trading) Ltd.
 * @date 2020
 * @license BSD-3-Clause
 */

#include "pico/stdlib.h"

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250 ///< The delay in milliseconds between LED state changes.
#endif

#ifndef PICO_DEFAULT_LED_PIN
#warning "blink_simple example requires a board with a regular LED"
#endif

/**
 * @brief Initializes the GPIO for the LED.
 */
void pico_led_init(void) {
#ifdef PICO_DEFAULT_LED_PIN
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif
}

/**
 * @brief Turns the LED on or off.
 * @param led_on true to turn the LED on, false to turn it off.
 */
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#endif
}

/**
 * @brief The main function of the program.
 *
 * Initializes the LED pin and then enters an infinite loop to blink the LED.
 *
 * @return int This function should not return.
 */
int main() {
    // Initialize the LED pin.
    pico_led_init();

    // Infinite loop to blink the LED.
    while (true) {
        // Turn the LED on.
        pico_set_led(true);
        // Wait for the specified delay.
        sleep_ms(LED_DELAY_MS);
        // Turn the LED off.
        pico_set_led(false);
        // Wait for the specified delay.
        sleep_ms(LED_DELAY_MS);
    }
}
