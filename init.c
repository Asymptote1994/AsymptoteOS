#include <s3c2440_regs.h>

void led_init()
{
	GPFCON |= 1 << 8;
	GPFCON |= 1 << 10;
	GPFCON |= 1 << 12;

	GPFDAT = 0xff;
}

void delay(void)
{
	/* 由于使用了-O2优化编译选项，volatile的作用在于
	   告诉编译器空循环不要优化，否则会被优化为空语句 */
	volatile unsigned long i, j;

	for (i = 0; i < 300; ++i)
		for (j = 0; j < 300; ++j);
}

/*
 * Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}
 * {prescaler value} = 0~255
 * {divider value} = 2, 4, 8, 16
 * 本实验的Timer0的时钟频率=100MHz/(99+1)/(16)=62500Hz
 * 设置Timer0 0.5秒钟触发一次中断：
 */
void timer0_init(void)
{
    TCFG0  = 99;        // 预分频器0 = 99        
    TCFG1  = 0x03;      // 选择16分频
    TCNTB0 = 6250;     // 0.1秒钟触发一次中断
    TCON   |= (1<<1);   // 手动更新
    TCON   = 0x09;      // 自动加载，清“手动更新”位，启动定时器0

    // 定时器0中断使能
    INTMSK   &= (~(1<<10));
}

void key_init(void)
{
	// S2,S3对应的2根引脚设为中断引脚 EINT0,ENT2
    	GPFCON &= ~((3<<(0*2)) | (3<<(2*2)));
    	GPFCON |= (0x2<<(0*2)) | (0x2<<(2*2));

    /*
     * 设定优先级：
     * ARB_SEL0 = 00b, ARB_MODE0 = 0: REQ1 > REQ3，即EINT0 > EINT2
     * 仲裁器1、6无需设置
     * 最终：
     * EINT0 > EINT2 > EINT11即K2 > K3 > K4
     */
    PRIORITY = (PRIORITY & ((~0x01) | (0x3<<7))) | (0x0 << 7) ;
    

    // EINT0、EINT2 中断使能
    INTMSK &= (~(1<<0)) & (~(1<<2));
}










