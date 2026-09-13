# On-target unit tests: a second .uf2 you flash separately from the app.
# Test entry point + shared harness (test_main.c, pico_test.h) stay
# directly under test/, mirroring src/main.c. Each module gets its own
# test/<name>/ folder (e.g. test/sgp30/test_sgp30.c) alongside its
# src/<name>/ + include/<name>/ pair - add the directory and both the
# module source and its test are picked up automatically below, no
# CMakeLists.txt edit needed.
#
# Only src/*/*.c (one level deep, i.e. module folders) is linked in here,
# never src/main.c, so there is no clash with the app's own main().

file(GLOB_RECURSE TEST_MODULE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/*/*.c)
file(GLOB APP_MODULE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/../src/*/*.c)

add_executable(${PROJECT_NAME}_tests
        test_main.c
        ${TEST_MODULE_SOURCES}
        ${APP_MODULE_SOURCES}
)

target_include_directories(${PROJECT_NAME}_tests PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../include
)

target_link_libraries(${PROJECT_NAME}_tests
        pico_stdlib
        hardware_i2c
        hardware_adc
        )

pico_enable_stdio_uart(${PROJECT_NAME}_tests 0)
pico_enable_stdio_usb(${PROJECT_NAME}_tests 1)

pico_add_extra_outputs(${PROJECT_NAME}_tests)

target_link_options(${PROJECT_NAME}_tests PRIVATE -Wl,--print-memory-usage)
