#include <stdio.h>
#include "aht20/aht20.h"
#include "pico/stdlib.h"

///
#define DEBUG_LOG 0

/// @brief COMMANDs for AHT20
#define AHT20_CMD_MEASUREMENT 0xAC
#define AHT20_CMD_READ_STATUS 0x71
#define AHT20_CMD_INIT 0xBE
#define AHT20_CMD_RST 0xBA

#define AHT20_DELAY_MEASUREMENT 80
#define AHT20_DELAY_INIT 10

// Plain i2c_write_blocking()/i2c_read_blocking() pass no timeout at all to
// the SDK's internal wait loop - if the bus is stuck (missing/broken pull-ups,
// a miswired sensor, or nothing on the bus ACKing), those calls spin forever
// with no way out. Every I2C call in this driver uses the *_timeout_us
// variants instead, so a bad bus produces a bounded failure rather than an
// unrecoverable hang (same rationale as sgp30.c).
#define AHT20_I2C_TIMEOUT_US 50000

// AHT20_begin() polls the status register while the sensor calibrates.
// Bound the number of polls so a sensor that never reports itself
// calibrated fails the call instead of hanging forever.
#define AHT20_CAL_POLL_RETRIES 20
#define AHT20_CAL_POLL_DELAY_MS 40

#define GET_BIT(byte, index) ((byte & (1 << index)) >> index)

//////////////////////////////////////////////// GLOBAL_VARIABLE ////////////////////////////////////////////////

static i2c_inst_t *s_i2c_port;
static uint8_t s_data[7] = {0};
static uint8_t s_mode;
static bool s_calibrated;

//////////////////////////////////////////////// PRIVATE_DECLARATION ////////////////////////////////////////////////

AHT20_rst_t m_AHT20_command_init(void)
{
    uint8_t cmd[4] = {AHT20_CMD_INIT, 0x08, 0x00, 0x00};
    int rslt = i2c_write_timeout_us(s_i2c_port, AHT20_ADDR, cmd, 4, false, AHT20_I2C_TIMEOUT_US);
    if (rslt < 4)
        return AHT20_ERR;

    return AHT20_OK;
}

AHT20_rst_t m_AHT20_command_reset(void)
{
    int rslt = i2c_write_timeout_us(s_i2c_port, AHT20_ADDR, (uint8_t[1]){AHT20_CMD_RST}, 1, false, AHT20_I2C_TIMEOUT_US);
    if (rslt < 1)
        return AHT20_ERR;

    return AHT20_OK;
}

AHT20_rst_t m_AHT20_command_read_status(void)
{
    int rslt = i2c_write_timeout_us(s_i2c_port, AHT20_ADDR, (uint8_t[1]){AHT20_CMD_READ_STATUS}, 1, false, AHT20_I2C_TIMEOUT_US);
    if (rslt < 1)
        return AHT20_ERR;

    return AHT20_OK;
}

AHT20_rst_t m_AHT20_command_measure(void)
{
    uint8_t cmd[3] = {AHT20_CMD_MEASUREMENT, 0x33, 0x00};
    int rslt = i2c_write_timeout_us(s_i2c_port, AHT20_ADDR, cmd, 3, false, AHT20_I2C_TIMEOUT_US);
    if (rslt < 3)
        return AHT20_ERR;

    return AHT20_OK;
}

AHT20_rst_t m_AHT20_decode_status(AHT20_data_t *pAHT20_data)
{
    // bit[7] == 1: device is busy
    pAHT20_data->is_busy = GET_BIT(s_data[0], 7) == 1;

    // bit[3] == 1: device has been calibrated
    s_calibrated = GET_BIT(s_data[0], 3) == 1;

    // bit[6] == 1: CMD mode. Else bit[6] == 0 && bit[5] == 1: CYC mode.
    // Else (bit[6] == 0 && bit[5] == 0): NOR mode.
    if (GET_BIT(s_data[0], 6) == 1)
        s_mode = 2;
    else if (GET_BIT(s_data[0], 5) == 1)
        s_mode = 1;
    else
        s_mode = 0;

#if (DEBUG_LOG)
    printf("[AHT20] status: busy=%d cal=%d mode=%d bin=%d%d%d%d%d%d%d%d hex=%02x\n",
           pAHT20_data->is_busy, s_calibrated, s_mode,
           GET_BIT(s_data[0], 0), GET_BIT(s_data[0], 1), GET_BIT(s_data[0], 2), GET_BIT(s_data[0], 3),
           GET_BIT(s_data[0], 4), GET_BIT(s_data[0], 5), GET_BIT(s_data[0], 6), GET_BIT(s_data[0], 7), s_data[0]);
#endif

    return AHT20_OK;
}

AHT20_rst_t m_AHT20_get_result(AHT20_data_t *pAHT20_data, size_t len)
{
    (void)pAHT20_data;
    int rslt = i2c_read_timeout_us(s_i2c_port, AHT20_ADDR, s_data, len, false, AHT20_I2C_TIMEOUT_US);
    if (rslt < (int)len)
        return AHT20_ERR;

#if (DEBUG_LOG)
    printf("[AHT20] read %u bytes\n", (unsigned)len);
#endif
    return AHT20_OK;
}

//////////////////////////////////////////////// PUBLIC_DECLARATION ////////////////////////////////////////////////

void AHT20_init(i2c_inst_t *i2c_port)
{
    s_i2c_port = i2c_port;
}

AHT20_rst_t AHT20_measure(AHT20_data_t *pAHT20_data)
{
    uint32_t rh_raw, temp_raw;
    float rh, temp;

    if (m_AHT20_command_measure() != AHT20_OK)
        return AHT20_ERR;

    sleep_ms(AHT20_DELAY_MEASUREMENT); // wait for the measurement to complete

    if (m_AHT20_get_result(pAHT20_data, 7) != AHT20_OK)
        return AHT20_ERR;

    m_AHT20_decode_status(pAHT20_data);
    if (pAHT20_data->is_busy)
    {
        printf("[AHT20] measure: device still busy\n");
        return AHT20_ERR;
    }

    rh_raw = (((uint32_t)s_data[1] << 16) | ((uint32_t)s_data[2] << 8) | (s_data[3])) >> 4;
    temp_raw = ((uint32_t)(s_data[3] & 0x0F) << 16) | ((uint32_t)s_data[4] << 8) | (uint32_t)s_data[5];

    rh = (rh_raw * (0.0000953674316F));
    temp = (temp_raw * (0.00019073F) - 50);

    if (rh > 100 || rh < 0 || temp > 120 || temp < -40)
    {
        printf("[AHT20] measure: reading out of bounds rh=%f temp=%f\n", rh, temp);
        return AHT20_ERR;
    }

    pAHT20_data->r_humidity = rh;
    pAHT20_data->temperature = temp;

    return AHT20_OK;
}

AHT20_rst_t AHT20_begin(AHT20_data_t *pAHT20_data)
{
    sleep_ms(40); // wait for the device to power up
    m_AHT20_command_reset();
    sleep_ms(40); // wait for the reset to take effect

    m_AHT20_command_read_status();
    m_AHT20_get_result(pAHT20_data, 1);
    m_AHT20_decode_status(pAHT20_data);
    printf("[AHT20] begin: calibrated=%d\n", s_calibrated);

    if (s_calibrated)
        return AHT20_OK;

    printf("[AHT20] begin: sending init\n");
    m_AHT20_command_init();
    sleep_ms(AHT20_DELAY_INIT); // wait for the device to initialize

    for (int retry = 0; !s_calibrated && retry < AHT20_CAL_POLL_RETRIES; retry++)
    {
        sleep_ms(AHT20_CAL_POLL_DELAY_MS); // wait for the device to be ready
        m_AHT20_command_read_status();
        m_AHT20_get_result(pAHT20_data, 1);
        m_AHT20_decode_status(pAHT20_data);
        printf("[AHT20] begin: calibrated=%d\n", s_calibrated);
    }

    return s_calibrated ? AHT20_OK : AHT20_ERR;
}
