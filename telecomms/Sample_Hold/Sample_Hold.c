/**
 * @file Sample_Hold.c
 * @author Adrián Silva Palafox (adriansilpa@gmail.com)
 * @brief  This code is outputs a pwm at a fixed duty-cycle
 *         but varible frecuency obeying an ADC's reading a
 *         potetiometer
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
/* Samplings periods */
#define one_hundred_hz 10000
#define one_kHz 1000
#define ten_kHz 100
/* Pinouts */
#define inboard_led 25
#define bjt_base 16
#define adc_pin 26

// GLOBAL VARIABLES
int64_t sample_period_us = one_kHz;               // 1 KHz
const float conversion_factor = 3.3f / (1 << 12); // 12 bits ADC
volatile uint16_t voltage;

// FUNCTION DECLARATIONS
void sampler_calculation(uint16_t adc_lec)
{
    // Update the sample
    sample_period_us = adc_lec * (one_hundred_hz - one_kHz) / 4095 + one_kHz;

    // Debug msg
    // printf("Raw value: 0x%03x, voltage: %fV, New_sampling_period: %d\n", adc_lec, adc_lec * conversion_factor, sample_period_us);
}

// TIMEER CALLBACK
volatile bool timer_flag = false;
bool timer_sampler_callback(__unused struct repeating_timer *t)
{
    timer_flag = true;
    // degug msg
    // printf("Timer flag set\n");
    return true;
}

int main()
{
    stdio_init_all();
    adc_init();

    // ADC configuration
    adc_gpio_init(adc_pin);
    adc_select_input(0);

    // GPIO configuration
    gpio_init(bjt_base);
    gpio_set_dir(bjt_base, GPIO_OUT);
    gpio_put(bjt_base, 0);

    // PWM configuration
    gpio_set_function(inboard_led, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(inboard_led);
    pwm_set_wrap(inboard_led, 3);

    // Periodic Timer setup
    struct repeating_timer timer_sampler;
    add_repeating_timer_us(sample_period_us, timer_sampler_callback, NULL, &timer_sampler);

    while (true)
    {
        // Samplea a la frecuencia determinada por el usuario
        if (timer_flag)
        {
            timer_flag = false;

            cancel_repeating_timer(&timer_sampler);
            add_repeating_timer_us(sample_period_us, timer_sampler_callback, NULL, &timer_sampler);

            // Sampling
            gpio_put(bjt_base, 1);
            sleep_us(100);
            gpio_put(bjt_base, 0);
        }

        voltage = adc_read() & 0xFFF0; // Clear the last 4 bits avoiding noise
        sampler_calculation(voltage);
    }
}
