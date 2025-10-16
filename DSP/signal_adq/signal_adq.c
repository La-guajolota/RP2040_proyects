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
#define ADC_PIN 26         ///< ADC pin to be used for analog input.
#define BUFFER_LENGTH 1024 ///< The length of the buffer to store ADC samples.
#define TSAMPLE_RATE 200   ///< The sampling period in microseconds (200us = 5kHz sampling rate).

volatile uint16_t adc_buffer[BUFFER_LENGTH]; ///< Buffer to store the acquired ADC values.
volatile uint16_t buffer_index = 0;          ///< Index to keep track of the current position in the ADC buffer.
volatile bool full_buffer = false;           ///< Flag to indicate when the ADC buffer is full.
struct repeating_timer timer;                ///< Repeating timer instance.

/**
 * @brief Initializes the ADC buffer with zeros.
 */
void initialize_adc_buffer()
{
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        adc_buffer[i] = 0;
    }
}

/**
 * @brief Callback function for the repeating timer.
 *
 * This function is called periodically by the timer. It reads a value from the ADC,
 * stores it in the buffer, and increments the buffer index. When the buffer is full,
 * it sets a flag and stops the timer.
 *
 * @param t Pointer to the repeating_timer structure.
 * @return true to keep the timer running (unless canceled).
 */
bool repeating_timer_callback(struct repeating_timer *t)
{
    // Read the ADC and store the value in the buffer.
    adc_buffer[buffer_index] = adc_read();
    buffer_index++;

    // Check if the buffer is full.
    if (buffer_index >= BUFFER_LENGTH)
    {
        full_buffer = true;        // Set the flag to indicate the buffer is full.
        cancel_repeating_timer(t); // Stop the timer.
    }
    return true; // Return true to keep the timer running.
}

/**
 * @brief Main function of the program.
 *
 * Initializes peripherals, sets up a repeating timer for ADC sampling, and enters
 * an infinite loop. When the ADC buffer is full, it transmits the data over USB-CDC
 * and then restarts the sampling process.
 *
 * @return int Should not return.
 */
int main()
{
    // Initialize stdio for USB output.
    stdio_init_all();

    // Delay to ensure USB is ready before proceeding.
    sleep_ms(1);

    // Initialize ADC peripheral.
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0); // Select ADC input 0 (GPIO26).

    // Initialize the ADC buffer.
    initialize_adc_buffer();

    // Create a repeating timer for periodic sampling.
    // A negative value for the delay makes the timer fire immediately and then repeat.
    add_repeating_timer_us(-TSAMPLE_RATE, repeating_timer_callback, NULL, &timer);

    while (true)
    {
        // Check if the buffer is full and ready to be processed.
        if (full_buffer)
        {
            // Transmit the entire buffer over USB (stdout).
            // Each value is printed on a new line.
            for (int i = 0; i < BUFFER_LENGTH; i++)
            {
                printf("%d\r\n", adc_buffer[i]);
            }

            // Reset the flag and buffer for the next acquisition cycle.
            full_buffer = false;
            initialize_adc_buffer();
            buffer_index = 0; // Reset buffer index.

            // Restart the timer to begin a new sampling period.
            add_repeating_timer_us(-TSAMPLE_RATE, repeating_timer_callback, NULL, &timer);
        }
    }
}
