#include <moveWheel.h>
#include "ultrasonic.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#define LEFT_HIGH GPIO_Pin_12
#define LEFT_LOW GPIO_Pin_13
#define RIGHT_HIGH GPIO_Pin_4
#define RIGHT_LOW GPIO_Pin_5


//모터 PB12, PB13
//모터2 PA4, PA5

//모터 (RC카에 있는 함수 사용해서 변경한 것 -> 함수명은 짱돌님에게 맞춰져있음 )

void RCC_Configure_wheel_pin(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

void GPIO_Configure_wheel(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = LEFT_HIGH | LEFT_LOW;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = RIGHT_HIGH | RIGHT_LOW;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void setDirectionToFront(void) {
    GPIO_SetBits(GPIOB, LEFT_HIGH);
    GPIO_ResetBits(GPIOB, LEFT_LOW);
    GPIO_SetBits(GPIOA, RIGHT_HIGH);
    GPIO_ResetBits(GPIOA, RIGHT_LOW);
} 

void setDirectionToBack(void) {
    GPIO_ResetBits(GPIOB, LEFT_HIGH);
    GPIO_SetBits(GPIOB, LEFT_LOW);
    GPIO_ResetBits(GPIOA, RIGHT_HIGH);
    GPIO_SetBits(GPIOA, RIGHT_LOW);
}

void turnToLeft(void) {
    GPIO_SetBits(GPIOB, LEFT_HIGH);
    GPIO_ResetBits(GPIOB, LEFT_LOW);
    GPIO_ResetBits(GPIOA, RIGHT_HIGH);
    GPIO_ResetBits(GPIOA, RIGHT_LOW);
}

void turnToRight(void) {
    GPIO_ResetBits(GPIOB, LEFT_HIGH);
    GPIO_ResetBits(GPIOB, LEFT_LOW);
    GPIO_SetBits(GPIOA, RIGHT_HIGH);
    GPIO_ResetBits(GPIOA, RIGHT_LOW);
}

void stopAllWheel(void) {
    GPIO_ResetBits(GPIOB, LEFT_HIGH);
    GPIO_ResetBits(GPIOB, LEFT_LOW);
    GPIO_ResetBits(GPIOA, RIGHT_HIGH);
    GPIO_ResetBits(GPIOA, RIGHT_LOW);
}



