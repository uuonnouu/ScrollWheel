#include "pico/stdlib.h"
#include "pico/bootrom.h"

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"
#include "led_pwm.h"
#include "debounce.pio.h"

#include "board_tiny2350.h"
#include "tusb.h"
#include "hid-device.h"

	ws2812_program_init(pio, sm, offset, WS2812_PIN);
}

// Initialize a pin for input, pulled up
static void init_sw_pin(PIO pio, int pin)
{
	gpio_init(pin);
	gpio_set_dir(pin, false);
	gpio_pull_up(pin);
	pio_gpio_init(pio, pin);
}

// We use PIO1 for the SW pins.
//
// They share the same program, just a separate state machine
// for each pin.
static void init_sw_pins(void)
{
	PIO pio = pio1;
	uint offset = pio_add_program(pio, &debounce_program);

	init_sw_pin(pio, GPIO_SW1);
	init_sw_pin(pio, GPIO_SW2);
	init_sw_pin(pio, GPIO_SW3);
	init_sw_pin(pio, GPIO_SW4);

	// We use the same PIO program for all SW pins
	// just with different state machines
	debounce_program_init(pio, 0, offset, GPIO_SW1);
	debounce_program_init(pio, 1, offset, GPIO_SW2);
	debounce_program_init(pio, 2, offset, GPIO_SW3);
	debounce_program_init(pio, 3, offset, GPIO_SW4);
;  
}

static void write_LED(uint32_t value)
{
	pio_sm_put_blocking(pio0, 0, value);
}

static void init_i2c(void)
{
	i2c_init(I2C_BUS, 100 * 1000);
	gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
	gpio_pull_up(I2C_SDA);
	gpio_pull_up(I2C_SCL);
}

//
// The AS5600 has a status register at address 0x0B,
// and the raw angle is in the two next bytes
//
// as5600_read_angle() returns 12 bits of angle data, or
// -1 for a bad read
//
#define AS5600_I2C_ADDR 0x36
static int as5600_read(uint8_t reg, uint8_t buf[], size_t size)
{
	if (i2c_write_blocking(I2C_BUS, AS5600_I2C_ADDR, &reg, 1, false) < 0)
		return -1;
	if (i2c_read_blocking(I2C_BUS, AS5600_I2C_ADDR, buf, size, false) != size)
		return -1;
	return 0;
}

#define AS5600_STATUS		0x0B
#define AS5600_RAWANGLE_HI	0x0C
#define AS5600_RAWANGLE_LO	0x0D
#define AS5600_CONF		0x07

// Set hysteresis and filtering to max, we do not
// need fast response, we're better off stable
static int as5600_init(void)
{
	uint8_t rxdata[2], txdata[3];

	if (as5600_read(AS5600_STATUS, rxdata, 2) < 0)
		return -1;

	txdata[0] = AS5600_STATUS;
	// WD = 0 FTH = 001 SF = 0
	txdata[1] = (rxdata[0] & 0b11000000) | 0b00000100;
	// HYST = 11 PM = 00
	txdata[2] = (rxdata[1] & 0b11110000) | 0b00001100;

	// No change?
	if (txdata[1] == rxdata[0] && txdata[2] == rxdata[1])
		return 0;

	if (i2c_write_blocking(I2C_BUS, AS5600_I2C_ADDR, txdata, 3, false) != 3)
		return -1;

	return 0;
}

static int as5600_read_angle(void)
{
	uint8_t rxdata[3] = { 0, 0, 0 };

	if (as5600_read(AS5600_STATUS, rxdata, 3) < 0)
		return -1;

	// The MD bit ("magnet detected") needs to be set for good angle measurements
	if (!(rxdata[0] & 0b00100000))
		return -1;

	// ML/MH (magnet field low/high) bits should optimally be clear
	// but apparently my magnets suck
#if 0
	if (rxdata[0] & 0b00011000)
		return -1;
#endif

	return ((rxdata[1] & 15) << 8) + rxdata[2];
}

static int read_angle(void)
{
	static char initialized = 0;
	static int reset_angle, last_angle;

	int angle = as5600_read_angle();
	if (angle < 0)
		return last_angle;

	if (!initialized) {
		reset_angle = angle;
		initialized = 1;
		return 0;
	}

	angle = (angle - reset_angle) & 0xfff;

	// Signed difference in 12 bits relative to last
	int diff = (int) (angle - last_angle) << 20 >> 20;

	// Add that offset to the last_angle, so that we
	// have an angle that can do multiple rotations
	last_angle += diff;

	return last_angle;
}

//
// Treat the angle as a fixed-point
// fraction.
//
// Multiply by 3 to get the byte
// index in the integer part (0..2)
// and then take the 5 MSB of the
// mantissa and make it the color.
//
uint32_t led_color(uint32_t angle)
{
	angle = (angle & 0xfff) * 3;

	int byteidx = angle >> 12;
	int colorval = (angle >> 7) & 31;

	// Move the color and its inverse
	// into adjacent bytes to make a
	// color wheel and shift it by
	// the byte index to change the
	// color mix. The "<< 24" is the
	// "rotated" case.
	uint32_t color = colorval << 8;
	colorval ^= 31;
	color += colorval + (colorval << 24);

	return color << (byteidx * 8);
}

static void process_buttons(int sw1, int sw2, int sw3, int sw4)
{
	static int last_sw1 = 0;
	static int last_sw2 = 0;
	static int last_sw3 = 0;
	static int last_sw4 = 0;
	uint8_t keycodes[6] = {0}, *p = keycodes;

	if (sw1 == last_sw1 &&
	    sw2 == last_sw2 &&
	    sw3 == last_sw3 &&
	    sw4 == last_sw4)
		return;

	last_sw1 = sw1;
	last_sw2 = sw2;
	last_sw3 = sw3;
	last_sw4 = sw4;

	// Send F1/F2/F3/F4 for the foot switches (F5/F6/F7/F8 for longpress)
	if (sw1) *p++ = HID_KEY_F1 + (sw1-1)*4;
	if (sw2) *p++ = HID_KEY_F2 + (sw2-1)*4;
	if (sw3) *p++ = HID_KEY_F3 + (sw3-1)*4;
	if (sw4) *p++ = HID_KEY_F4 + (sw4-1)*4;
	hid_send_keyboard(0, keycodes);
}

static void process_angle(unsigned int angle)
{
	static unsigned int last_angle = 0;
	static bool initialized = false;

	if (!initialized) {
		last_angle = angle;
		initialized = true;
		return;
	}

	int diff = angle - last_angle;

	// Reset if jump too large (sync loss)
	if (diff > 1024 || diff < -1024) {
		last_angle = angle;
		return;
	}

#ifdef SCROLLWHEEL
	// Scroll in steps of 64
	//
	// This divides a full rotation into 64 steps,
	// which feels about right
	if (diff >= 64 || diff <= -64) {
		int steps = diff / 64;
		hid_send_mouse(0, 0, steps, 0);
		last_angle += (steps * 64);
	}
#else
	// Volume events randomly 1/32th of a full rotation
	uint16_t report;
	static uint16_t last_report = 0;

	// The sign depends on which side of the sensor you
	// put the magnet on, so don't be too attached to it
	if (diff < -70) {
		report = HID_USAGE_CONSUMER_VOLUME_INCREMENT;
		last_angle -= 128;
	} else if (diff > 70) {
		report = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
		last_angle += 128;
	} else {
		report = 0;
		if (!last_report)
			return;
	}
	last_report = report;
	hid_send_consumer(report);
#endif
}

static int read_switch(int idx)
{
	if (pio_sm_is_rx_fifo_empty(pio1, idx))
		return 0;

	int longpress = !!pio_sm_get_blocking(pio1, idx);

	// DEBUG ONLY
	// Long-press with *both* switches pressed down
	// turns it into programming mode.
	if (longpress) {
		if (!gpio_get(GPIO_SW1) && !gpio_get(GPIO_SW2))
			reset_usb_boot(0, 0);
	}

	return 1 + longpress;
}

int main()
{
	absolute_time_t next_led_update, next_hid_update;

	init_ws2812();
	init_sw_pins();
	init_i2c();
	init_usb();

	as5600_init();
	next_led_update = delayed_by_ms(get_absolute_time(), 100);
	next_hid_update = delayed_by_ms(get_absolute_time(), 100);

	for (;;) {
		absolute_time_t now = get_absolute_time();
		unsigned int angle = read_angle();

		if (now > next_led_update) {
			next_led_update = delayed_by_ms(now, 100);
			write_LED(led_color(angle));
		}

		tud_task();
		if (now > next_hid_update) {
			next_hid_update = delayed_by_ms(now, 10);
			int sw1 = read_switch(0), sw2 = read_switch(1),
			    sw3 = read_switch(2), sw4 = read_switch(3);

			//
			// Process buttons first, since they
			// are single-use events.
			//
			// Angle processing can wait until next
			// report if HID is busy with buttons
			//
			process_buttons(sw1, sw2, sw3, sw4);
			process_angle(angle);
		}
	}
}
