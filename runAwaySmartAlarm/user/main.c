#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"

#include "lcd.h"
#include "touch.h"
#include "pir.h"
#include "ultrasonic.h"

#include <bluetooth.h>
#include <stdio.h>
#include <stdlib.h>
#include <moveWheel.h>
#include <DS3231_BUZZER.h>
#include <stop_button.h>
#include "stm32f10x_exti.h"


void RCC_Configure(void);
void GPIO_Configure(void);

void is_answer(void);

void setDirection();

int directionFlag = 0;
int onOff = 1;
extern int Alarm_ONOFF;//have to combine isAlramOn
int answer;




//---------------------------------------------------------------------------------------------------


void RCC_Configure(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    // interrupt
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);  // RCC GPIO D


  TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
// Clear TIM2 Capture compare interrupt pending bit

}

//------------------------------------------------- blue tooth below------------

uint16_t receive_string[50];
extern uint16_t user_answer[10];
int string_count = 0;
int day = 0;
int hour = 0;
int minute = 0;
int string_receive_offset = 0;
int start_offset = 0;
int answer_correct;
//---------------------------------------------------------------------------------------------------

void RCC_Configure_bluetooth(void)
{
   // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
   /* UART TX/RX port clock enable */
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //UART 4

   /* USART4 clock enable */ 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
      
   /* Alternate Function IO clock enable */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure_bluetooth(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

   // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
    

    
    //TX2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    //RX2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void USART4_Init(void)
{
   USART_InitTypeDef USART_InitStructure;

   USART_Cmd(UART4, ENABLE);
   

   USART_InitStructure.USART_BaudRate = 9600;
   USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode= USART_Mode_Rx| USART_Mode_Tx;
   USART_Init(UART4, &USART_InitStructure);
   

   USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
}

void NVIC_Configure_bluetooth(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
   
    
    NVIC_EnableIRQ(UART4_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void UART4_IRQHandler() {
   uint16_t word;
    if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET){
       // the most recent received data by the USART1 peripheral
       word = USART_ReceiveData(UART4);
       
       if (start_offset == 1) {     
         //0x3b는 ;을 의미함 -> ;을 받으면 단어 입력 받기 중단하고 입력 처리(string_partition) 수행
         if (word == 0x3b) { //SetAlarm 7-19:20 [0x53, 0x65, 0x74, 0x41, 0x6c, 0x61, 0x72, 0x6d, 0x20, 0x37, 0x2d, 0x31, 0x39, 0x3a, 0x32, 0x30]
           
           string_partition();
           start_offset = 0;
         }
         else {
           receive_string[string_count] = word; 
           string_count++;

           // clear 'Read data register not empty' flag
           USART_ClearITPendingBit(UART4,USART_IT_RXNE);
         }
       }
       
       //0x40은 @을 의미함 -> @을 받으면 단어 입력 시작
       if (word == 0x40)  start_offset = 1;
       
       
       
       
       if (start_offset ==2){
         // ; 입력시 입력 종료
         if (word == 0x3b){//;
           is_answer();
           start_offset = 0;
         }
         else {
           receive_string[string_count] = word;
           string_count++;
           
           USART_ClearITPendingBit(UART4,USART_IT_RXNE);
         }
       }

       //=을 입력받아 정답을 입력 시작
       if (word == 0x3d) start_offset = 2;
    
    }
}

void sendDataToUART4(uint16_t data) {
  while ((UART4->SR & USART_SR_TXE) == 0);
    USART_SendData(UART4, data);
}


void is_answer(void) { //추가함
  
  char String[10];
  int st = 10-string_count;
  
  for (int i = 0; i < 10; i++) {
    String[i] = 0x30;
  }
  
  for (int i = 0; i < string_count; i++) {
    String[st+i] = (char)receive_string[i];
    receive_string[i] = 0;
  } //받았던 단어들을 String 배열로 옮기고 원래 배열을 빈 배열로 만들어줌
  
   st = atoi(String);

  
  if (st == answer){
    string_count = 0;
    sendDataToUART4('O');
    answer_correct = 1;
  }else{
    sendDataToUART4('X');
  }
}

void string_partition(void) {
  char String[50];
  
  for (int i = 0; i < string_count; i++) {
    String[i] = (char)receive_string[i];
    receive_string[i] = 0;
  } //받았던 단어들을 String 배열로 옮기고 receive_string 배열을 빈 배열로 만들어줌
  
  string_count = 0;
  
  char temp[50], *point;
  char* parti = " -:";
  char* partion_char[10];
  int i = 0;
  
  strcpy(temp, String);
  point = strtok(temp, parti); //parti 글자가 나오는 부분에서 string 부분 나누기
  
  while(point) {
    partion_char[i] = point;
    i++;
    point = strtok(NULL, parti);
  }
  
  if (strcmp(partion_char[0], "SetAlarm") == 0) {
    day = atoi(partion_char[1]);
    hour = atoi(partion_char[2]);
    minute = atoi(partion_char[3]);
    string_receive_offset = 1;
    
    char msg[] = "Alarm setting Complete\n";
                        
    for(int i = 0; i < 23; i++) {
         sendDataToUART4(msg[i]);
    }
  }  
}




void bluetooth_Init(void) {
  RCC_Configure_bluetooth();
  GPIO_Configure_bluetooth();
  USART4_Init();
  NVIC_Configure_bluetooth();
}

int get_day(void) {
  return day;
}

int get_hour(void) {
  return hour;
}

int get_minute(void) {
  string_receive_offset = 0;
  
  return minute;
}

int get_receive_flag(void) {
  return string_receive_offset;
}


//차량의 움직임 결정
void setDirection(int flag){
  switch(flag){
  case 0:
    setDirectionToFront();
    break;
  case 1:
    turnToLeft();
    break;
  case 2:
    setDirectionToBack();
    break;
  case 3:
    turnToRight();
    break;
  case 4:
    turnToLeft();
    break;
  case 5:
    setDirectionToFront();
    break;
  }
}



void delay(int n){

  for (int i = 0; i < n;i++);
};

 int main() {
  
  SystemInit();
  bluetooth_Init();
  RCC_Configure();
  
  RCC_Configure_wheel_pin();
  GPIO_Configure_wheel();
  
  DS3231_Alarm_Init();
  

  LCD_Init();
  LCD_Clear(WHITE);
  
  while(1){setDirectionToBack();}
    
  ds3231_time ds_time_default;   //구조체 변수 선언
  ds3231_Alarm1 alarm1_default;
  //DS3231에 새롭게 입력할 시간 데이터들을 설정 (위에서 선언한 변수 사용)
  ds_time_default.sec=0;
  ds_time_default.min=5;
  ds_time_default.hour_select.am_pm_24=ds3231_24_hour;
  ds_time_default.hour_select.hour=10;
  ds_time_default.day=6;
  ds_time_default.date=22;
  ds_time_default.month=12;
  ds_time_default.year=23;
  
  ds3231_write_time(&ds_time_default);   //구조체를 이용해 DS3231에 시간 데이터 입력
  ds3231_read_time(&ds_time_default);
  //시간이 변경됐는지 확인하기 위해 타임 레지스터에 저장되어있는 시간 데이터 읽어옴

  
  alarm1_default.sec=10;
  alarm1_default.min = 30;
  alarm1_default.hour_select.am_pm_24=ds3231_24_hour;
  alarm1_default.hour_select.hour = 10;
  alarm1_default.day_date_select.value = 22;
  alarm1_default.day_date_select.day_or_date=ds3231_date;
  
  ds3231_set_alarm1(&alarm1_default);
  ds3231_read_alarm1(&alarm1_default);


    

  int sec1 = ds_time_default.sec;
  int min1 = ds_time_default.min;
  int hour1 = ds_time_default.hour_select.hour;
  int hour_am_pm1 = ds_time_default.hour_select.am_pm_24;
  int day1 = ds_time_default.day;
  int date1 = ds_time_default.date;
  int month1 = ds_time_default.month;
  int year1 = ds_time_default.year;
  
  
  int alarm_sec = 0;
  int alarm_min = 0;
  int alarm_hour = 0;
  int alarm_day = 0;
  
  while(1){

    
    //RTC
    ds3231_read_time(&ds_time_default);
    ds3231_read_alarm1(&alarm1_default);
    GPIO_ResetBits(GPIOC, GPIO_Pin_9); //+ 추가함 => 0에서 알람을 설정한 시간이 되니 1로 변경됨
    
    int problem1 = 0;
    int problem2 = 0;
    
    
    if(alarm_check(&ds_time_default, &alarm1_default)){

      int sec_problem = alarm1_default.sec;
      int min_problem = alarm1_default.min;
      int hour_problem = alarm1_default.hour_select.hour;
      
      
      //GPIO_SetBits(GPIOC, GPIO_Pin_9); // 부저 작동
      GPIO_SetBits(GPIOC, GPIO_Pin_9);
      
      
      LCD_Clear(WHITE);
      
      
      while (1) {
          GPIO_SetBits(GPIOB, GPIO_Pin_12);
          GPIO_ResetBits(GPIOB, GPIO_Pin_13);
          GPIO_SetBits(GPIOA, GPIO_Pin_4);
          GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        
         
         
         //sec%hour, min%hour, X
         problem1 = sec_problem + hour_problem;
         problem2 = min_problem * hour_problem;
         
        answer = problem1 * problem2;
        
        LCD_ShowString(70, 100, "Solve the problem", BLACK, WHITE);
        LCD_ShowNum(90, 150, problem1, 2, BLACK, WHITE);
        LCD_ShowString(110, 150, "X", BLACK, WHITE);
        LCD_ShowNum(130, 150, problem2, 2, BLACK, WHITE);
        LCD_ShowString(150, 150, "=", BLACK, WHITE);
        LCD_ShowString(170, 150, "?", BLACK, WHITE);
        LCD_ShowString(130, 300, "Alarm_on", 0x0000, 0xFFFF);
        

        
        if (answer_correct) { // S1 버튼 누를시
          LCD_Clear(WHITE);
          GPIO_ResetBits(GPIOC, GPIO_Pin_9); // 부저 종료
          Delay_little();
          LCD_ShowString(30, 300, "Alarm_off", 0x0000, 0xFFFF);
          LCD_Clear(WHITE);
          answer_correct=0;
          stopAllWheel();
          break;
        }
      }
    }
      
    
    
      
      
      if (get_receive_flag() == 1) {
        
      alarm_day = get_day();
      alarm_min = get_minute();
      alarm_hour = get_hour();
      
      alarm1_default.day_date_select.value = get_day();
      alarm1_default.hour_select.hour = get_hour();
      alarm1_default.min = get_minute();
      ds3231_set_alarm1(&alarm1_default);
      ds3231_read_alarm1(&alarm1_default);
    }
    
    // 현재 시간 가져오기.
      sec1 = ds_time_default.sec;
      min1 = ds_time_default.min;
      hour1 = ds_time_default.hour_select.hour;
      hour_am_pm1 = ds_time_default.hour_select.am_pm_24;
      day1 = ds_time_default.day;
      date1 = ds_time_default.date;
      month1 = ds_time_default.month;
      year1 = ds_time_default.year;
    
      
      //현재시간과 알람 설정시간 LCD에 표시.
      LCD_ShowNum(180, 60, GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9), 2, RED, WHITE); // C8 bit -> 알람이 on이면 1, off면 0
      
      LCD_ShowNum(50, 100, year1, 2, BLACK, WHITE);
      LCD_ShowString(70, 100, "/", BLACK, WHITE);
      LCD_ShowNum(90, 100, month1, 2, BLACK, WHITE);
      LCD_ShowString(110, 100, "/", BLACK, WHITE);
      LCD_ShowNum(130, 100, date1, 2, BLACK, WHITE);
      LCD_ShowString(150, 100, "/", BLACK, WHITE);
      LCD_ShowNum(170, 100, day1, 1, BLACK, WHITE);
      
      LCD_ShowNum(30, 200, ds_time_default.hour_select.am_pm_24, 1, BLACK, WHITE);
      LCD_ShowNum(60, 200, hour1, 2, BLACK, WHITE);
      LCD_ShowString(90, 200, ":", BLACK, WHITE);
      LCD_ShowNum(120, 200, min1, 2, BLACK, WHITE);
      LCD_ShowString(150, 200, ":", BLACK, WHITE);
      LCD_ShowNum(180, 200, sec1, 2, BLACK, WHITE);
      
      LCD_ShowNum(60, 250, alarm_hour, 2, BLACK, WHITE);
      LCD_ShowString(90, 250, ":", BLACK, WHITE);
      LCD_ShowNum(120, 250, alarm_min, 2, BLACK, WHITE);
      LCD_ShowString(150, 250, ":", BLACK, WHITE);
      LCD_ShowNum(180, 250, alarm_sec, 2, BLACK, WHITE);
      
      
  }
}