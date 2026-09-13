#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"
#include "sgp30/sgp30.h"
#include "aht20/aht20.h"

// This example uses I2C0 on GPIO16 (SDA) and GPIO17 (SCL) running at 400kHz.
// Pins can be changed, see the GPIO function select table in the datasheet
// for information on GPIO assignments.
#define I2C_PORT i2c0
#define I2C_SDA 16
#define I2C_SCL 17

#define ADC_MQ135 26
const float adc_convertion_factor = 3.3f / (1 << 12);

int main()
{
    stdio_init_all(); // UART via USB for printf() output

    // Initialize ADC
    adc_init();
    adc_gpio_init(ADC_MQ135);
    adc_select_input(0);

    // Initialize I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    sgp30_init(I2C_PORT);
    AHT20_init(I2C_PORT);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(I2C_SDA, I2C_SCL, GPIO_FUNC_I2C));

    sleep_ms(1500); // give the USB CDC time to enumerate before we print,
                    // otherwise this first line is commonly lost

    AHT20_data_t aht20_data;
    if (AHT20_begin(&aht20_data) != AHT20_OK)
        printf("AHT20 begin failed - check wiring/power\n");

    sgp30_measurement_t m;
    sgp30_iaq_baseline_t b;
    sgp30_iaq_init();
    if (sgp30_get_iaq_baseline(&b))
    {
        printf("Baseline CO2eq=%u ppm  TVOC=%u ppb\n", b.co2eq_ppm, b.tvoc_ppb);
    }

    while (1)
    {
        if (sgp30_measure_iaq(&m))
            printf("SGP30:->  CO2eq=%u ppm  TVOC=%u ppb\n", m.co2eq_ppm, m.tvoc_ppb);

        if (AHT20_measure(&aht20_data) == AHT20_OK)
            printf("AHT20:->  Temperature=%.2f C  Humidity=%.2f %%RH\n\n", aht20_data.temperature, aht20_data.r_humidity);

        uint16_t adc_rslt = adc_read();
        printf("MQ135:-> Volatage: %f V\n", adc_rslt * adc_convertion_factor);

        uint8_t dummy = 0;
        dummy = i2c_read_blocking(I2C_PORT, 0x3C, &dummy, 1, false);
        if (dummy < 1)
            printf("LCD display found\n");

        sleep_ms(1000);
    }
}
