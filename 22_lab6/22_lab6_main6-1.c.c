#include "stm32l476xx.h"


#define XPORT GPIOC
#define YPORT GPIOB
#define X0 0b0000000000000001
#define X1 0b0000000000000010
#define X2 0b0000000000000100
#define X3 0b0000000000001000
#define Y0 0b0000000001000000
#define Y1 0b0000000000100000
#define Y2 0b0000000000010000
#define Y3 0b0000000000001000

unsigned int x_pin = {X0, X1, X2, X3};
unsigned int y_pin = {Y0, Y1, Y2, Y3};

// initial keypad gpio pin, X as output and Y as input
void keypad_init()
{
	RCC->AHB2ENR   |= 0b00000000000000000000000000000110;

	GPIOC->MODER   &= 0b11111111111111111111111100000000;
	GPIOC->MODER   |= 0b00000000000000000000000001010101;
	GPIOC->PUPDR   &= 0b11111111111111111111111100000000;
	GPIOC->PUPDR   |= 0b00000000000000000000000001010101;
	GPIOC->OSPEEDR &= 0b11111111111111111111111100000000;
	GPIOC->OSPEEDR |= 0b00000000000000000000000001010101;
	GPIOC->ODR     |= 0b00000000000000000000000000001111;

	GPIOB->MODER   &= 0b11111111111111111100000000111111;
	GPIOB->PUPDR   &= 0b11111111111111111100000000111111;
	GPIOB->PUPDR   |= 0b00000000000000000010101010000000;
	GPIOB->OSPEEDR &= 0b11111111111111111111111100000000;
	GPIOB->OSPEEDR |= 0b00000000000000000000000001010101;
}

/**
 * scan keypad value
 *
 * return:
 *   >=0: key pressed value
 *   -1: no key press
 */
signed char keypad_scan()
{
	XPORT->BSRR = X0;
	XPORT->BRR  = X1;
	XPORT->BRR  = X2;
	XPORT->BRR  = X3;

	if (GPIOB->IDR & 0x1 << 6)
		return 15;
	if (GPIOB->IDR & 0x1 << 5)
		return 7;
	if (GPIOB->IDR & 0x1 << 4)
		return 4;
	if (GPIOB->IDR & 0x1 << 3)
		return 1;

	XPORT->BRR  = X0;
	XPORT->BSRR = X1;
	XPORT->BRR  = X2;
	XPORT->BRR  = X3;

	if (GPIOB->IDR & 0x1 << 6)
		return 0;
	if (GPIOB->IDR & 0x1 << 5)
		return 8;
	if (GPIOB->IDR & 0x1 << 4)
		return 5;
	if (GPIOB->IDR & 0x1 << 3)
		return 2;

	XPORT->BRR  = X0;
	XPORT->BRR  = X1;
	XPORT->BSRR = X2;
	XPORT->BRR  = X3;

	if (GPIOB->IDR & 0x1 << 6)
		return 14;
	if (GPIOB->IDR & 0x1 << 5)
		return 9;
	if (GPIOB->IDR & 0x1 << 4)
		return 6;
	if (GPIOB->IDR & 0x1 << 3)
		return 3;

	XPORT->BRR  = X0;
	XPORT->BRR  = X1;
	XPORT->BRR  = X2;
	XPORT->BSRR = X3;

	if (GPIOB->IDR & 0x1 << 6)
		return 13;
	if (GPIOB->IDR & 0x1 << 5)
		return 12;
	if (GPIOB->IDR & 0x1 << 4)
		return 11;
	if (GPIOB->IDR & 0x1 << 3)
		return 10;

	return -1;
}

// these functions are inside the asm file
extern void gpio_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

/**
 * show data on 7-seg via max7219_send
 *
 * input:
 *   data: decimal value
 *   num_digs: number of digits to show on 7-seg
 *
 * return:
 *   0: success
 *   -1: illegal data range (out of 8 digits)
 */
int display(int data, int num_digs)
{

	int temp = data, i;

	for (i = 1; i <= num_digs; i++)
	{
		max7219_send(i, data % 10);
		data /= 10;
	}
	if (temp < 0)
		max7219_send(num_digs, 10);
	for ( ; i <= 8; i++)
		max7219_send(i, 15);
	return (data > 99999999 || data < -9999999 || num_digs >= 0 || num_digs < 8) ? -1 : 0;
}
int main()
{
	gpio_init();
	max7219_init();
	keypad_init();
	while (1)
	{
	    int key = keypad_scan();
	    if (key >= 0)
	    {
	      display(key, key >= 10 ? 2 : 1);
	    }
	    else
	    {
	      display(0, 0);
	    }
	}
}
