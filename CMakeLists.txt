cmake_minimum_required(VERSION 3.13...3.27)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(rp2350)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(blink)

pico_generate_pio_header(blink ${CMAKE_CURRENT_SOURCE_DIR}/ws2812.pio)
pico_generate_pio_header(blink ${CMAKE_CURRENT_SOURCE_DIR}/debounce.pio)

set(MODE "SCROLLWHEEL" CACHE STRING "Mode: SCROLLWHEEL (default) or VOLUME")
set_property(CACHE MODE PROPERTY STRINGS SCROLLWHEEL VOLUME)

target_compile_definitions(blink PRIVATE
	${MODE}=1
	CFG_TUSB_MCU=OPT_MCU_RP2350
	CFG_TUSB_OS=OPT_OS_PICO
	CFG_TUSB_DEBUG=0)

target_sources(blink PRIVATE
	blink.c
	hid-device.c
	ws2812.pio.h
	debounce.pio.h
	tinyusb/src/tusb.c
	tinyusb/src/common/tusb_fifo.c
	tinyusb/src/device/usbd.c
	tinyusb/src/class/hid/hid_device.c
	tinyusb/src/portable/raspberrypi/rp2350/rp2350_usb.c
	tinyusb/src/portable/raspberrypi/rp2350/dcd_rp2350.c)

target_link_libraries(blink pico_stdlib
	hardware_gpio
	hardware_pio
	hardware_i2c
	hardware_pwm)

target_include_directories(blink PRIVATE
	include
	tinyusb/src)

#
# We really don't care about IEEE precision, nor
# do we want double-precision math just because
# of some random constant (and adding ".0f" to them
# everywhere is just cray-cray)
#
# Add -fsingle-precision-constant / -Wdouble-promotion
# when starting to do FP for audio
#
target_compile_options(blink PRIVATE
	-Wall -O2
	-ffast-math
	-fsingle-precision-constant
	-Wfloat-conversion)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(blink)
