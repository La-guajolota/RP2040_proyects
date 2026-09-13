#ifndef PICO_TEST_H
#define PICO_TEST_H

// Tiny on-target unit test harness: no external dependency, prints
// PASS/FAIL over the same stdio the app uses (USB CDC here), so results
// show up in any serial monitor after flashing the *_tests.uf2 firmware.

#include <stdio.h>

static int pico_test_count = 0;
static int pico_test_failures = 0;

#define TEST_ASSERT_TRUE(cond)                                              \
    do                                                                      \
    {                                                                       \
        pico_test_count++;                                                 \
        if (!(cond))                                                       \
        {                                                                   \
            pico_test_failures++;                                          \
            printf("  FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond);        \
        }                                                                   \
    } while (0)

#define TEST_ASSERT_EQUAL_UINT16(expected, actual)                          \
    do                                                                      \
    {                                                                       \
        pico_test_count++;                                                 \
        uint16_t _e = (expected);                                          \
        uint16_t _a = (actual);                                            \
        if (_e != _a)                                                      \
        {                                                                   \
            pico_test_failures++;                                          \
            printf("  FAIL %s:%d  expected %u got %u\n",                    \
                   __FILE__, __LINE__, (unsigned)_e, (unsigned)_a);          \
        }                                                                   \
    } while (0)

#define TEST_RUN(fn)                                                        \
    do                                                                      \
    {                                                                       \
        printf("-- %s --\n", #fn);                                         \
        fn();                                                              \
    } while (0)

static inline int pico_test_summary(void)
{
    printf("\n%d tests, %d failed\n", pico_test_count, pico_test_failures);
    printf(pico_test_failures == 0 ? "ALL TESTS PASSED\n" : "TESTS FAILED\n");
    return pico_test_failures == 0 ? 0 : 1;
}

#endif // PICO_TEST_H
