/**
 * @file LiDAR_TFluna.c
 * @brief Main application for a 2D LiDAR scanner using a TF-Luna sensor and a servo motor.
 *
 * This program orchestrates a TF-Luna LiDAR sensor and a servo motor to create a
 * simple 2D LiDAR scanner. It uses an interrupt to detect when a new distance
 * measurement is ready from the LiDAR, reads the value, and then moves the servo
 * to the next position to build a 2D map of the surroundings.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/irq.h"

// User-defined includes for the servo and LiDAR sensor
#include "sg90.h"      // Header for servo control
#include "tf_luna.h"   // Header for TF-Luna LiDAR sensor

// Instances
tf_luna_t LiDAR; // Create an instance of the LiDAR sensor structure
volatile bool data_ready = true; // Flag to indicate if new data is ready from the LiDAR.

// Function prototypes
void gpio_callback(uint gpio, uint32_t events);

/**
 * @brief The main function of the program.
 *
 * Initializes the servo, I2C for the LiDAR, and a GPIO interrupt. It then enters
 * an infinite loop to scan the environment.
 *
 * @return int This function should not return.
 */
int main()
{
    stdio_init_all();

    // Initialize the servo motor
    setup_servo();

    // Initialize I2C for the TF-Luna LiDAR sensor
    i2c_init(I2C_PORT, 400 * 1000); // Use I2C port 0 at 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    // Initialize the GPIO pin for the LiDAR's "data ready" signal
    gpio_init(TF_LUNA_MUX_OUT);
    gpio_set_dir(TF_LUNA_MUX_OUT, GPIO_IN);
    gpio_set_pulls(TF_LUNA_MUX_OUT, false, false); // No pulls, assuming external pull-up/down if needed

    // Configure an interrupt to fire on the rising edge of the "data ready" signal
    gpio_set_irq_enabled_with_callback(TF_LUNA_MUX_OUT, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    printf("LiDAR TF-Luna with MG995 servo scanning system initialized\n");

    // Main loop for scanning
    while (true)
    {
        // Check if the LiDAR has indicated that a new measurement is ready
        if (data_ready)
        {
            data_ready = false;   // Reset the flag
            get_distance(&LiDAR); // Read the distance value from the LiDAR sensor

            // Print the angle and distance in a format that can be parsed by the Python UI
            printf("%d:%d\n", current_angle, LiDAR.distance);

            // Move the servo to the next scanning position
            scan_servo();

            // Wait for a short period before the next measurement
            sleep_ms(SCAN_DELAY_MS);
        }
    }
}

/**
 * @brief GPIO interrupt callback function.
 *
 * This function is triggered when a rising edge is detected on the `TF_LUNA_MUX_OUT` pin,
 * which is connected to the LiDAR's data ready output.
 *
 * @param gpio The GPIO pin that triggered the interrupt.
 * @param events The type of event (e.g., rising edge).
 */
void gpio_callback(uint gpio, uint32_t events)
{
    // Check if the interrupt was triggered by the correct pin and event
    if (gpio == TF_LUNA_MUX_OUT && (events & GPIO_IRQ_EDGE_RISE))
    {
        // Set the flag to indicate that a new distance measurement is ready to be read
        data_ready = true;
    }
}
