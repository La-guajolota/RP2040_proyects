/**
 * @file LiDAR_TFluna.c
 * @brief Main application for the TF-Luna LiDAR sensor interface with MG995 servo control.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/irq.h"

// User-defined includes
#include "sg90.h" // Include the configuration header for servo setup
#include "tf_luna.h"

// Instances
tf_luna_t LiDAR;
volatile bool data_ready = true; // Flag to indicate if new data is ready.

// Function prototypes
void gpio_callback(uint gpio, uint32_t events);

int main()
{
    stdio_init_all();

    // Initialize servo
    setup_servo();

    // Initialize I2C for LiDAR
    i2c_init(I2C_PORT, 400 * 1000); // Set clock to 400 kHz.
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    // Initialize GPIO for the "data ready" signal.
    gpio_init(TF_LUNA_MUX_OUT);
    gpio_set_dir(TF_LUNA_MUX_OUT, GPIO_IN);
    gpio_set_pulls(TF_LUNA_MUX_OUT, false, false);
    // gpio_pull_up(TF_LUNA_MUX_OUT); // Pull-up to 3.3V.

    // Configure interrupt for rising edge on the "data ready" signal.
    gpio_set_irq_enabled_with_callback(TF_LUNA_MUX_OUT, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    printf("LiDAR TF-Luna with MG995 servo scanning system initialized\n");

    while (true)
    {
        if (data_ready)
        {
            data_ready = false;   // Clear the flag.
            get_distance(&LiDAR); // Read the distance value.
            // printf("Angle: %d, Distance: %d cm\n", current_angle, LiDAR.distance); // Print the distance value with current angle.
            // printf("%d\n", LiDAR.distance); // Print the distance value.
            printf("%d:%d\n", current_angle, LiDAR.distance);
            scan_servo();            // Move servo to next position.
            sleep_ms(SCAN_DELAY_MS); // Delay between measurements.
        }
    }
}

/**
 * @brief GPIO interrupt callback function.
 *
 * This function is triggered when a rising edge is detected on the `TF_LUNA_MUX_OUT` pin.
 *
 * @param gpio The GPIO pin that triggered the interrupt.
 * @param events The type of event (e.g., rising edge).
 */
void gpio_callback(uint gpio, uint32_t events)
{
    if (gpio == TF_LUNA_MUX_OUT && (events & GPIO_IRQ_EDGE_RISE))
    {
        data_ready = true; // Indicate that new data is ready.
    }
}
