#include <string.h>
#include "ssd1306/ssd1306.h"
#include "ssd1306/ssd1306_font.h"
#include "pico_test.h"

// Unlike aht20/sgp30, ssd1306_print()/ssd1306_printf() are pure text-layout
// logic (ssd1306_putc/ssd1306_newline) sitting on top of a real I2C flush -
// and it's exactly that layout logic (cursor advance, line wrap, scroll)
// that caused the multi-line rendering bug this driver shipped with. These
// tests exercise it directly through the ssd1306_test_*() accessors in
// ssd1306.h, so a regression there fails here instead of only showing up as
// "the display looks wrong" on real hardware.
//
// ssd1306_print() still calls ssd1306_flush() every time (test_main.c's
// ssd1306_init() call made that safe even if the display itself doesn't
// ACK), so these also incidentally exercise the real I2C write path.

static const uint8_t glyph_A[SSD1306_FONT_WIDTH] = {0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C};
static const uint8_t glyph_B[SSD1306_FONT_WIDTH] = {0x00, 0x7F, 0x49, 0x49, 0x49, 0x36};
static const uint8_t glyph_question[SSD1306_FONT_WIDTH] = {0x00, 0x02, 0x01, 0x51, 0x09, 0x06};

static void test_font_table_size(void)
{
    size_t expected_glyphs = SSD1306_FONT_LAST_CHAR - SSD1306_FONT_FIRST_CHAR + 1;
    TEST_ASSERT_TRUE(sizeof(ssd1306_font6x8) == expected_glyphs * SSD1306_FONT_WIDTH);
}

static void test_single_char_draws_at_origin(void)
{
    ssd1306_clear();
    ssd1306_print("A");

    TEST_ASSERT_TRUE(memcmp(ssd1306_test_framebuffer(), glyph_A, SSD1306_FONT_WIDTH) == 0);

    int x, y;
    ssd1306_test_cursor(&x, &y);
    TEST_ASSERT_TRUE(x == SSD1306_FONT_WIDTH + 1);
    TEST_ASSERT_TRUE(y == 0);
}

static void test_newline_moves_to_next_page(void)
{
    ssd1306_clear();
    ssd1306_print("A\nB");

    const uint8_t *fb = ssd1306_test_framebuffer();
    TEST_ASSERT_TRUE(memcmp(fb + SSD1306_WIDTH, glyph_B, SSD1306_FONT_WIDTH) == 0);

    int x, y;
    ssd1306_test_cursor(&x, &y);
    TEST_ASSERT_TRUE(x == SSD1306_FONT_WIDTH + 1);
    TEST_ASSERT_TRUE(y == 8);
}

static void test_unprintable_char_becomes_question_mark(void)
{
    ssd1306_clear();
    ssd1306_print("\x01");

    TEST_ASSERT_TRUE(memcmp(ssd1306_test_framebuffer(), glyph_question, SSD1306_FONT_WIDTH) == 0);
}

static void test_line_wraps_without_explicit_newline(void)
{
    // 128px wide / 7px per char (6px glyph + 1px gap) = 18 chars fit on one
    // line (18*7=126px); the 19th no longer fits (126+6 > 128) and should
    // wrap to the start of the next page instead of drawing off-screen.
    char line[20];
    memset(line, 'A', 19);
    line[19] = '\0';

    ssd1306_clear();
    ssd1306_print(line);

    const uint8_t *fb = ssd1306_test_framebuffer();
    TEST_ASSERT_TRUE(memcmp(fb + SSD1306_WIDTH, glyph_A, SSD1306_FONT_WIDTH) == 0);

    int x, y;
    ssd1306_test_cursor(&x, &y);
    TEST_ASSERT_TRUE(x == SSD1306_FONT_WIDTH + 1);
    TEST_ASSERT_TRUE(y == 8);
}

static void test_scroll_shifts_lines_up_and_pins_cursor(void)
{
    // Fill all 4 lines (32px / 8px per line) - exactly full, no scroll yet.
    ssd1306_clear();
    ssd1306_print("1\n2\n3\n4\n");

    int x, y;
    ssd1306_test_cursor(&x, &y);
    TEST_ASSERT_TRUE(y == 24); // sitting on the last line, not yet scrolled

    uint8_t line2_before[SSD1306_WIDTH];
    memcpy(line2_before, ssd1306_test_framebuffer() + SSD1306_WIDTH, SSD1306_WIDTH);

    // One more line overflows the screen and should scroll everything up
    // by one line - what used to be line "2" (page 1) should now be at the
    // top (page 0).
    ssd1306_print("5\n");

    TEST_ASSERT_TRUE(memcmp(ssd1306_test_framebuffer(), line2_before, SSD1306_WIDTH) == 0);

    // The cursor stays pinned on the bottom line rather than returning to
    // the top - documented/current behaviour (src/main.c works around this
    // by calling ssd1306_clear() itself once per loop rather than relying
    // on scrolling across independent print calls).
    ssd1306_test_cursor(&x, &y);
    TEST_ASSERT_TRUE(y == 24);
}

void run_ssd1306_tests(void)
{
    TEST_RUN(test_font_table_size);
    TEST_RUN(test_single_char_draws_at_origin);
    TEST_RUN(test_newline_moves_to_next_page);
    TEST_RUN(test_unprintable_char_becomes_question_mark);
    TEST_RUN(test_line_wraps_without_explicit_newline);
    TEST_RUN(test_scroll_shifts_lines_up_and_pins_cursor);
}
