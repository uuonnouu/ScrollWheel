//
// Board GPIO pin definitions
//

//
// Random carrier board:
//  - WS2812 on GPIO1
//  - GPIO 10-13 (and GND) brought out on IDC cable
//
// I2C0 on GPIO4/5 (and I2C1 on GPIO2/3)
//
// Foot (or finger) switches on GPIOs. Use internal
// pull-up with switch closing to GND.
//

#define I2C_BUS     i2c0
#define I2C_SDA     20
#define I2C_SCL     21
#define GPIO_SW1    0
#define GPIO_SW2    1
#define GPIO_SW3    2
#define GPIO_SW4    3
// No WS2812_PIN — using dumb RGB
#define LED_R_PIN   18
#define LED_G_PIN   19
#define LED_B_PIN   20

#define WS2812_PIN		1	// WS2812 RGB LED
