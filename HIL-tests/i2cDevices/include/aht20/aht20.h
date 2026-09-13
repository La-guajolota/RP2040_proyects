#ifndef AHT20_H
#define AHT20_H

#include <stdbool.h>
#include <stdint.h>
#include "hardware/i2c.h"

// AHT20 7-bit I2C address (fixed by the sensor).
#define AHT20_ADDR 0x38

/**
 * @brief Result of an AHT20 driver call.
 */
typedef enum
{
    AHT20_OK = 0,
    AHT20_ERR = -1,
} AHT20_rst_t;

/**
 * @brief One decoded AHT20 reading.
 */
typedef struct
{
    float r_humidity;  /**< Relative humidity, in %RH. */
    float temperature; /**< Temperature, in degrees Celsius. */
    bool is_busy;      /**< True if the sensor reported itself busy for this reading. */
} AHT20_data_t;

/**
 * @brief Bind the driver to the I2C port the sensor is wired to.
 *
 * Call this once, after the port itself has been set up with i2c_init() +
 * gpio_set_function()/gpio_pull_up() on the SDA/SCL pins, and before any
 * other AHT20_*() call.
 *
 * @param i2c_port I2C instance (e.g. i2c0/i2c1) the sensor is on.
 */
void AHT20_init(i2c_inst_t *i2c_port);

/**
 * @brief Reset the sensor and, if it isn't already factory-calibrated,
 *        send the init command and poll status until calibration completes.
 *
 * @param pAHT20_data scratch struct used while polling calibration status.
 * @return AHT20_OK on success, AHT20_ERR on I2C failure or if the sensor
 *         doesn't report itself calibrated within a bounded number of polls.
 */
AHT20_rst_t AHT20_begin(AHT20_data_t *pAHT20_data);

/**
 * @brief Trigger a measurement, wait for it to complete, then read back
 *        and decode temperature + relative humidity.
 *
 * @param pAHT20_data filled with the decoded reading on success.
 * @return AHT20_OK on success, AHT20_ERR on I2C failure, a still-busy
 *         sensor, or an out-of-range reading.
 */
AHT20_rst_t AHT20_measure(AHT20_data_t *pAHT20_data);

//////////////////////////////////////////////// INTERNAL - exposed for on-target HIL tests only ////////////////////////////////////////////////
// Not part of the stable driver API - use AHT20_init()/AHT20_begin()/AHT20_measure() above instead.
// Declared here (rather than kept static in aht20.c) purely so test/aht20/test_aht20.c can drive
// individual I2C transactions against real hardware.

/**
 * @brief Send init command to device, in order to init the device.
 * @return AHT20_OK on success, AHT20_ERR on I2C failure.
 */
AHT20_rst_t m_AHT20_command_init(void);

/**
 * @brief Send reset command to device, in order to restart the device.
 * @return AHT20_OK on success, AHT20_ERR on I2C failure.
 */
AHT20_rst_t m_AHT20_command_reset(void);

/**
 * @brief Send measure command to device.
 * @return AHT20_OK on success, AHT20_ERR on I2C failure.
 */
AHT20_rst_t m_AHT20_command_measure(void);

/**
 * @brief Send a read-status command to device.
 * @return AHT20_OK on success, AHT20_ERR on I2C failure.
 */
AHT20_rst_t m_AHT20_command_read_status(void);

/**
 * @brief Decode the status byte (and, once fully read, the measurement)
 *        held in the driver's internal receive buffer.
 * @param pAHT20_data a pointer to the struct of data
 * @return AHT20_OK always (decoding a buffer already in memory can't fail).
 */
AHT20_rst_t m_AHT20_decode_status(AHT20_data_t *pAHT20_data);

/**
 * @brief Read @p len bytes from the device into the driver's internal
 *        receive buffer, ready for m_AHT20_decode_status().
 * @param pAHT20_data a pointer to the struct of data
 * @param len number of bytes to read (at most 7)
 * @return AHT20_OK on success, AHT20_ERR on I2C failure.
 */
AHT20_rst_t m_AHT20_get_result(AHT20_data_t *pAHT20_data, size_t len);

#endif /* AHT20_H */
