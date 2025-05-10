#ifndef SG90_H
#define SG90_H

#include <hardware/gpio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Servo Configuration
#define SERVO_GPIO 0         // GPIO pin for servo control
#define SERVO_MIN_PULSE 500  // Pulse width in µs for 0 degrees (1ms)
#define SERVO_MAX_PULSE 2400 // Pulse width in µs for 180 degrees (2ms)
#define SERVO_FREQ 50        // PWM frequency in Hz (standard for servos is 50Hz)

// Variables for servo control
#define ANGLE_STEP 10     // Step size for angle change in degrees
#define SCAN_DELAY_MS 250 // Delay between angle changes in milliseconds
extern int current_angle;

/**
 * @brief Scan the servo back and forth
 */
void scan_servo(void);

/**
 * @brief Setup the servo motor PWM
 */
void setup_servo(void);

#endif // SG90_H