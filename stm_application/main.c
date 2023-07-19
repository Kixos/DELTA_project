#include "functions.h"

extern int32_t imp1;
extern int32_t imp2;
extern int32_t imp3;
char auf[200];
char buf[200];
char cuf[200];
char duf[100];
float x = 0;
float y = 0;
float z = 400;
extern uint8_t DMABufRX[100];
extern uint8_t BufRead[100];
int len1 = 0;
int len2 = 0;
int len3 = 0;
int len4 = 0;
int cnt = 0;
float max_vel = 30;
float vel1, vel2, vel3;
int32_t prev_imp1, prev_imp2, prev_imp3;
float dif1, dif2, dif3;
float prev_x = 0;
float prev_y = 0;
float prev_z = 400;
float vector = 0;
float step_x, step_y, step_z;
extern char pfb[100];
extern int pom;
extern int backimp1, backimp2, backimp3;
float tabqt[500];
extern float qt;
float x_poly3, y_poly3, z_poly3;
float x_poly5, y_poly5, z_poly5;
float dirX, dirY, dirZ;
float len0;
int traj = 1;

int main(void)
{
	SysTick_Config(16000000 / 1000);
	Led_Conf();
	Button_Conf();
	USART2ConfDMA();
	USART3ConfDMA();


	while(1)	
	{	
// home		
		if(DMABufRX[0] == 'A')
		{
			DMABufRX[0] = 0;
			len1 = sprintf(auf,"\\1\r\nHOMETYPE 35\r\nHOMEOFFSET 107502\r\nHOMECMD\r\n\\2\r\nHOMETYPE 35\r\nHOMEOFFSET 107502\r\nHOMECMD\r\n\\3\r\nHOMETYPE 35\r\nHOMEOFFSET 107502\r\nHOMECMD\r\n");
			USART2SendDMA(auf,len1);
		}
		if(DMABufRX[0] == 'B')
		{
			DMABufRX[0] = 0;
			len1 = sprintf(auf,"\\1\r\nMOVEABS 98724 20\r\n\\2\r\nMOVEABS 98724 20\r\n\\3\r\nMOVEABS 98724 20\r\n");
			USART2SendDMA(auf,len1);
			imp1 = 98724;
			imp2 = 98724;
			imp3 = 98724;
			x = 0;
			y = 0;
			z = 400;
		}
		if(DMABufRX[0] == 'C')
		{
			DMABufRX[0] = 0;
			pom = 1;
			len3 = sprintf(cuf,"\\1\r\nPFB\r\n");
			USART2SendDMA(cuf,len3);
			delay_ms(10);
			len3 = sprintf(cuf,"\\2\r\nPFB\r\n");
			USART2SendDMA(cuf,len3);
			delay_ms(10);
			len3 = sprintf(cuf,"\\3\r\nPFB\r\n");
			USART2SendDMA(cuf,len3);	
		}
		if(DMABufRX[0] == 'D')
		{
			DMABufRX[0] = 0;	
			len4 = sprintf(duf,"\\1\r\nEN\r\n\\2\r\nEN\r\n\\3\r\nEN\r\n");
			USART2SendDMA(duf,len4);
		}
		if(DMABufRX[0] == 'U')
		{
			DMABufRX[0] = 0;	
			traj = 1;
		}
		if(DMABufRX[0] == 'R')
		{
			DMABufRX[0] = 0;	
			traj = 3;
		}
		if(DMABufRX[0] == 'T')
		{
			DMABufRX[0] = 0;	
			traj = 5;
		}
// position		
		if(DMABufRX[0] == 'F')
		{		
			prev_x = x;
			prev_y = y;
			prev_z = z;
			
			for(int i=0;i<19;i++)
			{
				if(DMABufRX[i] == ' ')
				{
					if(cnt == 1)
					{
						x = (DMABufRX[i-1] - '0');
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-2] != '-' && DMABufRX[i-2] != 'G')
						{
							x += ((DMABufRX[i-2] - '0') * 10);
						}
						if(DMABufRX[i-2] == '-')
						{
							x = -x;
						}
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-3] != ' ' && DMABufRX[i-2] != 'G' && DMABufRX[i-3] != 'G' && DMABufRX[i-2] != '-' && DMABufRX[i-3] != '-')
						{
							x += ((DMABufRX[i-3] - '0') * 100);
						}
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-3] != ' ' && DMABufRX[i-2] != 'G' && DMABufRX[i-3] != 'G' &&  DMABufRX[i-2] != '-'&& DMABufRX[i-3] == '-')
						{
							x = -x;
						}
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-3] != ' ' && DMABufRX[i-2] != 'G' && DMABufRX[i-3] != 'G' && DMABufRX[i-2] != '-' && DMABufRX[i-3] != '-' && DMABufRX[i-4] == '-')
						{
							x = -x;
						}
					}
					if(cnt == 2)
					{
						y = (DMABufRX[i-1] - '0');
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-2] != '-')
						{
							y += ((DMABufRX[i-2] - '0') * 10);
						}
						if(DMABufRX[i-2] == '-')
						{
							y = -y;
						}
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-3] != ' ' && DMABufRX[i-2] != '-' && DMABufRX[i-3] != '-')
						{
							y += ((DMABufRX[i-3] - '0') * 100);
						}
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-3] != ' ' && DMABufRX[i-2] != '-' && DMABufRX[i-3] == '-')
						{
							y = -y;
						}
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-3] != ' ' && DMABufRX[i-2] != '-' && DMABufRX[i-3] != '-' && DMABufRX[i-4] == '-')
						{
							y = -y;
						}
					}
					if(cnt == 3)
					{
						z = (DMABufRX[i-1] - '0');
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-2] != '-')
						{
							z += ((DMABufRX[i-2] - '0') * 10);
						}
						if(DMABufRX[i-2] == '-')
						{
							z = -z;
						}
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-3] != ' ' && DMABufRX[i-2] != '-' && DMABufRX[i-3] != '-')
						{
							z += ((DMABufRX[i-3] - '0') * 100);
						}
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-3] != ' ' && DMABufRX[i-2] != '-' && DMABufRX[i-3] == '-')
						{
							z = -z;
						}
						if(DMABufRX[i-2] != ' ' && DMABufRX[i-3] != ' ' && DMABufRX[i-2] != '-' && DMABufRX[i-3] != '-' && DMABufRX[i-4] == '-')
						{
							z = -z;
						}
					}
					cnt++;
				}
				if(DMABufRX[i] == 'H')
				{
					for(int j=0;j<40;j++)
					{
						DMABufRX[j] = 0;
					}
					cnt = 0;
				}
			}
		}
// maximum velocity		
		if(DMABufRX[0] == 'V')
		{
			for(int i=0;i<5;i++)
			{
					if(DMABufRX[i] == 'H')
					{
						max_vel = (DMABufRX[i-1] - '0');
						if(DMABufRX[i-2] != 'V')
						{
							max_vel += ((DMABufRX[i-2] - '0') * 10);
						}
						if(DMABufRX[i-2] != 'V' && DMABufRX[i-3] != 'V')
						{
							max_vel += ((DMABufRX[i-3] - '0') * 100);
						}
						for(int j=0;j<19;j++)
						{
							DMABufRX[j] = 0;
						}
					}
				}
		}
// INTERPOLACJA
		if(Button_Read() && traj == 1)
		{
			prev_imp1 = imp1;
			prev_imp2 = imp2;
			prev_imp3 = imp3;

			vector = (float)(sqrt((x-prev_x)*(x-prev_x)+(y-prev_y)*(y-prev_y)+(z-prev_z)*(z-prev_z)));
			
			if(vector > 0)
			{
				for(float i=1;i<=(vector);i+=(vector/(int)vector))
				{
					step_x = (float)(i*(x-prev_x)/vector);
					step_y = (float)(i*(y-prev_y)/vector);
					step_z = (float)(i*(z-prev_z)/vector);
					
					prev_imp1 = imp1;
					prev_imp2 = imp2;
					prev_imp3 = imp3;
					
					DELTA_Inverse(prev_x+step_x,prev_y+step_y,prev_z+step_z);
					Deg2Imp();

					dif1 = abs(imp1 - prev_imp1);
					dif2 = abs(imp2 - prev_imp2);
					dif3 = abs(imp3 - prev_imp3);
					

					vel1 = 20.0*60.0*dif1/0.05/10000.0;
					vel2 = 20.0*60.0*dif2/0.05/10000.0;
					vel3 = 20.0*60.0*dif3/0.05/10000.0;
				
					len3 = sprintf(cuf,"\\1\r\nMOVEABS %d %.3f\r\n\\2\r\nMOVEABS %d %.3f\r\n\\3\r\nMOVEABS %d %.3f\r\n",imp1,vel1,imp2,vel2,imp3,vel3);			
					USART2SendDMA(cuf,len3);
					delay_ms(50);
				}
				
				Led_OnOff(r,LedTog);
				Led_OnOff(g,LedTog);
				Led_OnOff(b,LedTog);
				delay_ms(2000);
			}
		}	
// POLY3
		if(Button_Read() && traj == 3)
		{
			prev_imp1 = imp1;
			prev_imp2 = imp2;
			prev_imp3 = imp3;

			vector = (float)(sqrt((x-prev_x)*(x-prev_x)+(y-prev_y)*(y-prev_y)+(z-prev_z)*(z-prev_z)));
			
			if(vector > 0)
			{
				for(float t=0;t<5;t+=0.05)
				{
					qpoly3(t,5);
					tabqt[(int)(t*100.0)] = qt;	
					
					x_poly3 = prev_x + (x-prev_x)*qt;
					y_poly3 = prev_y + (y-prev_y)*qt;
					z_poly3 = prev_z + (z-prev_z)*qt;
					
					prev_imp1 = imp1;
					prev_imp2 = imp2;
					prev_imp3 = imp3;
					
					DELTA_Inverse(x_poly3,y_poly3,z_poly3);
					Deg2Imp();
					
					dif1 = abs(imp1 - prev_imp1);
					dif2 = abs(imp2 - prev_imp2);
					dif3 = abs(imp3 - prev_imp3);
					
					vel1 = 20.0*60.0*dif1/0.05/10000.0;
					vel2 = 20.0*60.0*dif2/0.05/10000.0;
					vel3 = 20.0*60.0*dif3/0.05/10000.0;

					len3 = sprintf(cuf,"\\1\r\nMOVEABS %d %.3f\r\n\\2\r\nMOVEABS %d %.3f\r\n\\3\r\nMOVEABS %d %.3f\r\n",imp1,vel1,imp2,vel2,imp3,vel3);			
					USART2SendDMA(cuf,len3);
					delay_ms(50);					
				}

				Led_OnOff(r,LedTog);
				Led_OnOff(g,LedTog);
				Led_OnOff(b,LedTog);
				delay_ms(2000);
			}
		}	
// POLY5
		if(Button_Read() && traj == 5)
		{
			prev_imp1 = imp1;
			prev_imp2 = imp2;
			prev_imp3 = imp3;

			vector = (float)(sqrt((x-prev_x)*(x-prev_x)+(y-prev_y)*(y-prev_y)+(z-prev_z)*(z-prev_z)));
			
			if(vector > 0)
			{
				for(float t=0;t<5;t+=0.05)
				{
					qpoly5(t,5);
					tabqt[(int)(t*100.0)] = qt;	
					
					x_poly5 = prev_x + (x-prev_x)*qt;
					y_poly5 = prev_y + (y-prev_y)*qt;
					z_poly5 = prev_z + (z-prev_z)*qt;
					
					prev_imp1 = imp1;
					prev_imp2 = imp2;
					prev_imp3 = imp3;
					
					DELTA_Inverse(x_poly5,y_poly5,z_poly5);
					Deg2Imp();	
					
					dif1 = abs(imp1 - prev_imp1);
					dif2 = abs(imp2 - prev_imp2);
					dif3 = abs(imp3 - prev_imp3);
					
					vel1 = 20.0*60.0*dif1/0.05/10000.0;
					vel2 = 20.0*60.0*dif2/0.05/10000.0;
					vel3 = 20.0*60.0*dif3/0.05/10000.0;

					len3 = sprintf(cuf,"\\1\r\nMOVEABS %d %.3f\r\n\\2\r\nMOVEABS %d %.3f\r\n\\3\r\nMOVEABS %d %.3f\r\n",imp1,vel1,imp2,vel2,imp3,vel3);			
					USART2SendDMA(cuf,len3);
					delay_ms(50);					
				}

				Led_OnOff(r,LedTog);
				Led_OnOff(g,LedTog);
				Led_OnOff(b,LedTog);
				delay_ms(2000);
			}
		}	
	delay_ms(100);
	}
}
