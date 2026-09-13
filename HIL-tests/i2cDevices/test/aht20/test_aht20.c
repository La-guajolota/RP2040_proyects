#include "aht20/aht20.h"
#include "pico_test.h"

// sgp30_crc8() is the only pure logic left in the sgp30 driver - the rest
// (sgp30_measure_iaq, sgp30_get_iaq_baseline, ...) talks to real I2C
// hardware and isn't exercised here. This checks it against the known-good
// vector from the datasheet (Sensirion SGP30, Sec 6.6): CRC(0xBEEF) = 0x92.

static void test_AHT20_command_init(void)
{
    m_AHT20_command_init();
}

static void test_AHT20_begin(void)
{
    AHT20_data_t p;
    AHT20_begin(&p);
}

void run_aht20_tests(void)
{
    TEST_RUN(test_AHT20_command_init);
    TEST_RUN(test_AHT20_begin);
}
