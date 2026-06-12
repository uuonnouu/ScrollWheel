
## Minimal rp2350 project for volume control / scrollwheel
[![DOI](https://zenodo.org/badge/1262064488

Use an AS5600 magnetic angle sensor and a couple of switches to make
random hardware knobs.

Typically with something like a Pimoroni Tiny 2350 with a Qwiic
connector for i2c, and 1-4 GPIOs connected to switches (with the
other side of the switch just connected to ground). 

Or take this as a base and do something else.

NOTE! This particular setup assumes that you have a WS2812 RGB LED, and
has a PIO program to send it data.  That is somewhat common (eg
Waveshare RP2350 Zero), but it is by no means universal.  Some have just
a single LED controlled by a single GPIO.

The Pimoroni Tiny has a RGB LED, but it's not the WS2812 smart LED: it
has a "dumb" RGB LED with RGB programmed through three separate GPIOs
individually (GPIO 18/19/20 for RGB respectively - typically using PWM
to set the level of each color). 

You don't _need_ a LED for any of this, but it's nice to have for visual
debugging, so just be aware that 

All the other GPIO assignments are also random based on board (eg the
Pimoroni board puts i2c0 on GPIO20/21, while the included board file is
set up for the more normal GPIO4/5).

So don't expect to just "build and use as-is".  Also don't expect
anything polished.  This is one of my early rp2354 project trials with
minimal updates to bring it up to snuff again.

