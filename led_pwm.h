// led_pwm.h — Dumb RGB LED via PWM for Pimoroni Tiny 2350
//
// Drop this file into your repo root.
// In blink.c: replace #include "ws2812.pio.h" with #include "led_pwm.h"
// and replace init_ws2812() + write_LED() calls with the equivalents below.
//
// Depends on: board.h (LED_R_PIN, LED_G_PIN, LED_TYPE_DUMB_RGB)
// SDK deps:   hardware/pwm.h (already available via pico_stdlib target)

#pragma once

#include "hardware/pwm.h"
#include "board_tiny2350.h"

// ─── Internal helpers ────────────────────────────────────────────────────────

static inline void _pwm_pin_init(uint pin)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice, 255);
    pwm_set_enabled(slice, true);
}

static inline void _pwm_pin_set(uint pin, uint8_t level)
{
    pwm_set_gpio_level(pin, level);
}

// ─── Public API ──────────────────────────────────────────────────────────────

static inline void init_led(void)
{
#ifdef LED_TYPE_DUMB_RGB
    _pwm_pin_init(LED_R_PIN);
    _pwm_pin_init(LED_G_PIN);
#ifdef LED_B_PIN
    _pwm_pin_init(LED_B_PIN);
#endif
#endif
}

static inline void write_LED(uint32_t grb)
{
#ifdef LED_TYPE_DUMB_RGB
    uint8_t g = (grb >> 24) & 0xFF;
    uint8_t r = (grb >> 16) & 0xFF;
    uint8_t b = (grb >>  8) & 0xFF;

    _pwm_pin_set(LED_R_PIN, r);
    _pwm_pin_set(LED_G_PIN, g);
#ifdef LED_B_PIN
    _pwm_pin_set(LED_B_PIN, b);
#else
    (void)b;
#endif
#endif
}
