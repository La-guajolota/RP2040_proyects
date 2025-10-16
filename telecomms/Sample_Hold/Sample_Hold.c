/**
 * @file Sample_Hold.c
 * @author Adrián Silva Palafox (adriansilpa@gmail.com)
 * @brief Drives an external Sample and Hold circuit with a variable sampling frequency.
 * @details This program generates a periodic pulse to control the switch of a Sample and Hold
 *          circuit. The frequency of this pulse (the sampling rate) is controlled by an
 *          analog input, read by the ADC from a potentiometer.
 * @version 0.1
 * @date 2025-02-17
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"

// MACROS
/* Sampling period limits in microseconds */
#define HZ_100_PERIOD 10000 // 100 Hz
#define HZ_1000_PERIOD 1000 // 1 kHz

/* Pinouts */
#define INBOARD_LED_PIN 25   // The onboard LED pin.
#define BJT_BASE_PIN 16      // GPIO pin to control the switch (e.g., a BJT) of the S&H circuit.
#define ADC_PIN 26           // ADC pin to read the potentiometer for frequency control.

// GLOBAL VARIABLES
int64_t sample_period_us = HZ_1000_PERIOD;      // Initial sample period (1 kHz).
const float conversion_factor = 3.3f / (1 << 12); // For 12-bit ADC.
volatile uint16_t adc_reading;

/**
 * @brief Calculates the new sampling period based on the ADC reading.
 * @param adc_lec The 12-bit value from the ADC.
 * @details Maps the ADC range (0-4095) to the desired sampling period range (1kHz to 100Hz).
 */
void update_sample_period(uint16_t adc_lec)
{
    // Linearly map the ADC value to the sample period range.
    sample_period_us = adc_lec * (HZ_100_PERIOD - HZ_1000_PERIOD) / 4095 + HZ_1000_PERIOD;
}

// TIMER CALLBACK
volatile bool timer_flag = false;
/**
 * @brief Callback function for the repeating timer.
 * @details This function is called when the sampling timer fires. It sets a flag
 *          to trigger the generation of the sampling pulse in the main loop.
 */
bool timer_sampler_callback(__unused struct repeating_timer *t)
{
    timer_flag = true;
    return true;
}

/**
 * @brief Main function of the program.
 */
int main()
{
    stdio_init_all();
    adc_init();

    // ADC configuration for the potentiometer
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);

    // GPIO configuration for the S&H switch control
    gpio_init(BJT_BASE_PIN);
    gpio_set_dir(BJT_BASE_PIN, GPIO_OUT);
    gpio_put(BJT_BASE_PIN, 0); // Ensure the switch is initially off.

    // PWM configuration for the onboard LED. Note: This seems to be for debug/visual feedback
    // and is not directly related to the sample and hold functionality.
    gpio_set_function(INBOARD_LED_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(INBOARD_LED_PIN);
    pwm_set_wrap(slice_num, 255); // Set a wrap value for the PWM.
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(INBOARD_LED_PIN), 128); // 50% duty cycle.
    pwm_set_enabled(slice_num, true);

    // Periodic Timer setup
    struct repeating_timer timer_sampler;
    add_repeating_timer_us(sample_period_us, timer_sampler_callback, NULL, &timer_sampler);

    while (true)
    {
        // Check if it's time to generate a new sample pulse.
        if (timer_flag)
        {
            timer_flag = false;

            // To dynamically change the timer's period, we must cancel and re-initialize it.
            cancel_repeating_timer(&timer_sampler);
            add_repeating_timer_us(sample_period_us, timer_sampler_callback, NULL, &timer_sampler);

            // Generate a short pulse (100us) on the BJT_BASE_PIN.
            // This pulse briefly closes the switch in the external S&H circuit, allowing the
            // capacitor to charge to the input signal's voltage.
            gpio_put(BJT_BASE_PIN, 1);
            sleep_us(100);
            gpio_put(BJT_BASE_PIN, 0);
        }

        // Continuously read the potentiometer to get the desired sampling frequency.
        adc_reading = adc_read() & 0xFFF0; // Read and clear the last 4 bits to reduce noise.
        update_sample_period(adc_reading);
    }
}
