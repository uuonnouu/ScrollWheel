#include "pico/stdlib.h"
#include "pico.h"

#include "board.h"
#include "tusb.h"

#include "hid-device.h"

enum {
	REPORT_ID_KEYBOARD = 1,
	REPORT_ID_MOUSE,
	REPORT_ID_CONSUMER_CONTROL,
};

enum {
	ITF_HID,
	ITF_NUM_TOTAL
};

void init_usb(void)
{
	tusb_rhport_init_t dev_init = {
		.role = TUSB_ROLE_DEVICE,
		.speed = TUSB_SPEED_AUTO
	};
	tusb_init(0, &dev_init);
}

void hid_send_mouse(int8_t x, int8_t y, int8_t vscroll, int8_t hscroll)
{
	if (tud_suspended())
		tud_remote_wakeup();

	if (!tud_hid_ready())
		return;

	tud_hid_mouse_report(REPORT_ID_MOUSE, 0, x, y, vscroll, hscroll);
}

void hid_send_keyboard(uint8_t modifier, uint8_t keycodes[6])
{
	if (tud_suspended())
		tud_remote_wakeup();

	if (!tud_hid_ready())
		return;

	tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, keycodes);
}

void hid_send_consumer(uint16_t usage)
{
	if (tud_suspended())
		tud_remote_wakeup();

	if (!tud_hid_ready())
		return;

	uint16_t report = usage;
	tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &report, sizeof(report));
}


// We do nothing
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
			   hid_report_type_t report_type,
			   uint8_t const *buffer, uint16_t bufsize)
{
	return;
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
			       hid_report_type_t report_type,
			       uint8_t *buffer, uint16_t reqlen)
{
	return 0;
}

static tusb_desc_device_t const desc_device = {
	.bLength = sizeof(tusb_desc_device_t),
	.bDescriptorType = TUSB_DESC_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = 0x00,
	.bDeviceSubClass = 0x00,
	.bDeviceProtocol = 0x00,
	.bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

	.idVendor = 0xFFFF,
	.idProduct = 0x0001,
	.bcdDevice = 0x0100,

	.iManufacturer = 0x01,
	.iProduct = 0x02,
	.iSerialNumber = 0x03,

	.bNumConfigurations = 0x01
};

uint8_t const *tud_descriptor_device_cb(void)
{
	return (uint8_t const *) &desc_device;
}

static uint8_t const desc_hid_report[] = {
	TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(REPORT_ID_KEYBOARD)),
	TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE)),
	TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(REPORT_ID_CONSUMER_CONTROL)),
};

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
	return desc_hid_report;
}

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)
#define EPNUM_HID 0x81

static uint8_t const desc_configuration[] = {
	// Config number, interface count, string index, total length, attribute, power in mA
	TUD_CONFIG_DESCRIPTOR(1, 1, 0, CONFIG_TOTAL_LEN, 0, 100),

	// Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
	TUD_HID_DESCRIPTOR(ITF_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 10)
};

uint8_t const
*tud_descriptor_configuration_cb(uint8_t index) {
	return desc_configuration;
}

enum {
	STRID_LANGID = 0,
	STRID_MANUFACTURER,
	STRID_PRODUCT,
	STRID_SERIAL,
};

#define DESC_STRING(len) (2*((len)+1)+(TUSB_DESC_STRING<<8))
uint16_t const
*tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
	// Annoying UTF16LE format
	static const uint16_t reply[4][10] = {
		{ DESC_STRING(1), 0x0409 },	// English: 0x0409
		{ DESC_STRING(5), 'L', 'i', 'n', 'u', 's' },
		{ DESC_STRING(5), 'W', 'h', 'e', 'e', 'l' },
		{ DESC_STRING(1), '0' }	// Sure
	};
	if (index >= 4)
		return NULL;
	return reply[index];
}
