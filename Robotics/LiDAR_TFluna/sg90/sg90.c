#include "sg90.h"

// PWM configuration variables
uint slice_num;
uint16_t pwm_wrap = 20000;

int current_angle = 0;
bool scanning_direction = true;

/**
 * @brief Set the servo to a specific angle
 *
 * @param angle Angle in degrees (0-180)
 */
static void set_servo_angle(int angle)
{
    // Constrain angle to 0-180 range
    if (angle < 0)
        angle = 0;
    if (angle > 180)
        angle = 180;

    // Map angle (0-180) to pulse width (500-2400 µs)
    uint16_t pulse_width = SERVO_MIN_PULSE + (angle * (SERVO_MAX_PULSE - SERVO_MIN_PULSE) / 180);

    // Convert pulse width in µs to PWM cycles (at 1MHz clock)
    uint16_t level = (pulse_width * pwm_wrap) / 20000;

    // Ensure level is within valid range
    if (level > pwm_wrap)
        level = pwm_wrap;
    if (level < 0)
        level = 0;

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
    pwm_config_set_wrap(&config, pwm_wrap - 1);

    // Initialize PWM
    pwm_init(slice_num, &config, true);

    // Enable PWM output on the GPIO
    pwm_set_enabled(slice_num, true);

    // Set initial position to 0 degrees
    set_servo_angle(0);
}