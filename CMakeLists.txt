cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(rp2350 C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(blink)

pico_generate_pio_header(blink ${CMAKE_CURRENT_SOURCE_DIR}/ws2812.pio)
pico_generate_pio_header(blink ${CMAKE_CURRENT_SOURCE_DIR}/debounce.pio)

set(MODE "SCROLLWHEEL" CACHE STRING "Mode: SCROLLWHEEL (default) or VOLUME")
set_property(CACHE MODE PROPERTY STRINGS SCROLLWHEEL VOLUME)

target_compile_definitions(blink PRIVATE
	${MODE}=1)

target_sources(blink PRIVATE
	blink.c
	hid-device.c)

target_link_libraries(blink
	pico_stdlib
	tinyusb_device
	tinyusb_board
	hardware_gpio
	hardware_pio
	hardware_i2c
	hardware_pwm)

target_include_directories(blink PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	include)


#
# We really don't care about IEEE precision, nor
# do we want double-precision math just because
# of some random constant
#
target_compile_options(blink PRIVATE
	-Wall -O2
	-ffast-math
	-fsingle-precision-constant
	-Wfloat-conversion)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(blink)
