/**
 * @file signal_adq.c
 * @brief ADC data acquisition and UART transmission on RP2040
 *
 * This program reads analog signals using the ADC on the RP2040 microcontroller
 * and transmits the sampled data over UART. The ADC samples are stored in a buffer,
 * and once the buffer is full, the data is sent via UART. The sampling rate is
 * controlled using a repeating timer.
 *
 * Author: Adrián Silva Palafox
 * Date: 2025-03-06
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/adc.h"

// UART defines
#define BAUD_RATE 115200
#define UART0_TX_PIN 0 ///< UART0 TX pin

// ADC defines
#define ADC_PIN 26         ///< ADC pin
#define BUFFER_LENGTH 1024 ///< Buffer length
#define TSAMPLE_RATE 200   ///< Sample rate in microseconds

volatile uint16_t adc_buffer[BUFFER_LENGTH]; ///< Buffer for ADC values
volatile uint16_t buffer_index = 0;          ///< Index for the buffer
volatile bool full_buffer = false;           ///< Flag to indicate if the buffer is full
struct repeating_timer timer;                ///< Timer instance

// Function to initialize the ADC buffer
void initialize_adc_buffer()
{
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        adc_buffer[i] = 0;
    }
}

// Repeating timer callback
bool repeating_timer_callback(struct repeating_timer *t)
{
    // Read ADC value and store it in the buffer
    adc_buffer[buffer_index] = adc_read();
    buffer_index++;
    if (buffer_index >= BUFFER_LENGTH)
    {
        full_buffer = true;        // Set the flag to indicate that the buffer is full
        cancel_repeating_timer(t); // Stop the timer
    }
    return true; // Return true to keep the timer running (if not canceled)
}

int main()
{
    // Initialize stdio for USB output
    stdio_init_all();

    // Add a delay to ensure USB is ready
    sleep_ms(1);

    // Debug: Print a message to confirm the program has started
    // printf("Program started\n");

    // Initialize ADC
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0); // Select ADC input 0

    // Debug: Print ADC initialization message
    // printf("ADC initialized on pin %d\n", ADC_PIN);

    // Initialize the adc buffer
    initialize_adc_buffer();

    // Create a repeating timer for periodic sampling (200us -> 5kHz)
    add_repeating_timer_us(-TSAMPLE_RATE, repeating_timer_callback, NULL, &timer);

    while (true)
    {
        // Main loop can process the buffer if needed
        if (full_buffer)
        {
            // printf("Buffer full, sending data...\n");

            // Send the buffer over USB (stdout)
            for (int i = 0; i < BUFFER_LENGTH; i++)
            {
                printf("%d\r\n", adc_buffer[i]);
            }

            // Reset the flag after processing
            full_buffer = false;
            initialize_adc_buffer();
            buffer_index = 0; // Reset buffer index

            // Restart the timer
            add_repeating_timer_us(-TSAMPLE_RATE, repeating_timer_callback, NULL, &timer);
        }
    }
}
