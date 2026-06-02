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

#define GPIO_SW1		10
#define GPIO_SW2		11
#define GPIO_SW3		12
#define GPIO_SW4		13

#define I2C_BUS			i2c0
#define I2C_SDA			4
#define I2C_SCL			5

#define WS2812_PIN		1	// WS2812 RGB LED
