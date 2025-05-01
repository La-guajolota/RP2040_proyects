/**
 * @file modulation_output.c
 * @brief This code is to output different digital modulations such as PAM, PCM, PWM, PPM sampling through our ADC.
 *
 * This file contains functions and logic to generate and output various digital modulation signals.
 * The modulations include:
 * - Pulse Amplitude Modulation (PAM)
 * - Pulse Code Modulation (PCM)
 * - Pulse Width Modulation (PWM)
 * - Pulse Position Modulation (PPM)
 *
 * The generated signals are sampled through the Analog-to-Digital Converter (ADC) for further processing.
 *
 * @author Adrián Silva Palafox
 * @date frebrero 27 2025
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/uart.h"

// ADC CONFIG
#define ADC_PIN 26           ///< ADC input pin
volatile uint16_t adc_value; ///< Variable to store ADC value
uint8_t _8bit_adc;

// UART CONFIG
#define UART0_TX_PIN 0   ///< UART0 TX pin
#define UART1_TX_PIN 8   ///< UART1 TX pin
#define BAUD_RATE 115200 ///< UART baud rate

// PWM CONFIG
#define PWM_PWM_PIN 22 ///< PWM output pin
#define PWM_PAM_PIN 29 ///< PWM output pin
const uint16_t pwm_duty_pam = 0xFFFF * 0.95;

// PROTOTYPES

int main()
{
    // INITIALIZATION OF PERIPHERALS AND SYSTEM
    stdio_init_all();
    uart_init(uart0, BAUD_RATE);
    uart_init(uart1, BAUD_RATE);
    adc_init();

    // GPIO ALTERNATE FUNCTION CONFIG
    gpio_set_function(UART0_TX_PIN, UART_FUNCSEL_NUM(uart0, UART0_TX_PIN));
    gpio_set_function(UART1_TX_PIN, UART_FUNCSEL_NUM(uart1, UART1_TX_PIN));
    gpio_set_function(PWM_PWM_PIN, GPIO_FUNC_PWM);
    gpio_set_function(PWM_PAM_PIN, GPIO_FUNC_PWM);
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);

    // UART CONGIG

    // PWM CONFIGS
    pwm_config pwm_config = pwm_get_default_config();

    uint slice_num_pwm = pwm_gpio_to_slice_num(PWM_PWM_PIN);
    uint chnn_slice_pwm = pwm_gpio_to_channel(PWM_PWM_PIN);
    uint slice_num_pam = pwm_gpio_to_slice_num(PWM_PAM_PIN);
    uint chnn_slice_pam = pwm_gpio_to_channel(PWM_PAM_PIN);

    pwm_set_wrap(slice_num_pwm, 0x0FFF - 1); // Set the highest value the counters will reach before returning to 0
    pwm_set_wrap(slice_num_pam, 0xFFFF);
    // pwm_set_phase_correct(slice_num_pwm, true);

    pwm_set_chan_level(slice_num_pam, chnn_slice_pam, pwm_duty_pam);
    pwm_set_chan_level(slice_num_pwm, chnn_slice_pwm, 0);

    pwm_set_enabled(slice_num_pam, true); // PAM modulation is always running without any more cpu duty
    pwm_set_enabled(slice_num_pwm, true);

    while (true)
    {
        // SAMPLE SIGNAL
        adc_value = adc_read();

        // PWM
        pwm_set_chan_level(slice_num_pwm, chnn_slice_pwm, adc_value);

        // PPM is implemented in PIO

        // PCM
        _8bit_adc = (adc_value * 0xFF) / 0xFFF;
        printf("%c", _8bit_adc); // Transmitted to USB cable
        // uart_putc(uart1, _8bit_adc);
    }
}
