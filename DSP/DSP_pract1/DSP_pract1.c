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
#define ADC_PIN 26     ///< ADC input pin
#define UART0_TX_PIN 0 ///< UART0 TX pin
#define UART0_RX_PIN 1 ///< UART0 RX pin

// UART PARAMS
#define BAUD_RATE 115200 ///< UART baud rate

// PROTOTYPES
volatile bool timer_flag = false;           ///< Flag to indicate timer event
bool timer_callback(repeating_timer_t *rt); ///< Timer callback function prototype

// GLOBAL
const int64_t SAMPLE_TIME = 100; ///< Sample time in us 20, 1000, 500,100
volatile uint32_t adc_value;     ///< Variable to store ADC value
char buffer[10];                 ///< Buffer for UART transmission

int main()
{
    // Initialize peripherals
    stdio_init_all();
    uart_init(uart0, BAUD_RATE);
    adc_init();

    // Periodic timer configuration
    repeating_timer_t timer;
    add_repeating_timer_us(SAMPLE_TIME, timer_callback, NULL, &timer);

    // Set the TX and RX pins by using the function select on the GPIO
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);

    // ADC configuration
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);

    // Get the frequency of the clocks
    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    printf("Código corriendo OK :)\n");
    printf("clk_adc  = %dkHz\n", f_clk_adc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    sleep_ms(5000);

    while (true)
    {
        if (timer_flag)
        {
            timer_flag = false;

            adc_value = 0;
            for (int i = 0; i < 4; i++)
            {
                adc_value += adc_read();
                // sleep_us(1);
            }

            adc_value /= 4;
            printf("%d\n", adc_value); // 434.028us to transmit 5 characteres in 115200 baudrate
        }
    }
}

/**
 * @brief Timer callback function
 *
 * Sets the timer_flag to true to indicate a timer event.
 *
 * @param rt Pointer to the repeating_timer_t structure
 * @return true Always returns true to keep the timer repeating
 */
bool timer_callback(repeating_timer_t *rt)
{
    timer_flag = true;
    return true;
}