#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"
#include "sgp30/sgp30.h"
#include "aht20/aht20.h"
#include "ssd1306/ssd1306.h"

// This example uses I2C0 on GPIO16 (SDA) and GPIO17 (SCL) running at 400kHz.
// Pins can be changed, see the GPIO function select table in the datasheet
// for information on GPIO assignments.
#define I2C_PORT i2c0
#define I2C_SDA 16
#define I2C_SCL 17

#define ADC_MQ135 26
const float adc_convertion_factor = 3.3f / (1 << 12);

#define PWR_HOLD_PIN 15
#define PWR_BTN_PIN 14
#define LEDS_PIN 13

int main()
{
    gpio_init(PWR_HOLD_PIN);
    gpio_init(PWR_BTN_PIN);
    gpio_init(LEDS_PIN);

    gpio_set_dir(PWR_BTN_PIN, GPIO_IN);
    gpio_set_dir(LEDS_PIN, GPIO_OUT);
    gpio_set_dir(PWR_HOLD_PIN, GPIO_OUT);

    gpio_put(PWR_HOLD_PIN, true);

    stdio_init_all(); // UART via USB for printf() output
    sleep_ms(1500);   // give the USB CDC time to enumerate before we print,
                      // otherwise the next several lines are commonly lost -
                      // this used to run after I2C setup, which meant the I2C
                      // bus scan below printed before the host had enumerated
                      // the CDC port and its output was silently dropped.

    // Printed first, always, so a stale/wrongly-targeted flash is
    // immediately obvious from the serial log instead of looking identical
    // to "nothing changed": __DATE__/__TIME__ are stamped in at compile
    // time, so this string only changes when this file is actually
    // recompiled and the resulting .uf2 actually reaches the board.
    printf("\n=== i2cDevices build %s %s ===\n", __DATE__, __TIME__);

    // Initialize ADC
    adc_init();
    adc_gpio_init(ADC_MQ135);
    adc_select_input(0);

    // Initialize I2C
    // 100kHz (not the usual 400kHz): confirmed by a logic analyzer capture
    // that AHT20 NACKed/timed out on every single transaction at 400kHz and
    // is clean on every transaction at 100kHz, so this stays until the
    // wiring/pull-ups are revisited.
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // TEMP DEBUG - remove once the SSD1306 is confirmed responding. A fresh
    // logic analyzer capture at 100kHz shows AHT20 (0x38) and SGP30 (0x58)
    // both ACKing cleanly on every transaction, but zero traffic at all at
    // 0x3C (SSD1306) - consistent with ssd1306_init() failing before it
    // ever gets past its first write. This scans every address and prints
    // whatever actually answers, to tell apart "wrong address" (e.g. some
    // modules are 0x3D, not 0x3C) from "nothing there at all" (wiring/dead
    // module).
    printf("\n--- I2C bus scan ---\n");
    for (uint8_t addr = 0x08; addr < 0x78; addr++)
    {
        uint8_t rxdata;
        int ret = i2c_read_timeout_us(I2C_PORT, addr, &rxdata, 1, false, 10000);
        if (ret >= 0)
            printf("  found device at 0x%02X\n", addr);
    }
    printf("--- scan done ---\n\n");

    sgp30_init(I2C_PORT);
    AHT20_init(I2C_PORT);
    bool oled_ok = ssd1306_init(I2C_PORT); // OLED shares the I2C0 bus with the sensors above
    if (!oled_ok)
        printf("SSD1306 init failed - check wiring/address\n");

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(I2C_SDA, I2C_SCL, GPIO_FUNC_I2C));

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
        // Redraw the whole screen from a blank, top-of-display cursor every
        // cycle instead of letting text accumulate/scroll across cycles -
        // keeps each sensor pinned to the same line every time, and avoids
        // the cursor drifting once the screen has scrolled once (see
        // ssd1306_newline() - once s_cursor_y hits the bottom line it stays
        // there, so later prints would land wherever the last cycle left
        // off instead of a known spot).
        if (oled_ok)
            ssd1306_clear();

        if (sgp30_measure_iaq(&m))
        {
            printf("SGP30:->  CO2eq=%u ppm  TVOC=%u ppb\n", m.co2eq_ppm, m.tvoc_ppb);
            if (oled_ok)
                ssd1306_printf("CO2:%u TVOC:%u\n", m.co2eq_ppm, m.tvoc_ppb);
        }

        if (AHT20_measure(&aht20_data) == AHT20_OK)
        {
            printf("AHT20:->  Temperature=%.2f C  Humidity=%.2f %%RH\n\n", aht20_data.temperature, aht20_data.r_humidity);
            if (oled_ok)
                ssd1306_printf("T:%.1fC H:%.1f%%\n", aht20_data.temperature, aht20_data.r_humidity);
        }

        uint16_t adc_rslt = adc_read();
        float mq135_v = adc_rslt * adc_convertion_factor;
        printf("MQ135:-> Volatage: %f V\n", mq135_v);
        if (oled_ok)
            ssd1306_printf("MQ135:%.2fV\n", mq135_v);

        if (gpio_get(PWR_BTN_PIN))
            gpio_put(PWR_HOLD_PIN, false);

        sleep_ms(1000);
    }
}
