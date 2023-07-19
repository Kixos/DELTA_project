#ifndef _FUNCTIONS
#define _FUNCTIONS

#include <stm32f446xx.h>
#include <system_stm32f4xx.h>
#include <stdbool.h>
#include <stm32f4xx.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum{LedOff = 0, LedOn = 1, LedTog = 2}eLed;
typedef enum{r = 0, b = 1, g = 2}LedCol;
typedef enum{null = 0, click = 1}eButton;
typedef enum{DIR = 0, ENA = 1}out;

void delay_ms(uint32_t ms);
void delay_ms2(uint32_t ms);
void Led_Conf(void);
void Led_OnOff(LedCol col, eLed state);
void SysTick_Handler(void);
void Button_Conf(void);
bool Button_Read(void); 
void USART2ConfDMA(void);
void USART2ReinitDMA(void);
void USART2SendDMA(const char *str, uint16_t length);	
void USART2SendDMAUINT(const uint8_t *str, uint16_t length);
void USART3ConfDMA(void);
void USART3ReinitDMA(void);
void USART3SendDMA(const char *str, uint16_t length);
void USART3SendDMAUINT(const uint8_t *str, uint16_t length);
void SendUsart(char c);
void ZeroDMABufRX(void);
int DELTA_CalcAngle(float x, float y, float z, float alfa, float *kat);
int DELTA_Inverse(float x, float y, float z);
void Deg2Imp(void);
void qpoly3(float t, int tf);
void vpoly3(float t, int tf);
void qpoly5(float t, int tf);
void vpoly5(float t, int tf);

#endif
