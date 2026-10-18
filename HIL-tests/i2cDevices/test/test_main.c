#include "pico/stdlib.h"
#include "pico_test.h"
#include "aht20/aht20.h"
#include "ssd1306/ssd1306.h"

// Same I2C wiring as src/main.c (I2C0 on GPIO16/17) - the aht20 tests are
// HIL tests that talk to the real sensor, so the bus needs to be brought up
// here too, not just in the app.
#define I2C_PORT i2c0
#define I2C_SDA 16
#define I2C_SCL 17

void run_sgp30_tests(void);
void run_aht20_tests(void);
void run_ssd1306_tests(void);
void i2c_bus_scan(uint8_t times);

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr)
{
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main(void)
{
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    AHT20_init(I2C_PORT);
    if (!ssd1306_init(I2C_PORT))
        printf("SSD1306 init failed - check wiring/address\n");

    sleep_ms(2000); // give the USB CDC time to enumerate before we print

    run_sgp30_tests();
    run_aht20_tests();
    run_ssd1306_tests();
    i2c_bus_scan(5);

    int failures = pico_test_summary();

    // Blink the LED fast on failure, slow on success, so the result is
    // visible even without a serial monitor attached.
    while (1)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(failures ? 100 : 800);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(failures ? 100 : 800);
    }
}

void i2c_bus_scan(uint8_t times)
{
    for (int num_test = 0; num_test < times; num_test++)
    {
        printf("\nI2C Bus Scan #%d\n", times);
        printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

        for (int addr = 0; addr < (1 << 7); ++addr)
        {
            if (addr % 16 == 0)
            {
                printf("%02x ", addr);
            }

            // Perform a 1-byte dummy read from the probe address. If a slave
            // acknowledges this address, the function returns the number of bytes
            // transferred. If the address byte is ignored, the function returns
            // -1.

            // Skip over any reserved addresses.
            int ret;
            uint8_t rxdata;
            if (reserved_addr(addr))
                ret = PICO_ERROR_GENERIC;
            else
                ret = i2c_read_blocking(I2C_PORT, addr, &rxdata, 1, false);

            printf(ret < 0 ? "." : "@");
            printf(addr % 16 == 15 ? "\n" : "  ");
        }
        printf("Done.\n");
    }
}