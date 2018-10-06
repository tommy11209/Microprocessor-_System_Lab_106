#ifndef GPIO_H
#define GPIO_H
#include "../inc/stm32l476xx.h"
#include "../inc/stm32l4xx.h"

#ifndef SetPin
#define SetPin LL_GPIO_SetOutputPin
#endif
#ifndef ResetPin
#define ResetPin LL_GPIO_ResetOutputPin
#endif
#ifndef IsSetPin
#define IsSetPin LL_GPIO_IsInputPinSet
#endif

#endif
