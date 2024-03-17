#include "stop_button.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

void RCC_BUTTON(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);  // RCC GPIO D
}

void GPIO_BUTTON(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = BUTTON_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);
}

void Init_BUTTON(void) {
    RCC_BUTTON();
    GPIO_BUTTON();
}

