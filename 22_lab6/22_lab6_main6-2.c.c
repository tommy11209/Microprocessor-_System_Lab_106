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

int total, length;
int num_arr[14];
int check = 0;

// initial keypad gpio pin, X as output and Y as key
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
#define DEBOUNCE 150

signed char keypad_scan()
{
	    int times = DEBOUNCE;
	    int keys = 0, data = 0, oldKeys = 0, clear = 0;
	    while (times--) {
	    data = 0,                                              //sum
	    clear = 0,                                             //when 1 , clear all bits
		keys = 0;                                              //0b0000000000000000

		XPORT->BSRR = X0;
		XPORT->BRR  = X1;
		XPORT->BRR  = X2;
		XPORT->BRR  = X3;

		if (GPIOB->IDR & 0x1 << 6)
		{
			keys =  keys | (1 << 3);        //keys = 0b0000000000001000
			clear = 1;
			//return 15;
		}

		if (GPIOB->IDR & 0x1 << 5)
		{
			keys = keys | ( 1 << 2);        //keys = 0b0000000000000100
			data += 7;
			//return 7;
		}

		if (GPIOB->IDR & 0x1 << 4)
		{
			keys = keys | (1 << 1);         //keys = 0b0000000000000010
			data += 4;
			//return 4;
		}
		if (GPIOB->IDR & 0x1 << 3)
		{
			keys = keys | (1 << 0);         //keys = 0b0000000000000001
			data += 1;
			//return 1;
		}

		XPORT->BRR  = X0;
		XPORT->BSRR = X1;
		XPORT->BRR  = X2;
		XPORT->BRR  = X3;

		if (GPIOB->IDR & 0x1 << 6)
		{
			keys = keys | (1 << 7);
			data += 0;
			//return 0;
		}
		if (GPIOB->IDR & 0x1 << 5)
		{

		    keys = keys | (1 << 6);
		    data += 8;
		}
			//return 8;
		if (GPIOB->IDR & 0x1 << 4)
		{
		    keys = keys | (1 << 5);
		    data += 5;
			//return 5;
		}
		if (GPIOB->IDR & 0x1 << 3)
		{
			keys = keys | (1 << 4);
			data += 2;
		//return 2;
		}
		XPORT->BRR  = X0;
		XPORT->BRR  = X1;
		XPORT->BSRR = X2;
		XPORT->BRR  = X3;

		if (GPIOB->IDR & 0x1 << 6)
		{
	    	keys = keys | (1 << 11);
	    	clear = 1;
			//return 14;
		}
		if (GPIOB->IDR & 0x1 << 5)
		{
		    keys = keys | (1 << 10);
		    data += 9;
			//return 9;
		}
		if (GPIOB->IDR & 0x1 << 4)
		{
			keys = keys | (1 << 9);
			data += 6;
			//return 6;
		}
		if (GPIOB->IDR & 0x1 << 3)
		{
			keys = keys | (1 << 8);
			data += 3;
			//return 3;
		}

		XPORT->BRR  = X0;
		XPORT->BRR  = X1;
		XPORT->BRR  = X2;
		XPORT->BSRR = X3;

		if (GPIOB->IDR & 0x1 << 6)
		{
			keys = keys | (1 << 15);
			data += 13;

		}

		if (GPIOB->IDR & 0x1 << 5)
		{
			keys = keys | (1 << 14);
			data += 12;
		}


		if (GPIOB->IDR & 0x1 << 4)
		{
			keys = keys | (1 << 13);
			data += 11;
		}

		if (GPIOB->IDR & 0x1 << 3)
		{
			keys = keys | (1 << 12);
			data += 10;
		}

	}

	return keys ? (clear ? -1 : data) : -2;

}

// these functions are inside the asm file
extern void gpio_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

/**
 * show data on 7-seg via max7219_send
 *
 * key:
 *   data: decimal value
 *   num_digs: number of digits to show on 7-seg
 *
 * return:
 *   0: success
 *   -1: illegal data range (out of 8 digits)
 */
int display(int data, int num_digs)
{
	num_digs = num_digs > 8 ? 8 : num_digs;
	int temp = data, i;
	for (i = 1; i <= num_digs; i++)
	{
		if (data % 10 < 0)
			max7219_send(i, -data % 10);
		else
			max7219_send(i, data % 10);
		data /= 10;
	}
	if (temp < 0)
	    max7219_send(num_digs, 10);
	for ( ; i <= 8; i++)
		max7219_send(i, 15);
	return (data > 99999999 || data < -9999999) ? -1 : 0;
}

int main()
{
	  gpio_init();
	  max7219_init();
	  keypad_init();
	  int length = 0, data = 0;
	  int count = 0;
	  while (1)
	  {
		  display(data, length);
		  int key = keypad_scan();
		  if (key == -2)              //dont do anything , just display data, then next
		  {
			  count = 0;
			  continue;
		  }

		  // push push push , wait a few sec until count > 50 to show next num
		  if (count)
		  {
			  if(count++ < 50)
	    	      continue;
		  }

		  count = 1;

		  if (key == -1)           //clear all
		  {
			  length = 0;
			  data = 0;
			  continue;
		  }
		  int keylen = key > 9 ? 2 : 1;                      //two bits or one bit
		  if (length + keylen > 8)                           //just show , dont do anything
			  continue;

		  length += keylen;                                  //the length to show the press button , key length is 1 or 2
		  data = data * (keylen == 2 ? 100 : 10) + key;      //show the sum like 10123, if 2 bits then *100 , 1 bits *10, then add
	  }
	  return 0;
	}
