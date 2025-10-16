/**
 * @file digital_modulators.c
 * @brief Demonstrates various digital modulation techniques using the RP2040.
 *
 * This program reads an analog signal from the ADC and uses it to generate
 * several types of modulated signals:
 * - Pulse Width Modulation (PWM)
 * - Pulse Code Modulation (PCM)
 * - A fixed pulse train for Pulse Amplitude Modulation (PAM) demonstration.
 *
 * The generated signals can be observed on GPIO pins or via serial communication.
 *
 * @author Adrián Silva Palafox
 * @date February 27, 2025
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/uart.h"

// ADC CONFIG
#define ADC_PIN 26           ///< ADC input pin for the modulating signal.
volatile uint16_t adc_value; ///< Variable to store the 12-bit ADC value.
uint8_t _8bit_adc;           ///< Variable to store the 8-bit quantized ADC value for PCM.

// UART CONFIG
#define UART0_TX_PIN 0   ///< UART0 TX pin.
#define UART1_TX_PIN 8   ///< UART1 TX pin.
#define BAUD_RATE 115200 ///< UART baud rate.

// PWM OUTPUT CONFIG
#define PWM_PWM_PIN 22 ///< GPIO pin for the PWM signal output.
#define PWM_PAM_PIN 29 ///< GPIO pin for the PAM carrier signal output.
const uint16_t pwm_duty_pam = 0xFFFF * 0.95; ///< Fixed duty cycle for the PAM carrier pulse.

// PROTOTYPES

/**
 * @brief Main function of the program.
 *
 * Initializes peripherals and enters an infinite loop to perform the modulations.
 * @return int Should not return.
 */
int main()
{
    // INITIALIZATION OF PERIPHERALS AND SYSTEM
    stdio_init_all(); // For USB CDC (printf)
    uart_init(uart0, BAUD_RATE);
    uart_init(uart1, BAUD_RATE);
    adc_init();

    // GPIO ALTERNATE FUNCTION CONFIG
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(PWM_PWM_PIN, GPIO_FUNC_PWM);
    gpio_set_function(PWM_PAM_PIN, GPIO_FUNC_PWM);
    adc_gpio_init(ADC_PIN);
    adc_select_input(0); // Select ADC0 (GPIO26)

    // PWM CONFIGS
    pwm_config pwm_config = pwm_get_default_config();

    // Get PWM slice and channel for the PWM output pin
    uint slice_num_pwm = pwm_gpio_to_slice_num(PWM_PWM_PIN);
    uint chnn_slice_pwm = pwm_gpio_to_channel(PWM_PWM_PIN);
    // Get PWM slice and channel for the PAM carrier pin
    uint slice_num_pam = pwm_gpio_to_slice_num(PWM_PAM_PIN);
    uint chnn_slice_pam = pwm_gpio_to_channel(PWM_PAM_PIN);

    // Configure PWM for PWM output. The wrap value sets the period.
    // A wrap of 0xFFF matches the 12-bit resolution of the ADC.
    pwm_set_wrap(slice_num_pwm, 0x0FFF - 1);
    // Configure PWM for PAM carrier. A 16-bit wrap value for higher resolution timing.
    pwm_set_wrap(slice_num_pam, 0xFFFF);

    // Set the fixed duty cycle for the PAM carrier signal.
    // Note: This creates a constant pulse train. True PAM would require modulating this amplitude.
    pwm_set_chan_level(slice_num_pam, chnn_slice_pam, pwm_duty_pam);
    // Initialize the PWM output to 0.
    pwm_set_chan_level(slice_num_pwm, chnn_slice_pwm, 0);

    // Enable both PWM slices.
    pwm_set_enabled(slice_num_pam, true);
    pwm_set_enabled(slice_num_pwm, true);

    while (true)
    {
        // 1. Sample the analog signal
        adc_value = adc_read();

        // 2. Generate Pulse Width Modulation (PWM)
        // The width of the pulse is directly proportional to the ADC value.
        pwm_set_chan_level(slice_num_pwm, chnn_slice_pwm, adc_value);

        // 3. Generate Pulse Position Modulation (PPM)
        // Note: The comment mentions PPM is implemented in PIO, but no PIO code is present here.
        // True PPM would involve shifting the position of a pulse based on the ADC value.

        // 4. Generate Pulse Code Modulation (PCM)
        // The 12-bit ADC value is quantized to an 8-bit value.
        _8bit_adc = (adc_value * 0xFF) / 0xFFF;
        // The 8-bit value is transmitted as raw binary data over USB CDC.
        printf("%c", _8bit_adc);
    }
}
