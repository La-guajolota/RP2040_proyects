/**
 * @file DSP_pract1.c
 * @brief DSP practice 1 for RP2040 MCU
 *
 * This file contains the implementation of a simple DSP practice
 * using the RP2040 microcontroller. It reads ADC values periodically
 * and sends the data over UART.
 *
 * @author Adrián Silva Palafox
 *
 * @date febrero 24 del 2025
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "hardware/adc.h"

// PINOUTS MCU
#define ADC_PIN 26     ///< The GPIO pin used for ADC input.
#define UART0_TX_PIN 0 ///< The GPIO pin used for UART0 transmit.
#define UART0_RX_PIN 1 ///< The GPIO pin used for UART0 receive.

// UART PARAMS
#define BAUD_RATE 115200 ///< The baud rate for UART communication.

// PROTOTYPES
volatile bool timer_flag = false;           ///< A flag to indicate that the timer has fired.
bool timer_callback(repeating_timer_t *rt); ///< The callback function for the repeating timer.

// GLOBAL
const int64_t SAMPLE_TIME = 100; ///< The time between ADC samples, in microseconds.
volatile uint32_t adc_value;     ///< A variable to store the ADC value.
char buffer[10];                 ///< A buffer to store the string to be transmitted over UART.

/**
 * @brief The main function of the program.
 *
 * This function initializes the necessary peripherals (stdio, UART, ADC), sets up a repeating timer
 * to trigger ADC readings, and then enters an infinite loop to process the ADC data.
 *
 * @return int This function should not return.
 */
int main()
{
    // Initialize all standard I/O
    stdio_init_all();
    // Initialize UART0 with the specified baud rate
    uart_init(uart0, BAUD_RATE);
    // Initialize the ADC
    adc_init();

    // Create a repeating timer that calls timer_callback every SAMPLE_TIME microseconds
    repeating_timer_t timer;
    add_repeating_timer_us(SAMPLE_TIME, timer_callback, NULL, &timer);

    // Set the GPIO function for the UART pins
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);

    // Initialize the ADC GPIO pin
    adc_gpio_init(ADC_PIN);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    // Get and print the clock frequencies
    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    printf("Code running OK :)\n");
    printf("clk_adc  = %dkHz\n", f_clk_adc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    sleep_ms(5000);

    // Infinite loop
    while (true)
    {
        // Check if the timer has fired
        if (timer_flag)
        {
            // Reset the timer flag
            timer_flag = false;

            // Oversampling and averaging to reduce noise
            adc_value = 0;
            for (int i = 0; i < 4; i++)
            {
                adc_value += adc_read();
            }
            adc_value /= 4;

            // Print the ADC value to the console. This data can be captured by a Python script for further analysis.
            printf("%d\n", adc_value); // It takes approximately 434.028us to transmit 5 characters at 115200 baudrate
        }
    }
}

/**
 * @brief The callback function for the repeating timer.
 *
 * This function is called every time the repeating timer fires. It sets the timer_flag to true.
 *
 * @param rt A pointer to the repeating_timer_t structure.
 * @return bool Always returns true to keep the timer repeating.
 */
bool timer_callback(repeating_timer_t *rt)
{
    timer_flag = true;
    return true;
}