#include<reg52.h>
#include<UART.h>
#include<config.h>

typedef unsigned char uint8;
typedef unsigned int uint16;
unsigned char T0H, T0L;		//定时器参数存储变量
void Timer0_Init(unsigned char ms);
extern void LCD1602_Init();
extern void LCD1602_Show(uint8 x,uint8 y,uint8 *dat,uint8 len); //LCD1602显示字符串（x：横坐标，y：纵坐标，dat：要显示的字符数组，len：要显示的长度）

void UART_Action(unsigned char *dat, unsigned char len)
{
	unsigned char datout[4] = {0};
	if(((dat[0] == dat[6])||(dat[0] == dat[11]))&&(dat[0]>='0' && dat[0]<='9'))
		datout[0] = dat[0];
	else if((dat[6] == dat[11]) && (dat[6]>='0' && dat[6]<='9'))
		datout[0] = dat[6];
	else return;
	if(((dat[1] == dat[7])||(dat[1] == dat[12]))&&(dat[1]>='0' && dat[1]<='9'))
		datout[1] = dat[1];
	else if((dat[7] == dat[12]) && (dat[7]>='0' && dat[7]<='9'))
		datout[1] = dat[7];
	else return;
	if(((dat[2] == dat[8])||(dat[2] == dat[13]))&&(dat[2]=='.'))
		datout[2] = dat[2];
	else if((dat[8] == dat[13]) && (dat[8]=='.'))
		datout[2] = dat[8];
	else return;
	if(((dat[3] == dat[9])||(dat[3] == dat[14]))&&(dat[3]>='0' && dat[3]<='9'))
		datout[3] = dat[3];
	else if((dat[9] == dat[14]) && (dat[9]>='0' && dat[9]<='9'))
		datout[3] = dat[9];
	else return;	
	LCD1602_Clear();
	LCD1602_Show(0,0,datout,4);
}

int main()
{
	UART_Conf(600);		//配置串口波特率为9600
	Timer0_Init(1);			//配置定时器中断发生频率为1ms一次
	LCD1602_Init();
	while(1)
	{
		UART_Driver();		//监控串口
	}
}

void Timer0_Init(unsigned char ms)
{
	unsigned int temp;						//用于计算的中间变量
	temp = 65536 - ms * XTAL * 1000 / 12;	//计算定时器初值，并配置习惯寄存器
	T0H = (unsigned char)(temp >> 8);
	T0L = (unsigned char)(temp);
	TH0 = T0H;
	TL0 = T0L;
	TMOD &= 0XF0;							//配置定时器工作模式为模式1
	TMOD |= 0X01;
	ET0 = 1;								//使能定时器0中断
	TR0 = 1;								//使能定时器0
}
void Interrupt_Timer0() interrupt 1
{
	TH0 = T0H;				//定时器寄存器赋初值
	TL0 = T0L;
	UART_RxMonitor(1);		//串口通信监控
}