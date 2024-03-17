#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#define US_TIMER               TIM3

// SENSOR PIN ... //will choose apeach one
#define US_TRIG_PORT            GPIOB              
#define US_TRIG_PIN               GPIO_Pin_0      //TIM3 Ch3 (trig output)

#define US_ECHO_PORT            GPIOA
#define US_ECHO_PIN               GPIO_Pin_6      //TIM3 Ch1 (echo input)
#define US_TIMER_TRIG_SOURCE      TIM_TS_TI1FP1


//updated
#define inputTriger_updated             TIM_TS_TI1FP1


#define US_TRIG_PORT2            GPIOA     
#define US_ECHO_PORT2            GPIOA     
#define US_TRIG_PIN2             GPIO_Pin_2 //tim5 ch3
#define US_ECHO_PIN2              GPIO_Pin_0//tim5 ch1

//-----------------------------------------have to change below
#define inputTriger_updated2             TIM_TS_TI2FP1
#define US_TRIG_PORT3            GPIOA     
#define US_ECHO_PORT3            GPIOA     
#define US_TRIG_PIN3             GPIO_Pin_3 //tim2 ch4 //using port A
#define US_ECHO_PIN3              GPIO_Pin_1//tim2 ch2 //using port A


//

extern void EnableHCSR04PeriphClock();
void EnableHCSR04PeriphClock();
void InitHCSR04();                                  // MUST DO
int32_t HCSR04GetDistance();                        // USE THIS METHOD TO MEASURE DISTANCE
int32_t HCSR04GetDistance2();
int32_t HCSR04GetDistance3();
int delayByTim2();//can use timmer witout delay function but will stop when HCSR04GetDistance3's return value is stop