#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "String.h"
#include "Stdio.h"
#include "Stdlib.h"

#include "misc.h"

/* function prototype */
void RCC_Configure_bluetooth(void);
void GPIO_Configure_bluetooth(void);
void USART4_Init(void);
void NVIC_Configure_bluetooth(void);
void bluetooth_Init(void); // 초기 setting

int get_day(void); // bluetooth를 통해 전해받은 day 값을 반환
int get_hour(void); // bluetooth를 통해 전해받은 hour 값을 반환
int get_minute(void); // bluetooth를 통해 전해받은 minute 값을 반환
int get_receive_flag(void); // receive flag 값을 반환 (1 : 수령받은 메세지가 있음, 0 : 수령받은 메세지가 없음)

void USART1_IRQHandler(void);
void USART4_IRQHandler(void);

void sendDataToUART1(uint16_t data);
void sendDataToUART4(uint16_t data);

void string_partition(void);