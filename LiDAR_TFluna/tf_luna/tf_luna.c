/**
 * @file tf_luna.c
 * @brief Implementation for the TF-Luna LiDAR sensor using I2C and GPIO on the Raspberry Pi Pico.
 */

#include "tf_luna.h"

/**
 * @brief Reads data from the TF-Luna sensor over I2C.
 *
 * @param data Pointer to the buffer where the read data will be stored.
 * @param reg The register address to read from.
 * @param len The number of bytes to read.
 */
void i2c_read(uint8_t *data, uint8_t reg, uint8_t len)
{
    i2c_write_blocking(I2C_PORT, TF_LUNA_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, TF_LUNA_ADDR, data, len, false);
}

/**
 * @brief Reads the distance value from the TF-Luna sensor.
 *
 * This function reads two bytes from the sensor and combines them into a 16-bit distance value.
 *
 * @param LiDAR Pointer to the tf_luna_t structure where the distance value will be stored.
 */
void get_distance(tf_luna_t *LiDAR)
{
    uint8_t dist_data[2];                                 // Buffer to store the distance data.
    i2c_read(dist_data, TF_LUNA_DIST_LOW_ADDR, 2);        // Read 2 bytes from the TF-Luna sensor.
    LiDAR->distance = (dist_data[1] << 8) | dist_data[0]; // Combine the two bytes into a 16-bit value.
}