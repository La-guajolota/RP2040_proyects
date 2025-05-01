/**
 * @file PSK.c
 * @brief Implementation of PWM configuration for PSK modulation on the Raspberry Pi Pico (RP2040).
 *
 * @details
 * This program configures two GPIO pins to output PWM signals with the same frequency but with a 180-degree
 * phase shift between them. The signals are generated with opposite logic (one active high, the other active low),
 * effectively creating a phase-shifted square wave pair suitable for PSK modulation.
 *
 * The program initializes the USB UART for debugging and configures the PWM signals. Debugging messages
 * are printed to verify the configuration.
 *
 * @note
 * A 2:1 multiplexer (MUX) can be used to combine the two PWM signals into a single output for PSK modulation.
 * This implementation assumes external logic gates or circuitry to handle the combination.
 *
 * @author
 * Adrián Silva Palafox
 * @date
 * 2025-03-06
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

/**
 * @brief Configures a GPIO pin to output a PWM signal with a specified frequency and polarity.
 *
 * @param gpio The GPIO pin number to configure for PWM.
 * @param freq_hz The desired frequency of the PWM signal in Hertz.
 * @param polarity The polarity of the PWM signal (true for active high, false for active low).
 *
 * @details
 * This function sets up the specified GPIO pin for PWM output. It calculates the clock divider and wrap
 * values required to achieve the desired frequency. The PWM signal is initialized with a 50% duty cycle.
 * The polarity parameter determines whether the signal is active high or active low.
 */
void setup_pwm(uint gpio, uint freq_hz, bool polarity)
{
    // Configure the GPIO for PWM
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    // Retrieve the PWM slice and channel for the GPIO
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    // Calculate the clock divider and wrap value for the desired frequency
    uint32_t clock_freq = clock_get_hz(clk_sys);          // System clock frequency
    float divider = (float)clock_freq / (freq_hz * 4096); // Calculate the divider in decimal format
    if (divider < 1.0f)
        divider = 1.0f; // Minimum divider allowed by the hardware

    uint32_t int_part = (uint32_t)divider;                      // Integer part of the divider
    uint32_t frac_part = (uint32_t)((divider - int_part) * 16); // Fractional part (4 bits)

    uint32_t wrap = clock_freq / (freq_hz * divider) - 1; // Calculate the wrap value

    // Debugging messages
    printf("Configuring GPIO %d\n", gpio);
    printf("Slice: %d, Channel: %d\n", slice_num, channel);
    printf("Clock frequency: %u Hz\n", clock_freq);
    printf("Divider: %f (int: %u, frac: %u), Wrap: %u\n", divider, int_part, frac_part, wrap);

    // Configure the clock divider and wrap value
    pwm_set_clkdiv_int_frac(slice_num, int_part, frac_part);
    pwm_set_wrap(slice_num, wrap);

    // Set PWM polarity (active high or active low)
    pwm_set_output_polarity(slice_num, channel, polarity);

    // Set the duty cycle to 50%
    pwm_set_chan_level(slice_num, channel, wrap / 2);

    // Enable the PWM output
    pwm_set_enabled(slice_num, true);

    // Final debugging message
    printf("GPIO %d configured: Slice %d, Channel %d, Frequency %d Hz, Polarity: %s\n",
           gpio, slice_num, channel, freq_hz, polarity ? "Active High" : "Active Low");
}

/**
 * @brief Main function to initialize the USB UART and configure PWM signals for PSK modulation.
 *
 * @details
 * This function initializes the USB UART for debugging and configures two GPIO pins to output PWM signals
 * with the same frequency but opposite logic, creating a 180-degree phase shift between them.
 */
int main()
{
    // Initialize the USB UART for debugging at 300 baud
    stdio_uart_init_full(uart0, 300, PICO_DEFAULT_UART_TX_PIN, PICO_DEFAULT_UART_RX_PIN);

    printf("Waiting for USB connection...\n");
    sleep_ms(2000); // Wait 2 seconds to stabilize the connection

    // GPIO pins for PWM
    const uint PWM_GPIO1 = 2; // GPIO 2 -> PWM1 A
    const uint PWM_GPIO2 = 4; // GPIO 4 -> PWM2 A

    // Configure the PWM signals with the same frequency but opposite polarity
    setup_pwm(PWM_GPIO1, 1000, true);  // 1 kHz on GPIO 2, active high
    setup_pwm(PWM_GPIO2, 1000, false); // 1 kHz on GPIO 4, active low

    printf("PWM configured on GPIO 2 and GPIO 4 for PSK modulation\n");

    while (true)
    {
        printf("PSK Modulation Active\n");
        sleep_ms(250); // Wait 250 ms between messages
    }
}
