#include "functions.h"

const float sqrt3 = 1.7320508075688772935274463415059;
const float pi = 3.1415926535897932384626433832795;
const float sin120 = sqrt3/2.0;   
const float cos120 = -0.5; 
const float deg2rad30 = 0.523598776;
const float deg2rad150 = 2.61799388;
const float deg2rad270 = 4.71238898;

const float e = 2*sqrt3*62;     // bok trojkata platformy
const float f = 2*sqrt3*190;     // bok trojkata podstawy
const float La = 257.5;		// dlugosc ramienia czynnego
const float b1 = 548;		// dlugosc ramienia biernego  
const float Lb = 190;
const float Le = 62;

volatile uint32_t tick = 0;
volatile uint32_t timeout = 0;
volatile uint8_t DMABufTX[100];
volatile uint8_t DMABufRX[100];
volatile uint8_t BufWrite[500];
volatile uint8_t BufRead[100];

uint16_t tab1[18000];
uint16_t tab2[18000];
uint16_t tab3[18000];
volatile uint16_t wp = 0;

float theta1;
float theta2;
float theta3;

int32_t imp1 = 98724;
int32_t imp2 = 98724;
int32_t imp3 = 98724;

float kat1;
float kat2;
float kat3;

float t;

char pfb[100];

int backimp1, backimp2, backimp3;
int pom = 0;
int pom1 = 0;
char pfb1[100];
char pfb2[100];
char pfb3[100];

float qt, vt;
float a0, a1, a2, a3, a4, a5;

void Led_Conf(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER &= ~GPIO_MODER_MODER0;
	GPIOB->MODER |= GPIO_MODER_MODER0_0;
	GPIOB->MODER &= ~GPIO_MODER_MODER7;
	GPIOB->MODER |= GPIO_MODER_MODER7_0;
	GPIOB->MODER &= ~GPIO_MODER_MODER14;
	GPIOB->MODER |= GPIO_MODER_MODER14_0;
}
void Led_OnOff(LedCol nr, eLed state)
{
	switch(nr)
	{
		case r:
			if(state == LedOff) 		GPIOB->ODR &= ~GPIO_ODR_ODR_14;
			else if(state == LedOn) 	GPIOB->ODR |= GPIO_ODR_ODR_14;
			else if(state == LedTog) 	GPIOB->ODR ^= GPIO_ODR_ODR_14;
		break;
				case g:
			if(state == LedOff) 			GPIOB->ODR &= ~GPIO_ODR_ODR_0;
			else if(state == LedOn) 	GPIOB->ODR |= GPIO_ODR_ODR_0;
			else if(state == LedTog) 	GPIOB->ODR ^= GPIO_ODR_ODR_0;
		break;
		case b:
			if(state == LedOff) 		GPIOB->ODR &= ~GPIO_ODR_ODR_7;
			else if(state == LedOn) 	GPIOB->ODR |= GPIO_ODR_ODR_7;
			else if(state == LedTog) 	GPIOB->ODR ^= GPIO_ODR_ODR_7;
		break;
	}
}
void SysTick_Handler(void)
{
	tick++;
	timeout++;
}
void delay_ms(uint32_t ms)
{
	tick = 0;
	while(tick < ms);
}
void delay_ms2(uint32_t ms)
{
	timeout = 0;
	while(timeout < ms);
}
void Button_Conf(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER &= ~GPIO_MODER_MODE13;
}	
bool Button_Read(void)
{
	if((GPIOC->IDR & GPIO_IDR_ID13) != 0) return true;
	else return false;
}
/***************************************************************************
*  	USART2 Configuration using DMA
*	Enabling GPIOD, USART2, DMA1 clocks
*	Setting GPIOD.5, GPIOD.6 to alternate function mode
*	Setting Baud Rate to 19200 bps
*	Enabling DMA mode for transmission and reception
*	Setting GPIOs to proper alternate function
*	Enabling receiver, transmitter and USART
*	Enabling IDLE interrupt
*	Enabling NVIC for USART2 IRQ
*	Selecting memory source for DMA stream transmission 
*	Selecting maximum size of transfer and the array to send data from
*	Memory increment mode, direction - out, transfer complete interrupt enable
*	Selecting memory source for DMA stream reception 
*	Selecting maximum size of transfer and the array to put data into
*	Memory increment mode, direction - in, stream enable
****************************************************************************/
void USART2ConfDMA(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	
	RCC -> AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	GPIOD->MODER &= ~GPIO_MODER_MODER5 & ~GPIO_MODER_MODER6;
	GPIOD->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1;
	
	USART2->BRR = 16000000/115200;
	USART2->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
	GPIOD->AFR[0] |= 0x07700000; 
	USART2->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; 
	
	USART2->CR1 |= USART_CR1_IDLEIE;
	
	NVIC_EnableIRQ(USART2_IRQn);
	
	DMA1_Stream6 -> PAR = (uint32_t)&USART2->DR;
	DMA1_Stream6 -> M0AR = (uint32_t)BufWrite;
	DMA1_Stream6 -> NDTR = (uint16_t)500;
	DMA1_Stream6 -> CR |= DMA_SxCR_CHSEL_2;
	DMA1_Stream6 -> CR |= DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE;
	
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
	
	DMA1_Stream5 -> PAR = (uint32_t)&USART2->DR;
	DMA1_Stream5 -> M0AR = (uint32_t)BufRead;
	DMA1_Stream5 -> NDTR = (uint16_t)100;
	DMA1_Stream5 -> CR |= DMA_SxCR_CHSEL_2;
	
	DMA1_Stream5 -> CR |= DMA_SxCR_MINC | DMA_SxCR_EN;	
}
/***************************************************************************
*  	DMA reinitialisation for USART2
*	Disabling the stream and clearing transfer complete flag after reception
*	Setting maximum size, enabling the stream again
****************************************************************************/
void USART2ReinitDMA(void)
{
	DMA1_Stream5 -> CR &= ~DMA_SxCR_EN;
	DMA1 -> HIFCR |= DMA_HIFCR_CTCIF5;
	
	DMA1_Stream5 -> NDTR = (uint16_t)100;
	DMA1_Stream5 -> CR |= DMA_SxCR_EN;
}
/***************************************************************************
*  	USART2 sending using DMA
*	Putting values into array to send data from
*	Disabling the stream, setting data length
*	Enabling the stream sending data
****************************************************************************/
void USART2SendDMA(const char *str, uint16_t length)
{
	for(uint16_t i=0; i<length; i++)
	{
		BufWrite[i]=str[i];
	}
	DMA1_Stream6 -> CR &= ~DMA_SxCR_EN;
	DMA1_Stream6 -> NDTR = (uint16_t)length;
	DMA1_Stream6 -> CR |= DMA_SxCR_EN;		
}
/***************************************************************************
*  	USART2 sending using DMA
*	Putting values into array to send data from
*	Disabling the stream, setting data length
*	Enabling the stream sending data
****************************************************************************/
void USART2SendDMAUINT(const uint8_t *str, uint16_t length)
{
	for(uint16_t i=0; i<length; i++)
	{
		BufWrite[i]=str[i];
	}
	DMA1_Stream6 -> CR &= ~DMA_SxCR_EN;
	DMA1_Stream6 -> NDTR = (uint16_t)length;
	DMA1_Stream6 -> CR |= DMA_SxCR_EN;
}
/***************************************************************************
*  	Interrupt handler for USART2 sending stream
*	Clearing the transfer complete flag
****************************************************************************/
void DMA1_Stream6_IRQHandler(void)
{
	if((DMA1 -> HISR & DMA_HISR_TCIF6) != RESET)
	{
		DMA1 -> HIFCR |= DMA_HIFCR_CTCIF6;
	}
}
/***************************************************************************
*  	USART2 interrupt handler
*	Reinitialising DMA for USART2 whenever USART2 is IDLE, 
*	making it ready to receive new data
****************************************************************************/
void USART2_IRQHandler(void)
{
	if((USART2 -> SR & USART_SR_IDLE) != RESET)
	{
		if(pom == 1 && (BufRead[0] == '6' || BufRead[0] == '7' || BufRead[0] == '8' || BufRead[0] == '9' || (BufRead[0] == '1' && BufRead[1] != '-')))
		{
			for(int i=0;i<100;i++)
			{
				pfb1[i] = BufRead[i];
				BufRead[i] = 0;
				pom = 2;
			}
		}
		if(pom == 2 && (BufRead[0] == '6' || BufRead[0] == '7' || BufRead[0] == '8' || BufRead[0] == '9' || (BufRead[0] == '1' && BufRead[1] != '-' && BufRead[1] != ' ')))
		{
			for(int i=0;i<100;i++)
			{
				pfb2[i] = BufRead[i];
				BufRead[i] = 0;
				pom = 3;
			}
		}
		if(pom == 3 && (BufRead[0] == '6' || BufRead[0] == '7' || BufRead[0] == '8' || BufRead[0] == '9' || (BufRead[0] == '1' && BufRead[1] != '-')))
		{
			for(int i=0;i<100;i++)
			{
				pfb3[i] = BufRead[i];
				BufRead[i] = 0;
				pom = 0;
			}
		}
		for(int i=0;i<100;i++)
		{
			if(pfb1[i] == '.')
			{
				backimp1 = (pfb1[i-1] - '0');				
				if(pfb1[i-2] != ' ' && pfb1[i-2] != '-')
				{
					backimp1 += ((pfb1[i-2] - '0') * 10);
				}
				if(pfb1[i-2] != ' ' && pfb1[i-3] != ' ' && pfb1[i-2] != '-' && pfb1[i-3] != '-')
				{
					backimp1 += ((pfb1[i-3] - '0') * 100);
				}
				if(pfb1[i-2] != ' ' && pfb1[i-3] != ' ' && pfb1[i-4] != ' ' && pfb1[i-2] != '-' && pfb1[i-3] != '-' && pfb1[i-4] != '-')
				{
					backimp1 += ((pfb1[i-4] - '0') * 1000);
				}
				if(pfb1[i-2] != ' ' && pfb1[i-3] != ' ' && pfb1[i-4] != ' ' && pfb1[i-5] != ' ' && pfb1[i-2] != '-' && pfb1[i-3] != '-' && pfb1[i-4] != '-' && pfb1[i-5] != '-')
				{
					backimp1 += ((pfb1[i-5] - '0') * 10000);
				}
				if(i == 6 && pfb1[i-2] != ' ' && pfb1[i-3] != ' ' && pfb1[i-4] != ' ' && pfb1[i-5] != ' ' && pfb1[i-6] != ' ' && pfb1[i-2] != '-' && pfb1[i-3] != '-' && pfb1[i-4] != '-' && pfb1[i-5] != '-' && pfb1[i-6] != '-')
				{
					backimp1 += ((pfb1[i-6] - '0') * 100000);
				}
			}
			if(pfb2[i] == '.')
			{
				backimp2 = (pfb2[i-1] - '0');				
				if(pfb2[i-2] != ' ' && pfb2[i-2] != '-')
				{
					backimp2 += ((pfb2[i-2] - '0') * 10);
				}
				if(pfb2[i-2] != ' ' && pfb2[i-3] != ' ' && pfb2[i-2] != '-' && pfb2[i-3] != '-')
				{
					backimp2 += ((pfb2[i-3] - '0') * 100);
				}
				if(pfb2[i-2] != ' ' && pfb2[i-3] != ' ' && pfb2[i-4] != ' ' && pfb2[i-2] != '-' && pfb2[i-3] != '-' && pfb2[i-4] != '-')
				{
					backimp2 += ((pfb2[i-4] - '0') * 1000);
				}
				if(pfb2[i-2] != ' ' && pfb2[i-3] != ' ' && pfb2[i-4] != ' ' && pfb2[i-5] != ' ' && pfb2[i-2] != '-' && pfb2[i-3] != '-' && pfb2[i-4] != '-' && pfb2[i-5] != '-')
				{
					backimp2 += ((pfb2[i-5] - '0') * 10000);
				}
				if(i == 6 && pfb2[i-2] != ' ' && pfb2[i-3] != ' ' && pfb2[i-4] != ' ' && pfb2[i-5] != ' ' && pfb2[i-6] != ' ' && pfb2[i-2] != '-' && pfb2[i-3] != '-' && pfb2[i-4] != '-' && pfb2[i-5] != '-' && pfb2[i-6] != '-')
				{
					backimp2 += ((pfb2[i-6] - '0') * 100000);
				}
			}
			if(pfb3[i] == '.')
			{
				backimp3 = (pfb3[i-1] - '0');				
				if(pfb3[i-2] != ' ' && pfb3[i-2] != '-')
				{
					backimp3 += ((pfb3[i-2] - '0') * 10);
				}
				if(pfb3[i-2] != ' ' && pfb3[i-3] != ' ' && pfb3[i-2] != '-' && pfb3[i-3] != '-')
				{
					backimp3 += ((pfb3[i-3] - '0') * 100);
				}
				if(pfb3[i-2] != ' ' && pfb3[i-3] != ' ' && pfb3[i-4] != ' ' && pfb3[i-2] != '-' && pfb3[i-3] != '-' && pfb3[i-4] != '-')
				{
					backimp3 += ((pfb3[i-4] - '0') * 1000);
				}
				if(pfb3[i-2] != ' ' && pfb3[i-3] != ' ' && pfb3[i-4] != ' ' && pfb3[i-5] != ' ' && pfb3[i-2] != '-' && pfb3[i-3] != '-' && pfb3[i-4] != '-' && pfb3[i-5] != '-')
				{
					backimp3 += ((pfb3[i-5] - '0') * 10000);
				}
				if(i == 6 && pfb3[i-2] != ' ' && pfb3[i-3] != ' ' && pfb3[i-4] != ' ' && pfb3[i-5] != ' ' && pfb3[i-6] != ' ' && pfb3[i-2] != '-' && pfb3[i-3] != '-' && pfb3[i-4] != '-' && pfb3[i-5] != '-' && pfb3[i-6] != '-')
				{
					backimp3 += ((pfb3[i-6] - '0') * 100000);
				}
			}
	}
		USART2ReinitDMA();
		uint8_t temp = USART2 -> SR;
		temp = USART2 -> DR;
		temp = USART2 -> SR;
	}
}
/***************************************************************************
*  	USART3 Configuration using DMA
*	Enabling GPIOD, USART3, DMA1 clocks
*	Setting GPIOD.8, GPIOD.9 to alternate function mode
*	Setting Baud Rate to 57600 bps
*	Enabling DMA mode for transmission and reception
*	Setting GPIOs to proper alternate function
*	Enabling receiver, transmitter and USART
*	Enabling IDLE interrupt
*	Enabling NVIC for USART3 IRQ
*	Selecting memory source for DMA stream transmission 
*	Selecting maximum size of transfer and the array to send data from
*	Memory increment mode, direction - out, transfer complete interrupt enable
*	Selecting memory source for DMA stream reception 
*	Selecting maximum size of transfer and the array to put data into
*	Memory increment mode, direction - in, stream enable
****************************************************************************/
void USART3ConfDMA(void)
{	
  RCC -> APB1ENR |= RCC_APB1ENR_USART3EN;
  RCC -> AHB1ENR |= RCC_AHB1ENR_GPIODEN;	
	RCC -> AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	
  GPIOD -> MODER &= ~GPIO_MODER_MODER8 & ~GPIO_MODER_MODER9;
  GPIOD -> MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1;
		
  GPIOD -> AFR[1] |= 0x00000077;
		
  USART3 -> BRR = 16000000/57600;
		
	USART3->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
		
  USART3 -> CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
	USART3 -> CR1 |= USART_CR1_IDLEIE;
	
	NVIC_EnableIRQ(USART3_IRQn);
	
	DMA1_Stream3 -> PAR = (uint32_t)&USART3->DR;
	DMA1_Stream3 -> M0AR = (uint32_t)DMABufTX;
	DMA1_Stream3 -> NDTR = (uint16_t)100;
	DMA1_Stream3 -> CR |= DMA_SxCR_CHSEL_2;
	DMA1_Stream3 -> CR |= DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE;
	
	NVIC_EnableIRQ(DMA1_Stream3_IRQn);

	DMA1_Stream1 -> PAR = (uint32_t)&USART3->DR;
	DMA1_Stream1 -> M0AR = (uint32_t)DMABufRX;
	DMA1_Stream1 -> NDTR = (uint16_t)100;
	DMA1_Stream1 -> CR |= DMA_SxCR_CHSEL_2;
	DMA1_Stream1 -> CR |= DMA_SxCR_MINC | DMA_SxCR_EN;
}
/***************************************************************************
*  	DMA reinitialisation for USART3
*	Disabling the stream and clearing transfer complete flag after reception
*	Setting maximum size, enabling the stream again
****************************************************************************/
void USART3ReinitDMA(void)
{
	DMA1_Stream1 -> CR &= ~DMA_SxCR_EN;
	DMA1 -> LIFCR |= DMA_LIFCR_CTCIF1;
	
	DMA1_Stream1 -> NDTR = (uint16_t)100;
	DMA1_Stream1 -> CR |= DMA_SxCR_EN;
}
/***************************************************************************
*  	USART3 sending using DMA
*	Putting values into array to send data from
*	Disabling the stream, setting data length
*	Enabling the stream sending data
****************************************************************************/
void USART3SendDMA(const char *str, uint16_t length)
{
	for(uint16_t i=0; i<length; i++)
	{
		DMABufTX[i]=str[i];
	}	
	
	DMA1_Stream3 -> CR &= ~DMA_SxCR_EN;
	DMA1_Stream3 -> NDTR = (uint16_t)length;
	DMA1_Stream3 -> CR |= DMA_SxCR_EN;	
}
/***************************************************************************
*  	USART3 sending using DMA
*	Putting values into array to send data from
*	Disabling the stream, setting data length
*	Enabling the stream sending data
****************************************************************************/
void USART3SendDMAUINT(const uint8_t *str, uint16_t length)
{
	for(uint16_t i=0; i<length; i++)
	{
		DMABufTX[i]=str[i];
	}
		
	DMA1_Stream3 -> CR &= ~DMA_SxCR_EN;
	DMA1_Stream3 -> NDTR = (uint16_t)length;
	DMA1_Stream3 -> CR |= DMA_SxCR_EN;
}
void SendUsart(char c)
{
	while(!(USART3->SR & USART_SR_TXE))
	{
		
	}
	USART3->DR = c;
}
/***************************************************************************
*  	Interrupt handler for USART3 sending stream
*	Clearing the transfer complete flag
****************************************************************************/
void DMA1_Stream3_IRQHandler(void)
{
	if((DMA1 -> LISR & DMA_LISR_TCIF3) != RESET)
	{
		DMA1 -> LIFCR |= DMA_LIFCR_CTCIF3;
	}
}
/***************************************************************************
*  	USART2 interrupt handler
*	Reinitialising DMA for USART2 whenever USART2 is IDLE or transfer
*	has been completed, making it ready to receive new data
****************************************************************************/
void USART3_IRQHandler(void)
{
	if((USART3 -> SR & USART_SR_IDLE) != RESET)
	{
		USART3ReinitDMA();
		uint8_t temp = USART3 -> SR;
		temp = USART3 -> DR;
	}
	
//	if((USART3 -> SR & USART_SR_TC) != RESET)
//	{
//		USART3 -> SR &= ~USART_SR_TC;
//		USART3ReinitDMA();
//	}
}
/***************************************************************************
*  	Zeroing USART3 reception array
****************************************************************************/
void ZeroDMABufRX(void)
{
	for(int i=0; i<50; i++)
	{
		DMABufRX[i]=0;
	}
}
/***************************************************************************
*  	DELTA ROBOT
****************************************************************************/
int DELTA_CalcAngle(float x, float y, float z, float alfa, float *kat)
{
	t = (-4*La*z+sqrt(16*La*La*z*z-4*(b1*b1-La*La+2*La*Lb-Lb*Lb-2*La*Le+2*Lb*Le-Le*Le-x*x-y*y-z*z-2*La*x*cos(alfa)+
	2*Lb*x*cos(alfa)-2*Le*x*cos(alfa)-2*La*y*sin(alfa)+2*Lb*y*sin(alfa)-2*Le*y*sin(alfa))*(b1*b1-La*La-2*La*Lb-
	Lb*Lb+2*La*Le+2*Lb*Le-Le*Le-x*x-y*y-z*z+2*La*x*cos(alfa)+2*Lb*x*cos(alfa)-2*Le*x*cos(alfa)+2*La*y*sin(alfa)+
	2*Lb*y*sin(alfa)-2*Le*y*sin(alfa))))/(2*(b1*b1-La*La+2*La*Lb-Lb*Lb-2*La*Le+2*Lb*Le-Le*Le-x*x-y*y-z*z-
	2*La*x*cos(alfa)+2*Lb*x*cos(alfa)-2*Le*x*cos(alfa)-2*La*y*sin(alfa)+2*Lb*y*sin(alfa)-2*Le*y*sin(alfa)));
	
	if((16*La*La*z*z-4*(b1*b1-La*La+2*La*Lb-Lb*Lb-2*La*Le+2*Lb*Le-Le*Le-x*x-y*y-z*z-2*La*x*cos(alfa)+
	2*Lb*x*cos(alfa)-2*Le*x*cos(alfa)-2*La*y*sin(alfa)+2*Lb*y*sin(alfa)-2*Le*y*sin(alfa))*(b1*b1-La*La-2*La*Lb-
	Lb*Lb+2*La*Le+2*Lb*Le-Le*Le-x*x-y*y-z*z+2*La*x*cos(alfa)+2*Lb*x*cos(alfa)-2*Le*x*cos(alfa)+2*La*y*sin(alfa)+
	2*Lb*y*sin(alfa)-2*Le*y*sin(alfa)))<0)
	return -1;
	*kat = 180-2*atan(t)*180/pi;
	return 0;
}
int DELTA_Inverse(float x, float y, float z) 
{
  kat1 = 0;
	kat2 = 0;
	kat3 = 0;
  int status1 = DELTA_CalcAngle(x, y, z, deg2rad30, &kat1);
  if (status1 == 0) status1 = DELTA_CalcAngle(x, y, z, deg2rad150, &kat2);
	if (status1 == 0) status1 = DELTA_CalcAngle(x, y, z, deg2rad270, &kat3);
	return status1;
}
void Deg2Imp(void)
{
	imp1 = kat1 * 200000.0/360.0;
	imp2 = kat2 * 200000.0/360.0;
	imp3 = kat3 * 200000.0/360.0;
}
void qpoly3(float t, int tf)
{
	a0 = 0;
	a1 = 0;
	a2 = 3.0/(tf*tf);
	a3 = (-2.0)/(tf*tf*tf);
	
	qt = a0 + a1*t + a2*t*t + a3*t*t*t;
}
void vpoly3(float t, int tf)
{
	a0 = 0;
	a1 = 0;
	a2 = 3.0/(tf*tf);
	a3 = (-2.0)/(tf*tf*tf);
	
	vt = a1 + 2.0*a2*t + 3.0*a3*t*t;
}
void qpoly5(float t, int tf)
{
	a0 = 0;
	a1 = 0;
	a2 = 0;
	a3 = 10.0/(tf*tf*tf);
	a4 = (-15.0)/(tf*tf*tf*tf);
	a5 = 6.0/(tf*tf*tf*tf*tf);
	
	qt = a0 + a1*t + a2*t*t + a3*t*t*t + a4*t*t*t*t + a5*t*t*t*t*t;
}
void vpoly5(float t, int tf)
{
	a0 = 0;
	a1 = 0;
	a2 = 0;
	a3 = 10.0/(tf*tf*tf);
	a4 = (-15.0)/(tf*tf*tf*tf);
	a5 = 6.0/(tf*tf*tf*tf*tf);
	
	vt = a1 + 2.0*a2*t + 3.0*a3*t*t + 4.0*a4*t*t*t + 5.0*a5*t*t*t*t;
}
