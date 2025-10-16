/**
 * @file hello_uart.c
 * @brief An interrupt-driven UART bridge for the RP2040.
 *
 * This program configures two UART peripherals on the Raspberry Pi Pico
 * to act as a bridge, forwarding data between them. It uses interrupts
 * for receiving data, making the process efficient and non-blocking.
 *
 * @author Adrián Silva Palafox
 * @date October 2025
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

// UART configuration
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// uart0 -- typically connected to a PC or another host
#define INTEL_N100 uart0
#define INTEL_N100_TX_PIN 1
#define INTEL_N100_RX_PIN 0

// uart1 -- can be connected to a peripheral, e.g., an RS485 transceiver
#define RS485 uart1
#define RS485_TX_PIN 20
#define RS485_RX_PIN 21

// Function prototypes for the interrupt service routines
void on_RS485_rx(void);
void on_INTEL_N100_rx(void);

/**
 * @brief Main function of the program.
 *
 * Initializes two UART peripherals, sets up their GPIOs, and configures
 * interrupts to handle byte reception. The main loop does nothing, as all
 * data forwarding is handled by the ISRs.
 */
void main()
{
    // Initialize stdio for debugging (e.g., via USB serial)
    stdio_init_all();

    // Initialize both UART instances with the specified baud rate
    uart_init(RS485, BAUD_RATE);
    uart_init(INTEL_N100, BAUD_RATE);

    // Set the GPIO pins to their UART function
    gpio_set_function(RS485_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RS485_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(INTEL_N100_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(INTEL_N100_RX_PIN, GPIO_FUNC_UART);

    // Disable hardware flow control (CTS/RTS)
    uart_set_hw_flow(RS485, false, false);
    uart_set_hw_flow(INTEL_N100, false, false);

    // Set the data format (8 data bits, 1 stop bit, no parity)
    uart_set_format(RS485, DATA_BITS, STOP_BITS, PARITY);
    uart_set_format(INTEL_N100, DATA_BITS, STOP_BITS, PARITY);

    // Disable the UART FIFOs. This means the UART interrupt is triggered for each character.
    // Useful for low-latency applications.
    uart_set_fifo_enabled(RS485, false);
    uart_set_fifo_enabled(INTEL_N100, false);

    // Set up the interrupt handlers. One for each UART.
    // irq_set_exclusive_handler is used to ensure no other handler is registered for these IRQs.
    irq_set_exclusive_handler(UART0_IRQ, on_INTEL_N100_rx);
    irq_set_exclusive_handler(UART1_IRQ, on_RS485_rx);

    // Enable the IRQs
    irq_set_enabled(UART0_IRQ, true);
    irq_set_enabled(UART1_IRQ, true);

    // Enable the UART to generate an interrupt when it receives data
    uart_set_irq_enables(RS485, true, false); // Only enable RX interrupt
    uart_set_irq_enables(INTEL_N100, true, false); // Only enable RX interrupt

    // The main loop is empty. All the work is done by the interrupts.
    while (1)
    {
        tight_loop_contents();
    }
}

/**
 * @brief Interrupt Service Routine for RS485 (uart1) RX.
 *
 * This function is called whenever a character is received on the RS485 UART.
 * It reads the character and forwards it to the INTEL_N100 UART.
 */
void on_RS485_rx()
{
    while (uart_is_readable(RS485))
    {
        uint8_t ch = uart_getc(RS485);
        // If the other UART is writable, send the character
        if (uart_is_writable(INTEL_N100))
        {
            uart_putc(INTEL_N100, ch);
        }
    }
}

/**
 * @brief Interrupt Service Routine for INTEL_N100 (uart0) RX.
 *
 * This function is called whenever a character is received on the INTEL_N100 UART.
 * It reads the character and forwards it to the RS485 UART.
 */
void on_INTEL_N100_rx()
{
    while (uart_is_readable(INTEL_N100))
    {
        uint8_t ch = uart_getc(INTEL_N100);
        // If the other UART is writable, send the character
        if (uart_is_writable(RS485))
        {
            uart_putc(RS485, ch);
        }
    }
}