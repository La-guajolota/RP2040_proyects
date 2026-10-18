/**
 * @file blink_simple.c
 * @brief A simple LED blinking example for the Raspberry Pi Pico.
 * @author Raspberry Pi (Trading) Ltd.
 * @date 2020
 * @license BSD-3-Clause
 */

#include "pico.h"
#include "pico/stdlib.h"

#define LED_DELAY_MS 250 ///< The delay in milliseconds between LED state changes.
#define DEFAULT_LED 25
#define PWR_READ_PIN 14
#define PWR_HOLD_PIN 15

/**
 * @brief Initializes the GPIO for the LED.
 */
void gpios_init(void)
{
    gpio_init(DEFAULT_LED);
    gpio_init(PWR_HOLD_PIN);
    gpio_init(PWR_READ_PIN);

    gpio_set_dir(DEFAULT_LED, GPIO_OUT);
    gpio_set_dir(PWR_HOLD_PIN, GPIO_OUT);
    gpio_set_dir(PWR_READ_PIN, GPIO_IN);

    gpio_put(PWR_HOLD_PIN, true);
    sleep_ms(1000);
}

int main()
{
    // Initialize the LED pin.
    gpios_init();

    while (true)
    {
        if (gpio_get(PWR_READ_PIN))
        {
            gpio_put(PWR_HOLD_PIN, false);
        }
    }
}
