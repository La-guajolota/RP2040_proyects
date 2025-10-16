/**
 * @file PSK.c
 * @brief Generates the carrier signals for Binary Phase Shift Keying (BPSK) modulation.
 *
 * @details
 * This program demonstrates how to generate the two fundamental carrier signals required for BPSK
 * modulation using the RP2040's PWM hardware. It configures two PWM outputs to produce square waves
 * of the same frequency but with a 180-degree phase difference.
 *
 * To create a complete BPSK modulator, you would need to add logic (e.g., using a multiplexer
 * or another GPIO) to switch between these two signals based on a digital data stream (0s and 1s).
 *
 * @author Adrián Silva Palafox
 * @date 2025-03-06
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

/**
 * @brief Configures a GPIO pin to output a PWM signal with a 50% duty cycle.
 *
 * @param gpio The GPIO pin number to configure.
 * @param freq_hz The desired frequency of the PWM signal in Hertz.
 * @param inverted If true, the PWM output is inverted (active low). If false, it's normal (active high).
 *
 * @details
 * This function sets up a PWM slice to generate a square wave at a specific frequency.
 * The key to creating the 180-degree phase shift is the `inverted` parameter, which controls
 * the PWM channel's output polarity.
 */
void setup_pwm_phase(uint gpio, uint freq_hz, bool inverted)
{
    // Configure the GPIO pin for PWM functionality
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    // Find out which PWM slice and channel is connected to this GPIO
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    // Calculate the clock divider and wrap value to achieve the desired frequency
    uint32_t clock_freq = clock_get_hz(clk_sys);
    float divider = (float)clock_freq / (freq_hz * 4096); // Using a common resolution for calculation
    if (divider < 1.0f) divider = 1.0f; // Hardware minimum divider

    uint32_t wrap = (uint32_t)((float)clock_freq / (divider * freq_hz)) - 1;

    // Set the PWM clock divider
    pwm_set_clkdiv(slice_num, divider);
    // Set the PWM wrap value, which determines the period
    pwm_set_wrap(slice_num, wrap);

    // Set the output polarity. This is how the 180-degree phase shift is created.
    // One channel is normal, the other is inverted.
    pwm_set_output_polarity(slice_num, !inverted, inverted);

    // Set the duty cycle to 50% to create a square wave
    pwm_set_chan_level(slice_num, channel, wrap / 2);

    // Enable the PWM slice
    pwm_set_enabled(slice_num, true);

    printf("GPIO %d configured: Freq=%d Hz, Inverted=%s\n", gpio, freq_hz, inverted ? "Yes" : "No");
}

/**
 * @brief Main function to initialize and configure the BPSK carrier signals.
 */
int main()
{
    // Initialize stdio for debugging output via USB
    stdio_init_all();
    sleep_ms(2000); // Wait for the serial connection to establish

    printf("Configuring PWM signals for BPSK carrier generation...\n");

    // Define the GPIO pins for the two phases of the carrier signal
    const uint CARRIER_0_DEG_PIN = 2;   // Represents the 0-degree phase carrier
    const uint CARRIER_180_DEG_PIN = 4; // Represents the 180-degree phase carrier

    // Set up the two PWM signals with the same frequency but opposite polarity
    setup_pwm_phase(CARRIER_0_DEG_PIN, 1000, false); // 1 kHz, normal polarity (0 degrees)
    setup_pwm_phase(CARRIER_180_DEG_PIN, 1000, true);  // 1 kHz, inverted polarity (180 degrees)

    printf("BPSK carrier signals are now active on GPIO %d and GPIO %d.\n", CARRIER_0_DEG_PIN, CARRIER_180_DEG_PIN);

    // The main loop is idle as the PWM hardware generates the signals independently.
    while (true)
    {
        tight_loop_contents();
    }
}

