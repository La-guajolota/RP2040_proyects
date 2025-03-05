/**
 * @file adc_uart_transmit.c
 * @brief ADC to UART transmission example for RP2040 MCU.
 *
 * This program initializes the ADC and UART peripherals on the RP2040 MCU.
 * It reads analog values from a specified ADC pin, converts them to digital,
 * and transmits the raw ADC value over UART. The transmitted data can be used
 * for debugging or monitoring purposes.
 *
 * @author Adrián Silva Palafox
 * @date 2025-02-20
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/adc.h"

// PINOUTS MCU
#define ADC_PIN 26     ///< ADC input pin
#define UART0_TX_PIN 0 ///< UART0 TX pin
#define UART1_TX_PIN 8 ///< UART1 TX pin

// UART PARAMS
#define BAUD_RATE 115200 ///< UART baud rate

// GLOBAL
const float conversion_factor = 3.3f / (1 << 12); ///< Conversion factor for 12-bit ADC
volatile uint16_t adc_value;                      ///< Variable to store ADC value
char buffer[10];                                  ///< Buffer for UART transmission

int main()
{
    stdio_init_all();
    uart_init(uart0, BAUD_RATE);
    uart_init(uart1, BAUD_RATE);
    adc_init();

    // We're just sending info
    gpio_set_function(UART0_TX_PIN, UART_FUNCSEL_NUM(uart0, UART0_TX_PIN));
    gpio_set_function(UART1_TX_PIN, UART_FUNCSEL_NUM(uart1, UART1_TX_PIN));

    // ADC configuration
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);

    while (true)
    {
        adc_value = adc_read() & 0xFFF0; // Clear the last 4 bits avoiding noise

        // Debug msg
        printf("Raw value: 0x%03x, voltage: %02fV\n", adc_value, adc_value * conversion_factor);

        // UART0 TX
        sprintf(buffer, "0x%03x\n", adc_value);
        uart_puts(uart1, buffer);

        sleep_ms(500);
    }
}
