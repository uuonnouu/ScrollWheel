#pragma once

void init_usb(void);
void hid_send_mouse(int8_t x, int8_t y, int8_t vscroll, int8_t hscroll);
void hid_send_keyboard(uint8_t modifier, uint8_t keycodes[6]);
void hid_send_consumer(uint16_t usage);
