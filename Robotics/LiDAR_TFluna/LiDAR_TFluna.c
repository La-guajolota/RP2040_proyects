/**
 * @file LiDAR_TFluna.c
 * @brief Main application for the TF-Luna LiDAR sensor interface with MG995 servo control.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

// Include the header file for the TF-Luna sensor.
#include "tf_luna.h"

// Instances
tf_luna_t LiDAR;

// Servo Configuration
#define SERVO_GPIO 0         // GPIO pin for servo control
#define SERVO_MIN_PULSE 1000 // Pulse width in µs for 0 degrees (1ms)
#define SERVO_MAX_PULSE 2000 // Pulse width in µs for 180 degrees (2ms)
#define SERVO_FREQ 50        // PWM frequency in Hz (standard for servos is 50Hz)

// Variables for servo control
int current_angle = 0;          // Current angle of the servo (0-180)
bool scanning_direction = true; // true = increasing angle, false = decreasing angle
#define ANGLE_STEP 5            // Step size for angle change in degrees
#define SCAN_DELAY_MS 100       // Delay between angle changes in milliseconds

// PWM configuration variables
uint slice_num;
uint16_t pwm_wrap;

volatile bool data_ready = true; // Flag to indicate if new data is ready.

// Function prototypes
void setup_servo(void);
void set_servo_angle(int angle);
void scan_servo(void);

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

/**
 * @brief Setup the servo motor PWM
 */
void setup_servo(void)
{
    // Set GPIO function to PWM
    gpio_set_function(SERVO_GPIO, GPIO_FUNC_PWM);

    // Get PWM slice number
    slice_num = pwm_gpio_to_slice_num(SERVO_GPIO);

    // Calculate the wrap value for 50Hz frequency
    // Clock frequency is 125MHz, with a divider of 125 gives 1MHz
    // For 50Hz, we need a wrap value of 20000 (1000000/50)
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f); // 125MHz / 125 = 1MHz
    pwm_wrap = 20000;                       // 1MHz / 50Hz = 20000 cycles per PWM period
    pwm_config_set_wrap(&config, pwm_wrap - 1);

    // Initialize PWM
    pwm_init(slice_num, &config, true);

    // Set initial position to 0 degrees
    set_servo_angle(0);
}

/**
 * @brief Set the servo to a specific angle
 *
 * @param angle Angle in degrees (0-180)
 */
void set_servo_angle(int angle)
{
    // Constrain angle to 0-180 range
    if (angle < 0)
        angle = 0;
    if (angle > 180)
        angle = 180;

    // Map angle (0-180) to pulse width (1000-2000 µs)
    uint16_t pulse_width = SERVO_MIN_PULSE + (angle * (SERVO_MAX_PULSE - SERVO_MIN_PULSE) / 180);

    // Convert pulse width in µs to PWM cycles (at 1MHz clock)
    uint16_t level = (pulse_width * pwm_wrap) / 20000;

    // Set PWM level
    pwm_set_gpio_level(SERVO_GPIO, level);

    // Update current angle
    current_angle = angle;
}

/**
 * @brief Scan the servo back and forth
 */
void scan_servo(void)
{
    // Change direction when reaching limits
    if (scanning_direction)
    {
        current_angle += ANGLE_STEP;
        if (current_angle >= 180)
        {
            current_angle = 180;
            scanning_direction = false;
        }
    }
    else
    {
        current_angle -= ANGLE_STEP;
        if (current_angle <= 0)
        {
            current_angle = 0;
            scanning_direction = true;
        }
    }

    // Set servo to new angle
    set_servo_angle(current_angle);
}

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
            printf("%d\n", LiDAR.distance); // Print the distance value.
            scan_servo();                   // Move servo to next position.
            sleep_ms(SCAN_DELAY_MS);        // Delay between measurements.
        }
    }
}