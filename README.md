
# Minimal RP2350 Project for Volume Control / Scroll Wheel

[![DOI](https://zenodo.org/badge/1262064488)]()

A minimal RP2350-based project that uses an AS5600 magnetic angle sensor and one or more switches to create custom hardware knobs, volume controls, scroll wheels, or other rotary-input devices.

This project is intended as a simple starting point rather than a polished, ready-to-deploy solution. It can be adapted to a variety of RP2350-based boards and hardware configurations.

## Hardware Overview

The typical setup consists of:

* An **RP2350-based microcontroller board**

  * Example: Pimoroni Tiny 2350
* An **AS5600 magnetic angle sensor**

  * Connected via I²C
* One to four push-buttons or switches

  * Connected to GPIO pins
  * Other side of each switch connected to ground
* Optional RGB status LED

This configuration can be used to create:

* Volume knobs
* Scroll wheels
* Media controllers
* Navigation dials
* Custom HID input devices
* Experimental hardware interfaces

## LED Support

### WS2812 RGB LED

This project currently assumes the presence of a WS2812-compatible RGB LED and includes a PIO program for transmitting LED data.

Boards commonly using WS2812 LEDs include:

* Waveshare RP2350 Zero
* Various RP2040/RP2350 development boards

The LED is not required for core functionality but is useful for:

* Visual debugging
* Status indication
* User feedback

### Pimoroni Tiny 2350

The Pimoroni Tiny 2350 uses a different LED configuration.

Instead of a WS2812 smart LED, it includes a standard RGB LED controlled through individual GPIO pins:

| Color | GPIO    |
| ----- | ------- |
| Red   | GPIO 18 |
| Green | GPIO 19 |
| Blue  | GPIO 20 |

Color intensity is typically controlled using PWM.

Because of this hardware difference, the included WS2812 driver code will not work directly on the Pimoroni Tiny 2350.

## GPIO Considerations

GPIO assignments vary significantly between RP2350 boards.

For example:

### Pimoroni Tiny 2350

I²C0 is commonly mapped to:

```text
GPIO20 = SDA
GPIO21 = SCL
```

### Generic RP2350 Configuration

Many examples assume:

```text
GPIO4 = SDA
GPIO5 = SCL
```

The included board configuration uses the more common GPIO4/GPIO5 mapping.

You will likely need to modify:

* I²C pin assignments
* Button GPIO assignments
* LED configuration
* Board-specific initialization

before building and deploying.

## Building Expectations

Do not expect the project to:

* Build and run unchanged on every RP2350 board
* Automatically detect hardware configurations
* Provide production-ready firmware

Instead, treat it as:

* A reference implementation
* A hardware experimentation platform
* A starting point for custom rotary-controller projects

## Project Status

This repository originated as one of my early RP2350 experiments and has only received minimal updates since its original development.

While functional, it should be considered:

* Experimental
* Educational
* Adaptable
* In need of board-specific customization

Use it as a foundation for your own RP2350-based hardware projects and modify it as needed for your target platform.

