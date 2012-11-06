/* 

This code is licensed under the GPLv2. See COPYING for details.

BCM2835 libraries are provided by Mike McCauley (mikem@open.com.au)
The library is available at www.open.com.au/mikem/bcm2835

NES controller data was found at: http://www.mit.edu/~tarvizo/nes-controller.html

Compiling:

gcc -o nesctrl nesctrl.c -l rt -l bcm2835

Running:

sudo ./nesctrl

*/

#include <stdio.h>
#include <bcm2835.h>

// RPi PIN #17
#define NES_LATCH RPI_GPIO_P1_11
// RPi PIN #18
#define NES_CLOCK RPI_GPIO_P1_12
// RPi PIN #21
#define NES_DATA  RPI_GPIO_P1_13

// Define buttons

#define BTN_A		0x01
#define BTN_B		0x02
#define BTN_SELECT	0x04
#define BTN_START	0x08
#define BTN_UP		0x10
#define BTN_DOWN	0x20
#define BTN_LEFT	0x40
#define BTN_RIGHT	0x80

/**
 * Read data from controller.
 *
 * @return	value	state of all eight buttons.
 */
uint8_t read_buttons()
{
	// Toggle the latch 12us then wait 6us
	bcm2835_gpio_write(NES_LATCH,HIGH);
	delayMicroseconds(12);
	bcm2835_gpio_write(NES_LATCH,LOW);
	delayMicroseconds(6);

	uint8_t value = 0;	// Will hold value of all buttons.
	uint8_t tmp;		// Temporary storage for button data.
	
	// Grab data from all eight buttons by pulsing the clock for 6us (12us for full pulse).

	int	i;				

	for (i = 0; i < 8; i++) {
		
		tmp = bcm2835_gpio_lev(NES_DATA);
		
		bcm2835_gpio_write(NES_CLOCK,HIGH);

		delayMicroseconds(6);

		bcm2835_gpio_write(NES_CLOCK,LOW);
		
		delayMicroseconds(6);

		value |= (!tmp << i);
	}

	return value;
}

/**
 * Main.
 *
 * @param	int	 	argc
 * @param	char**	argv
 *
 * @return	int
 */
int main(int argc, char **argv)
{
	bcm2835_set_debug(0);

	if (!bcm2835_init()) {
		return 1;
	}

	// Set each of the pins as input or output
	bcm2835_gpio_fsel(NES_LATCH,BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(NES_CLOCK,BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(NES_DATA,BCM2835_GPIO_FSEL_INPT);

	// Initialize clock and latch
	bcm2835_gpio_write(NES_LATCH,LOW);
	bcm2835_gpio_write(NES_CLOCK,LOW);
	
	while(1) {
		
		uint8_t pressed = read_buttons();

		if ((pressed & BTN_A) == BTN_A)
			printf("A");
			
		if ((pressed & BTN_B) == BTN_B)
			printf("B");

		if ((pressed & BTN_SELECT) == BTN_SELECT)
			printf("SELECT");

		if ((pressed & BTN_START) == BTN_START)
			printf("START");

		if ((pressed & BTN_UP) == BTN_UP)
			printf("UP");

		if ((pressed & BTN_DOWN) == BTN_DOWN)
			printf("DOWN");

		if ((pressed & BTN_LEFT) == BTN_LEFT)
			printf("LEFT");

		if ((pressed & BTN_RIGHT) == BTN_RIGHT)
			printf("RIGHT"); 

		if (pressed) {
			printf("\n");
		}

		// Delay approximately 60Hz
		delay(((1 / 60) * 1000));
	}

	bcm2835_close();

	return 0;
}
