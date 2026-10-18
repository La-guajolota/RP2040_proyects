#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "ssd1306/ssd1306.h"
#include "ssd1306/ssd1306_font.h"

// Same rationale as aht20.c/sgp30.c: bound every I2C call so a stuck bus
// (missing pull-ups, nothing ACKing at SSD1306_ADDR) fails this call
// instead of hanging forever.
#define SSD1306_I2C_TIMEOUT_US 50000

// Co=0, D/C#=0: every following byte is a command.
#define SSD1306_CTRL_CMD 0x00
// Co=0, D/C#=1: every following byte is framebuffer data.
#define SSD1306_CTRL_DATA 0x40

#define SSD1306_PAGES (SSD1306_HEIGHT / 8)
#define SSD1306_CHARS_PER_LINE (SSD1306_WIDTH / (SSD1306_FONT_WIDTH + 1))

//////////////////////////////////////////////// GLOBAL_VARIABLE ////////////////////////////////////////////////

static i2c_inst_t *s_i2c_port;
static uint8_t s_fb[SSD1306_FRAMEBUFFER_SIZE];
static int s_cursor_x;
static int s_cursor_y;

//////////////////////////////////////////////// PRIVATE_DECLARATION ////////////////////////////////////////////////

static bool ssd1306_write_ctrl(uint8_t ctrl, const uint8_t *payload, size_t len)
{
    uint8_t buf[1 + SSD1306_WIDTH];

    if (len > SSD1306_WIDTH)
        return false;

    buf[0] = ctrl;
    if (len > 0)
        memcpy(&buf[1], payload, len);

    int rslt = i2c_write_timeout_us(s_i2c_port, SSD1306_ADDR, buf, 1 + len, false, SSD1306_I2C_TIMEOUT_US);
    return rslt == (int)(1 + len);
}

static bool ssd1306_write_cmd(const uint8_t *cmds, size_t len)
{
    return ssd1306_write_ctrl(SSD1306_CTRL_CMD, cmds, len);
}

static bool ssd1306_set_address_window(void)
{
    static const uint8_t cmds[] = {
        0x21,
        0x00,
        SSD1306_WIDTH - 1, // column start/end
        0x22,
        0x00,
        SSD1306_PAGES - 1, // page start/end
    };

    return ssd1306_write_cmd(cmds, sizeof(cmds));
}

// Advances the cursor to the start of the next line, scrolling the whole
// framebuffer up by one text line once the last line is full - the same
// behaviour a serial terminal has once the screen fills up.
static void ssd1306_newline(void)
{
    s_cursor_x = 0;

    if (s_cursor_y + SSD1306_FONT_HEIGHT >= SSD1306_HEIGHT)
    {
        memmove(s_fb, s_fb + SSD1306_WIDTH, SSD1306_FRAMEBUFFER_SIZE - SSD1306_WIDTH);
        memset(s_fb + SSD1306_FRAMEBUFFER_SIZE - SSD1306_WIDTH, 0x00, SSD1306_WIDTH);
    }
    else
    {
        s_cursor_y += SSD1306_FONT_HEIGHT;
    }
}

// s_cursor_y is always a multiple of SSD1306_FONT_HEIGHT (8), i.e. always
// page-aligned, so a glyph column can be copied straight into its
// framebuffer page byte with no per-pixel bit shifting.
static void ssd1306_putc(char c)
{
    if (c == '\n')
    {
        ssd1306_newline();
        return;
    }
    if (c == '\r')
        return;

    if (c < SSD1306_FONT_FIRST_CHAR || c > SSD1306_FONT_LAST_CHAR)
        c = '?';

    if (s_cursor_x + SSD1306_FONT_WIDTH > SSD1306_WIDTH)
        ssd1306_newline();

    const uint8_t *glyph = &ssd1306_font6x8[(c - SSD1306_FONT_FIRST_CHAR) * SSD1306_FONT_WIDTH];
    uint8_t *dst = &s_fb[(s_cursor_y / 8) * SSD1306_WIDTH + s_cursor_x];
    memcpy(dst, glyph, SSD1306_FONT_WIDTH);

    s_cursor_x += SSD1306_FONT_WIDTH + 1; // +1px spacing between characters
}

//////////////////////////////////////////////// PUBLIC_API ////////////////////////////////////////////////

bool ssd1306_init(i2c_inst_t *i2c_port)
{
    s_i2c_port = i2c_port;

    static const uint8_t init_seq[] = {
        0xAE, // display off
        0xD5,
        0x80, // clock divide ratio / oscillator freq
        0xA8,
        SSD1306_HEIGHT - 1, // multiplex ratio
        0xD3,
        0x00, // display offset
        0x40, // display start line = 0
        0x8D,
        0x14, // charge pump enable
        0x20,
        0x00, // memory addressing mode = horizontal
        0xA1, // segment remap (mirror X)
        0xC8, // COM scan direction decrement (mirror Y)
        0xDA,
        0x02, // COM pins config (128x32 panels)
        0x81,
        0xCF, // contrast
        0xD9,
        0xF1, // pre-charge period
        0xDB,
        0x40, // VCOMH deselect level
        0xA4, // resume RAM content display
        0xA6, // normal (not inverted) display
        0xAF, // display on
    };

    if (!ssd1306_write_cmd(init_seq, sizeof(init_seq)))
        return false;

    ssd1306_clear();
    return ssd1306_flush();
}

void ssd1306_clear(void)
{
    memset(s_fb, 0x00, sizeof(s_fb));
    s_cursor_x = 0;
    s_cursor_y = 0;
}

bool ssd1306_flush(void)
{
    if (!ssd1306_set_address_window())
        return false;

    for (int page = 0; page < SSD1306_PAGES; page++)
    {
        if (!ssd1306_write_ctrl(SSD1306_CTRL_DATA, &s_fb[page * SSD1306_WIDTH], SSD1306_WIDTH))
            return false;
    }

    return true;
}

void ssd1306_print(const char *str)
{
    for (const char *p = str; *p != '\0'; p++)
        ssd1306_putc(*p);

    // TEMP DEBUG - remove once the multi-line rendering bug is found.
    // Reports what's actually in the framebuffer and whether the I2C write
    // itself succeeded, so we can tell apart "never drew the glyphs" from
    // "drew them fine but the display never got/showed the bytes".
    for (int page = 0; page < SSD1306_PAGES; page++)
    {
        int nonzero = 0;
        for (int i = 0; i < SSD1306_WIDTH; i++)
            if (s_fb[page * SSD1306_WIDTH + i] != 0)
                nonzero++;
        // printf("[ssd1306] page%d nonzero_bytes=%d\n", page, nonzero);
    }

    if (!ssd1306_flush())
        printf("[ssd1306] flush FAILED (I2C write error)\n");
}

void ssd1306_printf(const char *fmt, ...)
{
    char buf[SSD1306_CHARS_PER_LINE * SSD1306_PAGES + 1];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    ssd1306_print(buf);
}

const uint8_t *ssd1306_test_framebuffer(void)
{
    return s_fb;
}

void ssd1306_test_cursor(int *x, int *y)
{
    *x = s_cursor_x;
    *y = s_cursor_y;
}
