#include <s3c2440_regs.h>
#include <uart.h>

#define	OUTBUFSIZE	1024
#define	INBUFSIZE		1024

static unsigned char g_pcOutBuf[OUTBUFSIZE];
static unsigned char g_pcInBuf[INBUFSIZE];

/*
 * 初始化UART0
 * 115200, 8N1, 无流控
 */
void uart0_init(void)
{
    GPHCON  |= 0xa0;    // GPH2,GPH3用作TXD0,RXD0
    GPHUP   = 0x0c;     // GPH2,GPH3内部上拉

    ULCON0  = 0x03;     // 8N1(8个数据位，无较验，1个停止位)
    UCON0   = 0x05;     // 查询方式，UART时钟源为PCLK
    UFCON0  = 0x00;     // 不使用FIFO
    UMCON0  = 0x00;     // 不使用流控
    UBRDIV0 = UART_BRD; // 波特率为115200
}

void putc(unsigned char c)
{
	while (!(UTRSTAT0 & (1 << 2)));

	UTXH0 = c;
}

unsigned char getc(void)
{
	while (!(UTRSTAT0 & (1)));

	return URXH0;
}

void puts(unsigned char *s)
{
	while (*s != '\0')
		putc(*s++);
}

unsigned char *gets(unsigned char *s)
{
	while (*s != '\r')
		*s++ = getc();

	return s;
}
/*
int printf(const char *fmt, ...)
{
	int i;
	int len;
	va_list args;

	va_start(args, fmt);
	len = vsprintf(g_pcOutBuf,fmt,args);
	va_end(args);
	for (i = 0; i < strlen(g_pcOutBuf); i++)
	{
		putc(g_pcOutBuf[i]);
	}
	return len;
}

int scanf(const char * fmt, ...)
{
	int i = 0;
	unsigned char c;
	va_list args;
	
	while(1)
	{
		c = getc();
		if((c == 0x0d) || (c == 0x0a))
		{
			g_pcInBuf[i] = '\0';
			break;
		}
		else
		{
			g_pcInBuf[i++] = c;
		}
	}
	
	va_start(args,fmt);
	i = vsscanf(g_pcInBuf,fmt,args);
	va_end(args);

	return i;
}
*/

