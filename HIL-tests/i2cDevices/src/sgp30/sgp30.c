#include "sgp30/sgp30.h"
#include "pico/stdlib.h"

static i2c_inst_t *s_i2c_port;

// Plain i2c_write_blocking()/i2c_read_blocking() pass no timeout at all to
// the SDK's internal wait loop - if the bus is stuck (missing/broken pull-ups,
// a miswired sensor, or nothing on the bus ACKing), those calls spin forever
// with no way out, hanging all of main() (USB keeps working since that's
// interrupt-driven, but nothing in main() ever runs again, including any
// printf - total silence, easily mistaken for "nothing is flashed"). Every
// I2C call in this driver uses the *_timeout_us variants instead, so a bad
// bus produces a bounded failure (this function returns false) rather than
// an unrecoverable hang.
#define SGP30_I2C_TIMEOUT_US 50000

void sgp30_init(i2c_inst_t *i2c_port)
{
    s_i2c_port = i2c_port;
}

bool sgp30_iaq_init()
{
    static bool locked = false;
    if (locked)
        return false; // Prevent multiple initializations

    uint8_t init_iaq[2] = {0x20, 0x03}; // "init_iaq" command
    int rslt = i2c_write_timeout_us(s_i2c_port, SGP30_ADDR, init_iaq, 2, false, SGP30_I2C_TIMEOUT_US);
    if (rslt < 2)
        return false;

    sleep_ms(100); // Wait for the sensor to initialize
    locked = true; // Lock the initialization

    return true;
}

bool sgp30_measure_iaq(sgp30_measurement_t *out)
{
    uint8_t measure_iaq[2] = {0x20, 0x08}; // "measure_iaq" command
    int rslt = i2c_write_timeout_us(s_i2c_port, SGP30_ADDR, measure_iaq, 2, false, SGP30_I2C_TIMEOUT_US);
    if (rslt < 2)
        return false;

    sleep_ms(12); // max execution time for measure_iaq per the datasheet

    uint8_t raw[6];
    rslt = i2c_read_timeout_us(s_i2c_port, SGP30_ADDR, raw, 6, false, SGP30_I2C_TIMEOUT_US);
    if (rslt < 6)
        return false;

    // Check the CRC bytes
    if (raw[2] != sgp30_crc8(raw, 2) || raw[5] != sgp30_crc8(raw + 3, 2))
        return false;

    // Decode the measurement
    out->co2eq_ppm = ((uint16_t)raw[0] << 8) | raw[1];
    out->tvoc_ppb = ((uint16_t)raw[3] << 8) | raw[4];
    return true;
}

bool sgp30_get_iaq_baseline(sgp30_iaq_baseline_t *out)
{
    uint8_t iaq_baseline[2] = {0x20, 0x15}; // "get_iaq_baseline" command
    int rslt = i2c_write_timeout_us(s_i2c_port, SGP30_ADDR, iaq_baseline, 2, false, SGP30_I2C_TIMEOUT_US);
    if (rslt < 2)
        return false;

    sleep_ms(10); // Wait for the sensor to respond

    uint8_t raw[6];
    rslt = i2c_read_timeout_us(s_i2c_port, SGP30_ADDR, raw, 6, false, SGP30_I2C_TIMEOUT_US);
    if (rslt < 6)
        return false;

    // Check the CRC bytes
    if (raw[2] != sgp30_crc8(raw, 2) || raw[5] != sgp30_crc8(raw + 3, 2))
        return false;

    // Decode the baseline
    out->co2eq_ppm = ((uint16_t)raw[0] << 8) | raw[1];
    out->tvoc_ppb = ((uint16_t)raw[3] << 8) | raw[4];
    return true;
}
