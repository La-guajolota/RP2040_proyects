# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(i2cDevices C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
# Sources live in src/, public headers for our own modules/drivers in
# include/ (mirrors the PlatformIO src/include/test layout). main.c stays
# directly under src/; each module gets its own src/<name>/ + include/<name>/
# pair (e.g. src/sgp30/sgp30.c + include/sgp30/sgp30.h) - just add the
# directory and it's picked up automatically, no CMakeLists.txt edit needed.

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/*.c)

add_executable(i2cDevices ${APP_SOURCES})

pico_set_program_name(i2cDevices "i2cDevices")
pico_set_program_version(i2cDevices "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(i2cDevices 0)
pico_enable_stdio_usb(i2cDevices 1)

# Add the standard library to the build
target_link_libraries(i2cDevices
        pico_stdlib)

# Add our own module/driver headers to the build
target_include_directories(i2cDevices PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include
)

# Add any user requested libraries
target_link_libraries(i2cDevices
        hardware_i2c
        hardware_timer
        hardware_adc
        )

pico_add_extra_outputs(i2cDevices)

# Print FLASH/RAM usage vs the board's actual capacity after every link
# (shows up right in the "Compile Project" task output).
target_link_options(i2cDevices PRIVATE -Wl,--print-memory-usage)

# On-target unit tests (test/) build into a separate i2cDevices_tests.uf2 -
# flash it in place of the app to run the tests and read PASS/FAIL over
# USB serial. Set -DBUILD_TESTS=OFF to skip building it.
option(BUILD_TESTS "Build the on-target unit test firmware" ON)
if(BUILD_TESTS)
    add_subdirectory(test)
endif()

