#define STM32L476XX

#ifndef USE_FULL_LL_DRIVER
#define USE_FULL_LL_DRIVER
#endif

#include "string.h"
#include "stdio.h"
#include "../inc/stm32l476xx.h"
#include "../inc/stm32l4xx_ll_bus.h"
#include "../inc/stm32l4xx_ll_gpio.h"
#include "../inc/stm32l4xx_ll_rcc.h"
#include "gpio.h"

#include "onewire.h"
#include "ds18b20.h"

#define UserButton GPIOC, LL_GPIO_PIN_13

#define DS18B20 GPIOA, LL_GPIO_PIN_0
//in stm32l4xx.h
#define EnableSysTick()                              \
  MODIFY_REG(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, \
             (1) << SysTick_CTRL_ENABLE_Pos)
#define DisableSysTick()                             \
  MODIFY_REG(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk, \
             (0) << SysTick_CTRL_ENABLE_Pos)
#define EnableSysTickInt()                            \
  MODIFY_REG(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk, \
             (1) << SysTick_CTRL_TICKINT_Pos)
#define SelectSysTickSrc()                              \
  MODIFY_REG(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk, \
             (1) << SysTick_CTRL_CLKSOURCE_Pos)
#define IsEnableSysTick()                              \
  (READ_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk) == \
   (SysTick_CTRL_ENABLE_Msk))

void SystemClock_Config() {
  // Set system clock to 4 MHz
  LL_RCC_MSI_EnableRangeSelection();
  LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_8);  // Range mode 8: 01001, 24MHz
  while (!LL_RCC_MSI_IsReady())
    ;
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_MSI);
  // 4MHZ ~~= 24 * ( 10 / 7 ) / 8
  LL_RCC_PLL_ConfigDomain_SYS((LL_RCC_PLLSOURCE_MSI), LL_RCC_PLLM_DIV_7, 10,
                              LL_RCC_PLLR_DIV_8);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while (!LL_RCC_PLL_IsReady())
    ;
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

  if (IsEnableSysTick()) DisableSysTick();
  SysTick->LOAD = 0xffffc300;  // 4_000_000 * 2      2sec
  SysTick->VAL = 0;
  SelectSysTickSrc();
  EnableSysTickInt();
  EnableSysTick();
}

int isBtnPress(){
	int i = 0, d = 20;
	while(d--){
		if(i != IsSetPin(UserButton)){
			d = 20;
			i = !i;
		}
	}
	return !i;
}

volatile int count;
volatile int Toggle_MAX7129 = 1;
volatile int tog = 1;
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

int l = -1;
int btnPressed = 0;
float temp;
int convert = 0;

//Toggle the temp when interrupt occurred
void SysTick_Handler() {
  count++;
  Toggle_MAX7219 = Toggle_MAX7129? 0: 1;
}

OneWire_t OneWire_ds18b20;

int display(int data, int num_digs)
{
	int SHOWDIGIT_MAX7129_dec_pt = 0;
	if (num_digs <= -1000)
	{
		num_digs = -1000 - num_digs ;
		SHOWDIGIT_MAX7129_dec_pt = 1;
	}
	num_digs = num_digs > 8 ? 8 : num_digs;
	int data2 = data, i;
	for (i = 1; i <= num_digs; i++)
	{
		if (data2 < 0 && i == num_digs);
		else if (SHOWDIGIT_MAX7129_dec_pt && i == 3 && data % 10 < 0)
			max7219_send(i, -data % 10 | 0b10000000);
		else if (SHOWDIGIT_MAX7129_dec_pt && i == 3)
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
int displayf(float data, int num_digs)
{
	if (num_digs > 8)
		return display(-1, 2);
	if ((int) (data * 100) % 100)
		return display(data * 100, -1002 - num_digs);
	else
		return display(data, num_digs);
}

void show_result()
{
	if(!isBtnPress())          //debouncing button
	{
		if(!btnPressed)
			btnPressed = 1;
		if(convert)
		{
			if(!DS18B20_Done(&OneWire_ds18b20))
			{
				if(tog)
				{
					DS18B20_Read(&OneWire_ds18b20, &temp);
					display(temp , 2);
				}
				convert = 0;
			}
		}
		else
		{
			if(l != count)
			{
				l = count;
				convert = 1;
				DS18B20_ConvT(&OneWire_ds18b20, TM_DS18B20_Resolution_12bits);
			}
		}

	}
	else
	{
		if(btnPressed)
		{
			IsEnableSysTick() ? DisableSysTick() : EnableSysTick();    //if pressed then disable
			tog = tog? 0: 1;
			btnPressed = 0;
			l = -1, count = 0;
		}
	}
}

int main() {

  MODIFY_REG(SCB->CPACR, 0xf << 20, 0xf << 20);
  SystemClock_Config();
  GPIO_init();      //for button
  gpio_init();      //for max7219
  max7219_init();
  OneWire_Init(&OneWire_ds18b20, DS18B20);

  while(1){
	  show_result();
  }
}
