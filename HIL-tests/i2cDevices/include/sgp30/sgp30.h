#ifndef SGP30_H
#define SGP30_H

#include <stdbool.h>
#include <stdint.h>
#include "hardware/i2c.h"
#include "sgp30_crc_table.h"

// SGP30 7-bit I2C address (fixed by the sensor).
#define SGP30_ADDR 0x58

typedef struct
{
    uint16_t co2eq_ppm;
    uint16_t tvoc_ppb;
} sgp30_measurement_t;

typedef struct
{
    uint16_t co2eq_ppm;
    uint16_t tvoc_ppb;
} sgp30_iaq_baseline_t;

/**
 * @brief Initialize the SGP30 driver with the given I2C port.
 *
 * @param i2c_port
 */
void sgp30_init(i2c_inst_t *i2c_port);

bool sgp30_iaq_init();

/**
 * @brief Send the "measure_iaq" command, wait the sensor's max execution
 *        time, then read back and decode the 6-byte reply.
 *
 * @param out filled with the decoded measurement on success.
 * @return true on success, false if either the write or the read over
 *         I2C did not transfer the expected number of bytes.
 */
bool sgp30_measure_iaq(sgp30_measurement_t *out);

/**
 * @brief Send the "get_iaq_baseline" command, wait the sensor's max execution
 *        time, then read back and decode the 6-byte reply.
 *
 * @param out filled with the decoded baseline on success.
 * @return true on success, false if either the write or the read over
 *         I2C did not transfer the expected number of bytes.
 */
bool sgp30_get_iaq_baseline(sgp30_iaq_baseline_t *out);

#endif // SGP30_H
