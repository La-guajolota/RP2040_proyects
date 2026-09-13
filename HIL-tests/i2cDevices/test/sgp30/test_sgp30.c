#include "sgp30/sgp30_crc_table.h"
#include "pico_test.h"

// sgp30_crc8() is the only pure logic left in the sgp30 driver - the rest
// (sgp30_measure_iaq, sgp30_get_iaq_baseline, ...) talks to real I2C
// hardware and isn't exercised here. This checks it against the known-good
// vector from the datasheet (Sensirion SGP30, Sec 6.6): CRC(0xBEEF) = 0x92.

static void test_crc8_datasheet_vector(void)
{
    uint8_t word[2] = {0xBE, 0xEF};
    TEST_ASSERT_TRUE(sgp30_crc8(word, 2) == 0x92);
}

static void test_crc8_zero(void)
{
    uint8_t word[2] = {0x00, 0x00};
    TEST_ASSERT_TRUE(sgp30_crc8(word, 2) == 0x81);
}

void run_sgp30_tests(void)
{
    TEST_RUN(test_crc8_datasheet_vector);
    TEST_RUN(test_crc8_zero);
}
