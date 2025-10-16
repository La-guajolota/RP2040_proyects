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
#define ADC_PIN 26     ///< ADC input pin to be used.
#define UART0_TX_PIN 0 ///< UART0 TX pin.
#define UART1_TX_PIN 8 ///< UART1 TX pin.

// UART PARAMS
#define BAUD_RATE 115200 ///< UART baud rate in bits per second.

// GLOBAL
const float conversion_factor = 3.3f / (1 << 12); ///< Conversion factor for 12-bit ADC.
volatile uint16_t adc_value;                      ///< Variable to store the raw ADC value.
char buffer[10];                                  ///< Buffer for storing the string to be transmitted over UART.

/**
 * @brief Main function of the program.
 *
 * Initializes the necessary peripherals (stdio, UART, ADC), and enters an infinite loop
 * to continuously read the ADC value and transmit it over UART.
 *
 * @return int Should not return.
 */
int main()
{
    // Initialize stdio for printf
    stdio_init_all();
    // Initialize UART0 and UART1 with the specified baud rate
    uart_init(uart0, BAUD_RATE);
    uart_init(uart1, BAUD_RATE);
    // Initialize the ADC
    adc_init();

    // Set the GPIO pins for UART transmission
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);

    // ADC configuration
    // Initialize the GPIO pin for ADC functionality
    adc_gpio_init(ADC_PIN);
    // Select the ADC input channel (0 for GPIO26)
    adc_select_input(0);

    // Infinite loop to continuously read and transmit data
    while (true)
    {
        // Read the ADC value and clear the last 4 bits to reduce noise
        adc_value = adc_read() & 0xFFF0;

        // Print the raw ADC value and the calculated voltage to the console for debugging
        printf("Raw value: 0x%03x, voltage: %02fV\n", adc_value, adc_value * conversion_factor);

        // Format the ADC value as a hexadecimal string and store it in the buffer
        sprintf(buffer, "0x%03x\n", adc_value);
        // Transmit the buffer content over UART1
        uart_puts(uart1, buffer);

        // Wait for 500 milliseconds before the next reading
        sleep_ms(500);
    }
}
