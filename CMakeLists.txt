cmake_minimum_required(VERSION 3.13)

# Pull in the SDK (your in-tree pico-sdk submodule)
include(pico-sdk/pico_sdk_init.cmake)

project(rp2350 C CXX ASM)

# Initialise the SDK. PICO_BOARD=pico2 (passed on the cmake command line, or set
# here BEFORE this call) selects PICO_PLATFORM=rp2350 and the Cortex-M33 toolchain.
pico_sdk_init()

add_executable(rp2350)

target_sources(rp2350 PRIVATE
    blink.c
    hid-device.c          # your USB HID descriptors / tud_* callbacks
)
# NOTE: ws2812.pio.h / debounce.pio.h are generated headers, not sources.
# Generate them with pico_generate_pio_header instead of listing the .h in target_sources:
#   pico_generate_pio_header(rp2350 ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
#   pico_generate_pio_header(rp2350 ${CMAKE_CURRENT_LIST_DIR}/debounce.pio)

# Only project-specific defines remain. Do NOT set CFG_TUSB_MCU/CFG_TUSB_OS here;
# the SDK's tinyusb integration sets CFG_TUSB_MCU=OPT_MCU_RP2040 and CFG_TUSB_OS=OPT_OS_PICO.
target_compile_definitions(rp2350 PRIVATE
    MODE=1
)

# tusb_config.h lives in include/ (or repo root); make sure TinyUSB can find it.
target_include_directories(rp2350 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Link the SDK's TinyUSB device stack + board support. NO manual tinyusb/src/*.c.
target_link_libraries(rp2350 PRIVATE
    pico_stdlib
    tinyusb_device
    tinyusb_board
    hardware_gpio
    hardware_pio
    hardware_i2c
    hardware_pwm
)

pico_add_extra_outputs(rp2350)
