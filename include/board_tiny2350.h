// board.h — Pimoroni Tiny 2350
// Drop this into include/ replacing the original board.h
//
// Fork of: https://github.com/torvalds/ScrollWheel
// Adapted for Pimoroni Tiny 2350 by UUON Foundation

#pragma once

// ─── I2C ────────────────────────────────────────────────────────────────────
// Pimoroni Tiny 2350 exposes i2c0 on GPIO 20/21 (Qwiic connector)
// Upstream default was GPIO 4/5 — this is the primary divergence point

#define I2C_BUS     i2c0
#define I2C_SDA     20
#define I2C_SCL     21

// ─── Switches ───────────────────────────────────────────────────────────────
// Active-low, internal pullup enabled in init_sw_pins()
// Other side of each switch wired directly to GND
// Adjust GPIO numbers to match your wiring

#define GPIO_SW1    0
#define GPIO_SW2    1
#define GPIO_SW3    2
#define GPIO_SW4    3

// ─── LED ────────────────────────────────────────────────────────────────────
// Pimoroni Tiny 2350 has a DUMB RGB LED — three separate GPIOs, NOT WS2812
// Do NOT define WS2812_PIN here; the LED init in blink.c handles PWM instead
// GPIO 18 = Red, 19 = Green, 20 = Blue (confirmed from Pimoroni schematic)
//
// NOTE: GPIO 20 is shared with I2C_SDA above.
// On the Pimoroni Tiny, the LED blue channel and I2C SDA are the same pin.
// The SDK will assert if you try to set two functions on the same GPIO.
// EITHER: use only Red+Green (comment out LED_B_PIN)
// OR:     wire your I2C to i2c1 on different pins and adjust I2C_BUS above.
//
// Default here: Red+Green only (safe, no conflict)

#define LED_TYPE_DUMB_RGB       // tells blink.c to use PWM, not PIO WS2812

#define LED_R_PIN   18
#define LED_G_PIN   19
// #define LED_B_PIN   20       // conflicts with I2C_SDA — see note above

// ─── AS5600 ─────────────────────────────────────────────────────────────────
// Fixed I2C address — do not change unless you have a modified AS5600
#define AS5600_I2C_ADDR  0x36   // already hardcoded in blink.c, here for reference

// ─── Scroll / Volume tuning ──────────────────────────────────────────────────
// These mirror the magic numbers in blink.c process_angle()
// Documented here so you don't hunt for them in the source

// SCROLLWHEEL mode: one HID scroll step per this many raw angle units
#define SCROLL_STEP     64      // 4096 / 64 = 64 steps per full rotation

// VOLUME mode: threshold to trigger one volume event
#define VOLUME_THRESHOLD 70     // raw angle units
#define VOLUME_STEP     128     // angle units consumed per event
