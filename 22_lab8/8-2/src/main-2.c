#include "stm32l476xx.h"


#define SYST_CSR	(*((uint32_t *)0xe000e010))
#define SYST_RVR	(*((uint32_t *)0xe000e014))
#define SYST_CVR	(*((uint32_t *)0xe000e018))
#define SYST_CALIB	(*((uint32_t *)0xe000e01c))



extern void gpio_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

void sysclk_config (void);
void SysTick_Handler (void);
void EXTI_setup (void);
void EXTI9_5_IRQHandler (void);
void EXTI15_10_IRQHandler (void);
int display(int , int);
void keypad_init (void);
int keypad_scan (void);
int row4 (void);
int row3 (void);
int row2 (void);
int row1 (void);
void col4 (int val);
void col3 (int val);
void col2 (int val);
void col1 (int val);

void keypad_init (void)
{
	/* enable AHB2 clock for port A, B */
	RCC->AHB2ENR |= 0x3;

	/* set PA8, 9, 10 as input mode and PA0, 1, 4 as output mode */
	GPIOA->MODER &= ~ (GPIO_MODER_MODE0_Msk |
			GPIO_MODER_MODE1_Msk |
			GPIO_MODER_MODE4_Msk |
			GPIO_MODER_MODE8_Msk |
			GPIO_MODER_MODE9_Msk |
			GPIO_MODER_MODE10_Msk);
	GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE4_0;
	/* set PB5 as input mode and PB0 as output mode */
	GPIOB->MODER &= ~ (GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE5_Msk);
	GPIOB->MODER |= GPIO_MODER_MODE0_0;

	/* set PA8, PA9, PA10, PB5 as pull-down inputs */
	GPIOA->PUPDR &= ~ (GPIO_PUPDR_PUPD8_Msk | GPIO_PUPDR_PUPD9_Msk | GPIO_PUPDR_PUPD10_Msk);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD8_1 | GPIO_PUPDR_PUPD9_1 | GPIO_PUPDR_PUPD10_1;
	GPIOB->PUPDR &= ~ GPIO_PUPDR_PUPD5_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD5_1;
	/* set PA0, PA1, PA4, PB0 as pull-up outputs */
	GPIOA->PUPDR &= ~ (GPIO_PUPDR_PUPD0_Msk | GPIO_PUPDR_PUPD1_Msk | GPIO_PUPDR_PUPD4_Msk);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD0_0 | GPIO_PUPDR_PUPD1_0 | GPIO_PUPDR_PUPD4_0;
	GPIOB->PUPDR &= ~ GPIO_PUPDR_PUPD0_Msk;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD0_0;

	/* set PA0, 1, 4, 8, 9, 10 as medium speed mode */
	GPIOA->OSPEEDR &= ~ (GPIO_OSPEEDR_OSPEED0_Msk |
			GPIO_OSPEEDR_OSPEED1_Msk |
			GPIO_OSPEEDR_OSPEED4_Msk |
			GPIO_OSPEEDR_OSPEED8_Msk |
			GPIO_OSPEEDR_OSPEED9_Msk |
			GPIO_OSPEEDR_OSPEED10_Msk);
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED0_0 |
			GPIO_OSPEEDR_OSPEED1_0 |
			GPIO_OSPEEDR_OSPEED4_0 |
			GPIO_OSPEEDR_OSPEED8_0 |
			GPIO_OSPEEDR_OSPEED9_0 |
			GPIO_OSPEEDR_OSPEED10_0;
	/* set PB0, 5 as medium speed mode */
	GPIOB->OSPEEDR &= ~ (GPIO_OSPEEDR_OSPEED0_Msk | GPIO_OSPEEDR_OSPEED5_Msk);
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED0_0 | GPIO_OSPEEDR_OSPEED5_0;
}

int row4 (void)
{
	return !!(GPIOA->IDR & GPIO_IDR_ID10);
}

int row3 (void)
{
	return !!(GPIOB->IDR & GPIO_IDR_ID5);
}

int row2 (void)
{
	return !!(GPIOA->IDR & GPIO_IDR_ID8);
}

int row1 (void)
{
	return !!(GPIOA->IDR & GPIO_IDR_ID9);
}

void col4 (int val)	/* PB0 */
{
	if (val) {
		GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0);	/* push-pull */
		GPIOB->BSRR = GPIO_BSRR_BS0;
	}
	else {
		GPIOB->OTYPER |= GPIO_OTYPER_OT0;	/* open-drain */
		GPIOB->BRR = GPIO_BRR_BR0;
	}
}

void col3 (int val)	/* PA4 */
{
	if (val) {
		GPIOA->OTYPER &= ~(GPIO_OTYPER_OT4);	/* push-pull */
		GPIOA->BSRR = GPIO_BSRR_BS4;
	}
	else {
		GPIOA->OTYPER |= GPIO_OTYPER_OT4;	/* open-drain */
		GPIOA->BRR = GPIO_BRR_BR4;
	}
}

void col2 (int val)	/* PA1 */
{
	if (val) {
		GPIOA->OTYPER &= ~(GPIO_OTYPER_OT1);	/* push-pull */
		GPIOA->BSRR = GPIO_BSRR_BS1;
	}
	else {
		GPIOA->OTYPER |= GPIO_OTYPER_OT1;	/* open-drain */
		GPIOA->BRR = GPIO_BRR_BR1;
	}
}

void col1 (int val)	/* PA0 */
{
	if (val) {
		GPIOA->OTYPER &= ~(GPIO_OTYPER_OT0);	/* push-pull */
		GPIOA->BSRR = GPIO_BSRR_BS0;
	}
	else {
		GPIOA->OTYPER |= GPIO_OTYPER_OT0;	/* open-drain */
		GPIOA->BRR = GPIO_BRR_BR0;
	}
}

int keypad_scan (void)
{
	int	bits = 0;

	/* set the col 4 */
	col4 (1);
	col3 (0);
	col2 (0);
	col1 (0);
	/* read through row 4 to row 1 */
	if (row4 ())
		bits |= 0x1 << 13;
	if (row3 ())
		bits |= 0x1 << 12;
	if (row2 ())
		bits |= 0x1 << 11;
	if (row1 ())
		bits |= 0x1 << 10;

	/* set the col 3 */
	col4 (0);
	col3 (1);
	col2 (0);
	col1 (0);
	/* read through row 4 to row 1 */
	if (row4 ())
		bits |= 0x1 << 14;
	if (row3 ())
		bits |= 0x1 << 9;
	if (row2 ())
		bits |= 0x1 << 6;
	if (row1 ())
		bits |= 0x1 << 3;

	/* set the col 2 */
	col4 (0);
	col3 (0);
	col2 (1);
	col1 (0);
	/* read through row 4 to row 1 */
	if (row4 ())
		bits |= 0x1 << 0;
	if (row3 ())
		bits |= 0x1 << 8;
	if (row2 ())
		bits |= 0x1 << 5;
	if (row1 ())
		bits |= 0x1 << 2;

	/* set the col 1 */
	col4 (0);
	col3 (0);
	col2 (0);
	col1 (1);
	/* read through row 4 to row 1 */
	if (row4 ())
		bits |= 0x1 << 15;
	if (row3 ())
		bits |= 0x1 << 7;
	if (row2 ())
		bits |= 0x1 << 4;
	if (row1 ())
		bits |= 0x1 << 1;

	return bits;
}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

int	key = 0, col = 0;

void EXTI_setup (void)
{
	/* enable the APB2 clock for SYSCFG */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* manage the external interrupt to GPIOs */
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PB;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PA;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI9_PA;
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PA;

	/* set up the interrupt mask register EXTI_IMR1 */
	EXTI->IMR1 |= EXTI_IMR1_IM5 | EXTI_IMR1_IM8 | EXTI_IMR1_IM9 | EXTI_IMR1_IM10;
	/* set the falling-triggered interrupt */
	EXTI->FTSR1 |= EXTI_FTSR1_FT5 | EXTI_FTSR1_FT8 | EXTI_FTSR1_FT9 | EXTI_FTSR1_FT10;

	/* set up the NVIC */
	NVIC_SetPriority (EXTI9_5_IRQn, 0);
	NVIC_SetPriority (EXTI15_10_IRQn, 0);
	NVIC_EnableIRQ (EXTI9_5_IRQn);
	NVIC_EnableIRQ (EXTI15_10_IRQn);
}

//read 4 input and look what column now and return key
void EXTI9_5_IRQHandler (void)
{
	NVIC_ClearPendingIRQ (EXTI9_5_IRQn);
	if(col == 4){
		if (row4 ()) {
			key = 13;
			EXTI->PR1 |= EXTI_PR1_PIF10;
		}
		if (row3 ()) {
			key = 12;
			EXTI->PR1 |= EXTI_PR1_PIF5;
		}
		if (row2 ()) {
			key = 11;
			EXTI->PR1 |= EXTI_PR1_PIF8;
		}
		if (row1 ()) {
			key = 10;
			EXTI->PR1 |= EXTI_PR1_PIF9;
		}
	}
	else if(col == 3){
		if (row4 ()) {
			key = 14;
			EXTI->PR1 |= EXTI_PR1_PIF10;
		}
		if (row3 ()) {
			key = 9;
			EXTI->PR1 |= EXTI_PR1_PIF5;
		}
		if (row2 ()) {
			key = 6;
			EXTI->PR1 |= EXTI_PR1_PIF8;
		}
		if (row1 ()) {
			key = 3;
			EXTI->PR1 |= EXTI_PR1_PIF9;
		}
	}
	else if(col == 2){
		if (row4 ()) {
			key = 0;
			EXTI->PR1 |= EXTI_PR1_PIF10;
		}
		if (row3 ()) {
			key = 8;
			EXTI->PR1 |= EXTI_PR1_PIF5;
		}
		if (row2 ()) {
			key = 5;
			EXTI->PR1 |= EXTI_PR1_PIF8;
		}
		if (row1 ()) {
			key = 2;
			EXTI->PR1 |= EXTI_PR1_PIF9;
		}
	}
	else if(col == 1){
		if (row4 ()) {
			key = 15;
			EXTI->PR1 |= EXTI_PR1_PIF10;
		}
		if (row3 ()) {
			key = 7;
			EXTI->PR1 |= EXTI_PR1_PIF5;
		}
		if (row2 ()) {
			key = 4;
			EXTI->PR1 |= EXTI_PR1_PIF8;
		}
		if (row1 ()) {
			key = 1;
			EXTI->PR1 |= EXTI_PR1_PIF9;
		}
	}
}

void EXTI15_10_IRQHandler (void)
{
	NVIC_ClearPendingIRQ (EXTI15_10_IRQn);
	if(col == 4){
		if (row4 ()) {
			key = 13;
			EXTI->PR1 |= EXTI_PR1_PIF10;
		}
		if (row3 ()) {
			key = 12;
			EXTI->PR1 |= EXTI_PR1_PIF5;
		}
		if (row2 ()) {
			key = 11;
			EXTI->PR1 |= EXTI_PR1_PIF8;
		}
		if (row1 ()) {
			key = 10;
			EXTI->PR1 |= EXTI_PR1_PIF9;
		}
	}
	else if(col == 3){
		if (row4 ()) {
			key = 14;
			EXTI->PR1 |= EXTI_PR1_PIF10;
		}
		if (row3 ()) {
			key = 9;
			EXTI->PR1 |= EXTI_PR1_PIF5;
		}
		if (row2 ()) {
			key = 6;
			EXTI->PR1 |= EXTI_PR1_PIF8;
		}
		if (row1 ()) {
			key = 3;
			EXTI->PR1 |= EXTI_PR1_PIF9;
		}
	}
	else if(col == 2){
		if (row4 ()) {
			key = 0;
			EXTI->PR1 |= EXTI_PR1_PIF10;
		}
		if (row3 ()) {
			key = 8;
			EXTI->PR1 |= EXTI_PR1_PIF5;
		}
		if (row2 ()) {
			key = 5;
			EXTI->PR1 |= EXTI_PR1_PIF8;
		}
		if (row1 ()) {
			key = 2;
			EXTI->PR1 |= EXTI_PR1_PIF9;
		}
	}
	else if(col == 1){
		if (row4 ()) {
			key = 15;
			EXTI->PR1 |= EXTI_PR1_PIF10;
		}
		if (row3 ()) {
			key = 7;
			EXTI->PR1 |= EXTI_PR1_PIF5;
		}
		if (row2 ()) {
			key = 4;
			EXTI->PR1 |= EXTI_PR1_PIF8;
		}
		if (row1 ()) {
			key = 1;
			EXTI->PR1 |= EXTI_PR1_PIF9;
		}
	}
}

/* the exception handler */
//when interruot , scan which column pressed
void SysTick_Handler (void)
{
	if (--col <= 0)
		col = 4;

	if(col == 4){
		col4 (1);
		col3 (0);
		col2 (0);
		col1 (0);
	}
	else if(col == 3){
		col4 (0);
		col3 (1);
		col2 (0);
		col1 (0);
	}
	else if(col == 2){
		col4 (0);
		col3 (0);
		col2 (1);
		col1 (0);
	}
	else if(col == 1){
		col4 (0);
		col3 (0);
		col2 (0);
		col1 (1);
	}
}
void sysclk_config (void)
{
	/* switch the SYSCLK to MSI */
		RCC->CFGR &= ~RCC_CFGR_SW_Msk;
		RCC->CFGR |= RCC_CFGR_SW_MSI;
		while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_MSI);
		/* turn the PLL off */
		RCC->CR &= ~RCC_CR_PLLON;
		while (RCC->CR & RCC_CR_PLLRDY);
		/* set the RCC_PLLCFGR */
		RCC->PLLCFGR = 0x03000a02;
		/* turn the HSI16 on */
		RCC->CR |= RCC_CR_HSION;
		while ((RCC->CR & RCC_CR_HSIRDY) == 0);
		/* turn the PLL on */
		RCC->CR |= RCC_CR_PLLON;
		while ((RCC->CR & RCC_CR_PLLRDY) == 0);
		/* switch the SYSCLK to PLL */
		RCC->CFGR &= ~RCC_CFGR_SW_Msk;
		RCC->CFGR |= RCC_CFGR_SW_PLL;
		while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);
		/* set the HPRE */
		RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;
		RCC->CFGR |= RCC_CFGR_HPRE_DIV4;

		/* initialize the SysTick timer */
		SYST_RVR = 999999;
		SYST_CSR |= 0x7;
}

int display(int data, int num_digs)
{
	int show_dec_pt = 0;
	if (num_digs <= -1000)
	{
		num_digs = -1000 - num_digs ;
		show_dec_pt = 1;
	}
	num_digs = num_digs > 8 ? 8 : num_digs;
	int data2 = data, i;
	for (i = 1; i <= num_digs; i++)
	{
		if (data2 < 0 && i == num_digs);
		else if (show_dec_pt && i == 3 && data % 10 < 0)
			max7219_send(i, -data % 10 | 0b10000000);
		else if (show_dec_pt && i == 3)
			max7219_send(i, data % 10 | 0b10000000);
		else if (data % 10 < 0)
			max7219_send(i, -data % 10);
		else
			max7219_send(i, data % 10);
		data /= 10;
	}
	if (data2 < 0)
		max7219_send(num_digs, 10);
	for ( ; i <= 8; i++)
		max7219_send(i, 15);
	return (data > 99999999 || data < -9999999) ? -1 : 0;
}

int main (void)
{
	sysclk_config ();
	gpio_init();
	max7219_init ();
	keypad_init ();
	EXTI_setup ();

	while (1)
	{
		display (key, 2);
	}
	return 0;
}


