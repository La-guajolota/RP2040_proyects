/**
 * @file LiDAR_TFluna.c
 * @brief Main application for the TF-Luna LiDAR sensor interface.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

// Include the header file for the TF-Luna sensor.
#include "tf_luna.h"

// Intances
tf_luna_t LiDAR;

// Prototypes
#define servo_pwm 0  // PWM channel for servo control
#define servo_gpio 0 // GPIO pin for servo control
int pos = 0;
bool going_up = true;
void move_servo(void);

volatile bool data_ready = true; // Flag to indicate if new data is ready.
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

int main()
{
    stdio_init_all();

    // Initialize PWM for servo control.
    gpio_set_function(servo_gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(servo_gpio);
    pwm_config config = pwm_get_default_config(); // Get the default PWM configuration.
    pwm_config_set_clkdiv(&config, 4.f);          // Set the clock divider to 4.
    pwm_init(slice_num, &config, true);           // Initialize PWM with the configuration.

    // Initialize I2C.
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

    while (true)
    {
        if (data_ready)
        {
            data_ready = false;             // Clear the flag.
            get_distance(&LiDAR);           // Read the distance value.
            printf("%d\n", LiDAR.distance); // Print the distance value.
            move_servo();                   // Move servo motor.
            sleep_ms(250);                  // Delay x ms.
        }
    }
}

// Function to move the servo motor.
void move_servo(void)
{
    if (going_up)
    {
        ++pos;
        if (pos > 255)
        {
            pos = 255;
            going_up = false;
        }
    }
    else
    {
        --pos;
        if (pos < 0)
        {
            pos = 0;
            going_up = true;
        }
    }

    pwm_set_gpio_level(servo_gpio, pos * pos);
}