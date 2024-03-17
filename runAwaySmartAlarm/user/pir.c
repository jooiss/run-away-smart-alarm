#include "pir.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

void RCC_PIR(void)
{
  // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);  // RCC GPIO D
  
}

void GPIO_PIR(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
  
  // body detect input D1
  GPIO_InitStructure.GPIO_Pin = PIR_PIN1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(PIR_PORT1, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = PIR_PIN2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(PIR_PORT2, &GPIO_InitStructure);
}

void Init_PIR(void) {
    RCC_PIR();
    GPIO_PIR();
}

