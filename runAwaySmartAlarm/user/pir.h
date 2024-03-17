#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#define PIR_PORT1               GPIOD             
#define PIR_PIN1                GPIO_Pin_1
#define PIR_PORT2               GPIOD
#define PIR_PIN2                GPIO_Pin_0

void Init_PIR(void);