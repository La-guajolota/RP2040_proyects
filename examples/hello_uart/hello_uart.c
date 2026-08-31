/**
 * @file uart_rs485_bridge.c
 * @brief An interrupt-driven UART to RS485 bridge for the RP2040.
 *
 * This program configures two UART peripherals on the Raspberry Pi Pico
 * to act as a bridge, forwarding data between a host (Intel N100) and
 * an RS485 bus with auto-direction transceiver.
 *
 * @author Adrián Silva Palafox
 * @date October 2025
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

// UART configuration
#define BAUD_RATE 38400
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// uart0 -- connected to Intel N100 (Linux host)
#define INTEL_N100 uart0
#define INTEL_N100_TX_PIN 0
#define INTEL_N100_RX_PIN 1

// uart1 -- connected to RS485 transceiver (auto-direction)
#define RS485 uart1
#define RS485_TX_PIN 20
#define RS485_RX_PIN 21

// Function prototypes
void on_RS485_rx(void);
void on_INTEL_N100_rx(void);

/**
 * @brief Main function of the program.
 *
 * Initializes two UART peripherals, sets up their GPIOs, and configures
 * interrupts to handle byte reception. The main loop blinks an LED to
 * show the system is running.
 */
int main()
{
    // Initialize stdio for debugging (via USB)
    stdio_init_all();
    sleep_ms(2000); // Wait for USB to enumerate

    // Initialize both UART instances
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

    // Set the data format (8N1)
    uart_set_format(RS485, DATA_BITS, STOP_BITS, PARITY);
    uart_set_format(INTEL_N100, DATA_BITS, STOP_BITS, PARITY);

    // Enable FIFOs for better performance and reduced interrupt load
    // The FIFO can hold up to 32 bytes
    uart_set_fifo_enabled(RS485, true);
    uart_set_fifo_enabled(INTEL_N100, true);

    // Set up the interrupt handlers
    irq_set_exclusive_handler(UART0_IRQ, on_INTEL_N100_rx);
    irq_set_exclusive_handler(UART1_IRQ, on_RS485_rx);

    // Enable the IRQs
    irq_set_enabled(UART0_IRQ, true);
    irq_set_enabled(UART1_IRQ, true);

    // Enable UART RX interrupts only
    uart_set_irq_enables(RS485, true, false);
    uart_set_irq_enables(INTEL_N100, true, false);

    while (1)
    {
        tight_loop_contents();
    }
    return 0;
}

/**
 * @brief ISR for RS485 (uart1) RX.
 *
 * This function is called whenever data is received on the RS485 UART.
 * It reads all available characters and forwards them to the Intel N100.
 */
void on_RS485_rx()
{
    while (uart_is_readable(RS485))
    {
        uint8_t ch = uart_getc(RS485);

        // Forward to Intel N100
        if (uart_is_writable(INTEL_N100))
        {
            uart_putc_raw(INTEL_N100, ch);
        }
        // Note: If FIFO is full, data will be lost. This should be rare
        // with proper Modbus timing and the 32-byte FIFO depth.
    }
}

/**
 * @brief ISR for INTEL_N100 (uart0) RX.
 *
 * This function is called whenever data is received on the Intel N100 UART.
 * It reads all available characters and forwards them to the RS485 bus.
 */
void on_INTEL_N100_rx()
{
    while (uart_is_readable(INTEL_N100))
    {
        uint8_t ch = uart_getc(INTEL_N100);

        // Forward to RS485
        if (uart_is_writable(RS485))
        {
            uart_putc_raw(RS485, ch);
        }
        // Note: If FIFO is full, data will be lost. This should be rare
        // with proper Modbus timing and the 32-byte FIFO depth.
    }
}