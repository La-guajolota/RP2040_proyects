/**
 * @file tf_luna.h
 * @brief Interface for the TF-Luna LiDAR sensor using I2C and GPIO on the Raspberry Pi Pico.
 */

#ifndef TF_LUNA_H
#define TF_LUNA_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C configuration
#define I2C_PORT i2c0 // I2C port for TF-Luna
#define I2C_SDA 4     // GPIO pin for SDA
#define I2C_SCL 5     // GPIO pin for SCL

// GPIO configuration
#define TF_LUNA_MUX_OUT 15 // GPIO pin for "data ready" signal

// TF-Luna addresses
#define TF_LUNA_ADDR 0x10    // I2C address
#define TF_LUNA_WO_ADDR 0x20 // Write-only address
#define TF_LUNA_RO_ADDR 0x21 // Read-only address

// TF-Luna registers
#define TF_LUNA_DIST_LOW_ADDR 0x00  // Low byte of distance
#define TF_LUNA_DIST_HIGH_ADDR 0x01 // High byte of distance

typedef struct tf_luna
{
    uint16_t distance; // Distance value in cm
} tf_luna_t;

/**
 * @brief Reads the distance value from the TF-Luna sensor.
 *
 * This function reads two bytes from the sensor and combines them into a 16-bit distance value.
 *
 * @param LiDAR Pointer to the tf_luna_t structure where the distance value will be stored.
 */
void get_distance(tf_luna_t *LiDAR);

#endif // TF_LUNA_H