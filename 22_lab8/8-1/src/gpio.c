#include "gpio.h"
#include "../inc/stm32l476xx.h"
#include "../inc/stm32l4xx_ll_gpio.h"
#define UserButton GPIOC, LL_GPIO_PIN_13

void GPIO_init_pin(GPIO_TypeDef *GPIOx, uint32_t PinMask) {
  LL_GPIO_SetPinMode(GPIOx, PinMask, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinSpeed(GPIOx, PinMask, LL_GPIO_SPEED_HIGH);
  LL_GPIO_SetPinOutputType(GPIOx, PinMask, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(GPIOx, PinMask, LL_GPIO_PULL_DOWN);
  ResetPin(GPIOx, PinMask);
}

void GPIO_init() {
  RCC->AHB2ENR |=
      RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN;
  LL_GPIO_SetPinMode(UserButton, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(UserButton, LL_GPIO_PULL_DOWN);
}
