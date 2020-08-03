/* 
SD ����

����ʱ����ͬ�� 5 ������������Ϣ�Ĳ�������λ������Ƶ����ͨ������ SDIPRE �Ĵ�����Ӧλ�����ơ����Ը�
����Ƶ�����������������ݼĴ���ֵ��

��̹��̣�ͨ�ã�
��� SDI ģ����Ҫ���¼����������裺

��ʼ��
1. ���� SDICON �����ʵ���ʱ�Ӻ��ж�ʹ�ܡ�
2. ���� SDIPRE ����Ϊ�ʵ�ֵ��
3. �ȴ�Ϊ��ʼ������ 74 �� SDCLK ʱ�����ڡ�

CMD ͨ·���
1. д 32 λ��������� SDICmdArg��
2. �����������Ͳ����� SDICmdCon ����������͡�
3. �� SDICmdSta ���ض���־��λʱȷ�� SDICMD ͨ·�����Ľ�����
4. �����������Ϊ����Ӧʱ��־Ϊ CmdSent��
5. ����������ʹ���Ӧʱ��־Ϊ RspFin��
6. ͨ��д'1'����Ӧλ����� SDICmdSta �ı�־��

DAT ͨ·���
1. д���ݳ�ʱʱ�䵽 SDIDTimer��
2. д���С���鳤�ȣ��� SDIBSize��ͨ��Ϊ 0x80 ���֣�
3. ȷ���鷽ʽ�������߻� DMA �Ȳ�ͨ������ SDIDatCon �������ݴ��䡣
4. Tx ���ݡ�д���ݵ����ݼĴ�����SDIDAT�������� Tx FIFO Ϊ���ã�TFDET Ϊ��λ������һ�루TFHalf Ϊ��λ����
	��գ�TFEmpty Ϊ��λ����
5. Rx ���ݡ������ݼĴ�����SDIDAT�������ݣ����� Rx FIFO Ϊ���ã�RFDET Ϊ��λ����������RFFull Ϊ��λ����
	��һ�루RFHalf Ϊ��λ�����ٻ�������ݾ�����RFLast Ϊ��λ����
6. �� SDIDatSta �� DatFin ��־��λʱȷ�� SDI DAT ͨ·�����Ľ�����
7. ͨ��д'1'����Ӧλ����� SDIDatSta �ı�־�� 

*/


/*
// SD��ָ���  	   
#define CMD0    0       //����λ
#define CMD1    1
#define CMD8    8       //����8 ��SEND_IF_COND
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����sector
#define CMD18   18      //����18����Multi sector
#define CMD23   23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define CMD41   41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00
*/

#include <sd.h>
#include <s3c2440_regs.h>
#include <romfs.h>

#define _SD_DEBUG_
#define SDCARD_BUFF_SIZE 512
 
u8 send_buf[SDCARD_BUFF_SIZE];
u8 recv_buf[SDCARD_BUFF_SIZE];
struct sd_info SDCard;
 
int sdi_test()  //���Ժ���
{
	u32 i;

	/* ��ʼ��sd */
	if (sdi_init()) {
		printk("SDI��ʼ��������\r\n");
	} else {
		printk("SDI��ʼ����������ֹ��\r\n");
		return -1;
	}

	/* ����/���ջ�������ʼ�� */
	for (i = 0; i < 512; i++) {
		send_buf[i] = i + 1;
		recv_buf[i] = 0;
	}

	/* д���ݵ�sd */
	if (sd_write_sector((u32 *)send_buf, 0, 1)) {
		printk("\r\nд��SD��0��ַ���ݳɹ���");
	} else {
		printk("\r\nд��SD��0��ַ���ݳ�������ֹ��");
		return -1;
	}
	
	/* ��sd������ */
	if (sd_read_sector((u32 *)recv_buf, 0, 1)) {
		printk("\r\n����SD��0��ַ���ݳɹ���");
		printk("\r\n����������:\r\n");

		for (i = 0; i < 512; i += 4) {
			if (i % 32)
				printk("\r\n");
			
			printk("%d ",recv_buf[i]);
			printk("%d ",recv_buf[i+1]);
			printk("%d ",recv_buf[i+2]);
			printk("%d ",recv_buf[i+3]);
		}
	} else {
		printk("\r\n����SD��0��ַ���ݳ�������ֹ��");
		return -1;
	}

	/* ȡ��Ƭѡ */
//	select_or_deselect(0,SDCard.sdiRCA);

	return 0;
}

/*
 *
 *
 *
 *
 *
 *
 */
u8 sdi_init()
{
	int i;
	
	printk("\r\nSDI��ʼ����ʼ��");
	
	GPEUP  = 0xf83f;   			// The pull up  1111 1000 0011 1111  ��������
    GPECON = 0xaaaaaaaa;  		// 1010 1010 1010 1010 1010 1010 1010 1010
    SDICSTA = 0xffff;   		//SDIָ��״̬
    SDIDSTA = 0xffff;			//SDI����״̬
 
	SDIPRE = 124; 				// 400KHz  ���������� Ƶ�� PCLK/400K -1
    SDICON=(1<<4)|1; 			// Type B,  clk enable SDI����
    SDIFSTA|=1<<16; 			//FIFO reset
	SDIBSIZE=0x200; 			// 512byte(128word)  SDI���С
	SDIDTIMER=0x7fffff; 		// Set timeout count ���ݴ��䳬ʱʱ��
	
	//�ȴ�74��CLK
    for(i=0;i<0x1000;i++);
	
	//��ִ��CMD0,��λ
	CMD0(); 
	
	//�жϿ�������
	if(SDI_MMC_OCR()) 
		SDCard.sdiType = 1;  //��ΪMMC
	else
		SDCard.sdiType = 0;  //��ΪSD
		
	//���SD��
	if (SDI_SD_OCR()) {
		printk("SD is ready\r\n");
	} else{
		printk("Initialize fail\r\nNo Card assertion\r\n");
        return 0;
    }  
	
	//��CID
	if (CMD2(SDCard.cCardCID)) {
		printk("CID\r\n");
		printk("MID = %d\r\n",SDCard.cCardCID[0]);
		printk("OLD = %d\r\n",(SDCard.cCardCID[1]*0X100)+SDCard.cCardCID[2]);
		printk("��������:%s\r\n",(SDCard.cCardCID+3));
		printk("��������:20%d,%d\r\n",((SDCard.cCardCID[13]&0x0f)<<4)+((SDCard.cCardCID[14]&0xf0)>>4),(SDCard.cCardCID[14]&0x0f));
	} else {
		printk("Read Card CID is fail!\r\n");
		return 0;
	}
	
	//����RCA       MMC��RCA=1   SD��RCA=0
	//MMC
	if (SDCard.sdiType==1) {
		if (CMD3(1,&SDCard.sdiRCA)) {
			SDCard.sdiRCA = 1;
			SDIPRE = 2;  //16MHZ
			printk("MMC Card RCA = 0x%x\r\n",SDCard.sdiRCA);
			printk("MMC Frequency is %dHz\r\n",(PCLK/(SDIPRE+1)));
		} else {
			printk("Read MMC RCA is fail!\r\n");
			return 0;
		}
	//SD
	} else {
		if (CMD3(0,&SDCard.sdiRCA)) {
			SDIPRE = 1; // Normal clock=25MHz
			printk("SD Card RCA = 0x%x\r\n",SDCard.sdiRCA);
			printk("SD Frequency is %dHz\r\n",(PCLK/(SDIPRE+1)));
		} else {
			printk("Read SD RCA is fail!\r\n");
			return 0;
		}
	}
	
	//��CSD
	if (CMD9(SDCard.sdiRCA,SDCard.lCardCSD)) {
		SDCard.lCardSize = (((SDCard.lCardCSD[1]&0x0000003f)<<16)+((SDCard.lCardCSD[2]&0xffff0000)>>16)+1)*512;
		SDCard.lSectorSize = ((SDCard.lCardCSD[2]>>6)&0x0000007f)+1;
		printk("Read Card CSD OK!\r\n");
		printk("0x%08x\r\n",SDCard.lCardCSD[0]);
		printk("0x%08x\r\n",SDCard.lCardCSD[1]);
		printk("0x%08x\r\n",SDCard.lCardCSD[2]);
		printk("0x%08x\r\n",SDCard.lCardCSD[3]);
		printk("������Ϊ:%dKB,%dMB\r\n",SDCard.lCardSize,SDCard.lCardSize/1024);
	} else {
		printk("Read Card CSD Fail!\r\n");
		return 0;
	}
	
	//ѡ�п�  CMD7  ���봫��״̬
	//1��ʾѡ�п� 
	if (select_or_deselect(1,SDCard.sdiRCA)) {
		printk("Card sel desel OK!\r\n");
	} else {
		printk("Card sel desel fail!\r\n");
		return 0;
	}
	
	//CMD13 ��ѯ�Ƿ�Ϊ����״̬
	while ((CMD13(SDCard.sdiRCA) & 0x1e00) != 0x800);

	//�������ߴ��� ACMD6
	if (Set_bus_Width(SDCard.sdiType,1,SDCard.sdiRCA)) {
		SDCard.sdiWide = 1;
		printk("Bus Width is 4bit\r\n");
	} else {
		SDCard.sdiWide = 0;
		printk("Bus Width is 1bit\r\n");
	}
	
	return 1;
}
 
//���CMD�Ƿ����
u32 SDI_Check_CMD_End(int cmd, int be_resp) 
{
    int finish0;

	// No response   
    if (!be_resp) {
     	finish0 = SDICSTA;		

		while ((finish0 & 0x800) != 0x800) // Check cmd end		
   			finish0 = SDICSTA;
 
		SDICSTA = finish0;		// Clear cmd end state
	 
		printk("%x\r\n", finish0);
		return 1;
		
	// With response
	} else {
	    finish0 = SDICSTA;

		while (!( ((finish0 & 0x200) == 0x200) | ((finish0 & 0x400) == 0x400) ))    // Check cmd/rsp end
	        finish0=SDICSTA;
	 
//		printk("CMD%d:SDICSTA=0x%x, SDIRSP0=0x%x\r\n", cmd, SDICSTA, SDIRSP0);   

		// CRC no check
		if (cmd==1 | cmd==9 | cmd==41) {
		   	// Check error
		   	if ( (finish0&0xf00) != 0xa00 ) {
				SDICSTA=finish0;   // Clear error state 
			 
				if (((finish0&0x400)==0x400)) {
					printk("CMD%d Time out!\r\n", cmd);
				    return 0; // Timeout error     
				}
			}
			
			SDICSTA=finish0; // Clear cmd & rsp end state
			// printk("%x\r\n", finish0);

		// CRC check
		} else {
			// Check error
			if ( (finish0&0x1f00) != 0xa00 ) { 
				SDICSTA=finish0;   // Clear error state
			 
				if (((finish0&0x400)==0x400)) {
					printk("CMD%d Time out!\r\n", cmd);
				    return 0; // Timeout error
				}
			}
			SDICSTA=finish0;
		}
		
		return 1;
	}

	return 1;
}
 
//��λ��ʹ������IDEL״̬
void CMD0(void)
{
	SDICARG = 0x0; 
	SDICCON = (1<<8)|0x40; // No_resp, start
 
	SDI_Check_CMD_End(0, 0);
	SDICSTA = 0x800; // Clear cmd_end(no rsp)
}
 
//���ù�����ѹ�Ǹ���SD��OCR�Ĵ���������
u8 CMD1(void)
{
	SDICARG = 0xff8000; //(SD OCR:2.7V~3.6V)
	SDICCON = (0x1<<9)|(0x1<<8)|0x41; //sht_resp, wait_resp, start, 
	 
	if(SDI_Check_CMD_End(1, 1)) //[31]:Card Power up status bit (busy)
	{
	if((SDIRSP0>>16)==0x80ff)
	{
	SDICSTA = 0xa00; // Clear cmd_end(with rsp)
	return 1; // Success
	}
	else
	return 0;
	}
	return 0;
}

//�����豸��CMD�ϴ���CID
u8 CMD2(u8 *cCID_Info) 
{
	SDICARG = 0x0;
	SDICCON = (0x1<<10)|(0x1<<9)|(0x1<<8)|0x42; //lng_resp, wait_resp, start
	 
	if(!SDI_Check_CMD_End(2, 1)) 
	return 0;
	*(cCID_Info+0) = SDIRSP0>>24;
	*(cCID_Info+1) = SDIRSP0>>16;
	*(cCID_Info+2) = SDIRSP0>>8;
	*(cCID_Info+3) = SDIRSP0;
	*(cCID_Info+4) = SDIRSP1>>24;
	*(cCID_Info+5) = SDIRSP1>>16;
	*(cCID_Info+6) = SDIRSP1>>8;
	*(cCID_Info+7) = SDIRSP1;
	*(cCID_Info+8) = SDIRSP2>>24;
	*(cCID_Info+9) = SDIRSP2>>16;
	*(cCID_Info+10) = SDIRSP2>>8;
	*(cCID_Info+11) = SDIRSP2;
	*(cCID_Info+12) = SDIRSP3>>24;
	*(cCID_Info+13) = SDIRSP3>>16;
	*(cCID_Info+14) = SDIRSP3>>8;
	*(cCID_Info+15) = SDIRSP3;
	SDICSTA = 0xa00; // Clear cmd_end(with rsp)
	return 1;
}

//��SD���趨һ����Ե�ַ��Ҳ����Ѱַ�ĵ�ַ = 0:SD����=1:MMC�� =0 ʧ�� =1 �ɹ�
u8 CMD3(u16 iCardType,u16 *iRCA) 
{
	SDICARG = iCardType<<16;     // (MMC:Set RCA, SD:Ask RCA-->SBZ)
	SDICCON = (0x1<<9)|(0x1<<8)|0x43; // sht_resp, wait_resp, start
	 
	if(!SDI_Check_CMD_End(3, 1)) 
	return 0;
	SDICSTA=0xa00; // Clear cmd_end(with rsp)
	 
	if(iCardType)
	{
	*iRCA = 1;
	}
	else 
	    {
	*iRCA =( SDIRSP0 & 0xffff0000 )>>16;
	}
	if( SDIRSP0 & 0x1e00!=0x600 )   // CURRENT_STATE check
	return 0;
	else
	return 1;
}
 
//ѡ�п����߽��ѡ�� cSorD=1Ϊѡ�� Ϊ0����ѡ��
u8 CMD7(u8 cSorD,u16 iRCA) 
{
	if(cSorD)
	{
	SDICARG = iRCA<<16; // (RCA,stuff bit)
	SDICCON = (0x1<<9)|(0x1<<8)|0x47;   // sht_resp, wait_resp, start
	if(!SDI_Check_CMD_End(7, 1))
	return 0;
	SDICSTA = 0xa00; // Clear cmd_end(with rsp)
	//--State(transfer) check
	if( SDIRSP0 & 0x1e00!=0x800 )
	return 0;
	else
	return 1;
	}
	else
	{
	SDICARG = 0<<16; //(RCA,stuff bit)
	SDICCON = (0x1<<8)|0x47; //no_resp, start
	 
	if(!SDI_Check_CMD_End(7, 0))
	return 0;
	SDICSTA = 0x800; //Clear cmd_end(no rsp)
	return 1;
	}
}

//��ȡ����CSD�Ĵ�����ֵ
u8 CMD9(u16 iRCA,u32 *lCSD) 
{
	SDICARG = iRCA<<16; // (RCA,stuff bit)
	SDICCON = (0x1<<10)|(0x1<<9)|(0x1<<8)|0x49; // long_resp, wait_resp, start
	 
	if(!SDI_Check_CMD_End(9, 1)) 
	return 0;
	 
	*(lCSD+0) = SDIRSP0;
	*(lCSD+1) = SDIRSP1;
	*(lCSD+2) = SDIRSP2;
	*(lCSD+3) = SDIRSP3;
	return 1;
}
 
//ֹͣ���ݴ���
u8 CMD12(void) 
{
	SDICARG = 0x0;    
	SDICCON = (0x1<<9)|(0x1<<8)|0x4c; //sht_resp, wait_resp, start,
	 
	if(!SDI_Check_CMD_End(12, 1)) 
	return 0;
	else
	SDICSTA = 0xa00; //Clear cmd_end(with rsp)
	return 1;
}

//��ȡ����״̬
u16 CMD13(u16 iRCA) 
{
	SDICARG = iRCA<<16; // (RCA,stuff bit)
	SDICCON = (0x1<<9)|(0x1<<8)|0x4d; // sht_resp, wait_resp, start
	 
	if(!SDI_Check_CMD_End(13, 1)) 
	return 0;
	 
	SDICSTA=0xa00; // Clear cmd_end(with rsp)
	return SDIRSP0;
}
 
//��ȡһ�����ݿ�
u8 CMD17(u32 addr) 
{
    //STEP1:����ָ�� 
    SDICARG = addr; //�趨ָ����� 
    SDICCON = (1<<9)|(1<<8)|0x51; //����CMD17ָ��
    
    if(SDI_Check_CMD_End(17,1))
     return 1;
    else
     return 0;
}
 
//��ȡ������ݿ�
u8 CMD18(u32 addr) 
{
    //STEP1:����ָ�� 
    SDICARG = addr; //�趨ָ����� 
    SDICCON = (1<<9)|(1<<8)|0x52; //����CMD18ָ��
    
    if(SDI_Check_CMD_End(18,1))
     	return 1;
    else
     	return 0;
}
 
//д��һ�����ݿ�
u8 CMD24(u32 addr) 
{
    //STEP1:����ָ�� 
    SDICARG = addr; //�趨ָ����� 
    SDICCON = (1 << 9) | (1 << 8) | 0x58; //����CMD24ָ��
    
    if (SDI_Check_CMD_End(24, 1))
     	return 1;
    else
     	return 0;
}

//д�������ݿ�
u8 CMD25(u32 addr) 
{
    //STEP1:����ָ�� 
    SDICARG = addr; //�趨ָ����� 
    SDICCON = (1<<9)|(1<<8)|0x59; //����CMD25ָ��
    
    if(SDI_Check_CMD_End(25,1))
     return 1;
    else
     return 0;
}

//����Ƿ��п���ִ��ACMD������ִ��CMD55
u8 CMD55(u16 iRCA) 
{
	SDICARG = iRCA<<16;
	SDICCON = (0x1<<9)|(0x1<<8)|0x77; //sht_resp, wait_resp, start
	 
	if(!SDI_Check_CMD_End(55, 1)) 
	return 0;
	SDICSTA = 0xa00; // Clear cmd_end(with rsp)
	return 1;
}

// ACMD6����Ϊ�������ߴ��� [1:0] 00Ϊ1bit  10Ϊ4bit
u8 ACMD6(u8 BusWidth,u16 iRCA) 
{
	if(!CMD55(iRCA))
	return 0;
	 
	SDICARG = BusWidth<<1;     //Wide 0: 1bit, 1: 4bit
	SDICCON = (0x1<<9)|(0x1<<8)|0x46; //sht_resp, wait_resp, start
	 
	if(!SDI_Check_CMD_End(6, 1)) 
	return 0;
	SDICSTA=0xa00;     // Clear cmd_end(with rsp)
	return 1;
}
 
//����Ƿ�ΪSD�������� =0Ӧ�������߿���æ  =1��׼SD�� =2SDHC V2.0
u8 ACMD41(u16 iRCA) 
{
	u8 cReturn;
	if(!CMD55(iRCA)) 
	return 0;
	SDICARG=0x40ff8000; //ACMD41(SD OCR:2.7V~3.6V)
	SDICCON=(0x1<<9)|(0x1<<8)|0x69;//sht_resp, wait_resp, start, ACMD41
	 
	if(SDI_Check_CMD_End(41, 1)) 
	{
	if(SDIRSP0==0xc0ff8000)
	cReturn = 2; //SDHC
	else if(SDIRSP0==0x80ff8000)
	cReturn = 1; //��׼SD
	else
	cReturn = 0; //Ӧ�����
	SDICSTA = 0xa00; // Clear cmd_end(with rsp)
	return cReturn; // Success    
	}
	SDICSTA = 0xa00; // Clear cmd_end(with rsp)
	return 0;
}

//���MMC��
u32 SDI_MMC_OCR(void)
{
    int i;
    //-- Negotiate operating condition for MMC, it makes card ready state
    for(i=0;i<10;i++)
    {
if(CMD1())
return 1;
    }
    return 0; // Fail
}

//���SD��
u32 SDI_SD_OCR(void)
{
    int i;
 
    SDCard.sdiRCA = 0;
    for(i=0;i<50;i++)
    {
     if(ACMD41(SDCard.sdiRCA))
	return 1;
	file_delay(1000);
	    }
	return 0;  //fail
}

//����CMD7��ѡ�л���ѡ�п�������1��ɹ���0ʧ��
u8 select_or_deselect(u8 cSelDesel,u16 iCardRCA)
{
	if (CMD7(cSelDesel,iCardRCA))
		return 1;
	else
		return 0;
}

//�������ߴ���
u8 Set_bus_Width(u8 cCardType,u8 cBusWidth,u16 iRCA)
{
	if(cCardType==1) //MMC������0����Ҫ���� Ĭ��Ϊ1bit���ߴ���
		return 0;
	return ACMD6(cBusWidth,iRCA);
}
 
 
/*
 * sd_read_sector - ��SD���ж���ָ������ʼ��ַ�ĵ����������ݿ�
 *
 * @addr  ���������ʼ��ַ
 * @buffer ���ڽ��ն������ݵĻ�����
 * @block_num ���Ŀ���
 *
 * @return 
 *		0 ����������ɹ�
 *		1 ��������ɹ�
 *
 */
u8 sd_read_sector(u32 *buf, u32 addr, u32 block_num)
{
	u32 i=0;
	u32 status=0;
	 
	SDIDTIMER = 0x7fffff; 				// Set timeout count
	SDIBSIZE = 0x200; 					// 512byte(128word)
	SDIFSTA = SDIFSTA | (1 << 16); 		// FIFO reset
	SDIDCON = (block_num << 0) | (2 << 12) | (1 << 14) | (SDCard.sdiWide << 16) | (1 << 17) | (1 << 19) | (2 << 22);

//	printk("enter sd_read_sector(): src_block = %d, block_num = %d\r\n", addr, block_num);
	
	//���Ͷ������ָ��
	while (CMD18(addr) != 1)		
		SDICSTA = 0xF << 9;

	//��ʼ�������ݵ�������
	while (i < block_num * 128) { 
		//����Ƿ�ʱ��CRCУ���Ƿ����
		if (SDIDSTA & 0x60) { 
			//�����ʱ��־��CRC�����־
			SDIDSTA = (0x3 << 0x5); 
			return -1;
		}
		
		status = SDIFSTA;
		//�������FIFO��������
		if ((status & 0x1000) == 0x1000) { 
			*buf = SDIDAT;
//			*buf = be32_to_le32(*buf);

			//printk("sd_read_sector(): *buf = %x\r\n", *buf);
			buf++;
			i++;
		}
	}
	 
	SDIDCON = SDIDCON & ~(7 << 12);
	SDIFSTA = SDIFSTA & 0x200;			//Clear Rx FIFO Last data Ready 
	SDIDSTA = 0x10;						//Clear data Tx/Rx end detect 

	//���ͽ���ָ�� 
	while (CMD12() != 1)				
		SDICSTA = 0xF << 9;
//	printk("leave sd_read_sector(): src_block = %d, block_num = %d\r\n", addr, block_num);
	 
	return 0;
}

 
/*
 * sd_write_sector - ��SD����һ���������ݿ�д������
 *
 * @addr  ��д�����ʼ��ַ
 * @buffer ���ڷ������ݵĻ�����
 * @block_num ����
 *
 * @return 
 *		0 ����д�����ʧ��
 *		1 ����д������ɹ�
 *
 */
u8 sd_write_sector(u32 *buf, u32 addr, u32 block_num)
{
	u16 i = 0;
	u32 status = 0;
	 
	SDIDTIMER = 0x7fffff; 				// Set timeout count
	SDIBSIZE = 0x200; 					// 512byte(128word)
	SDIFSTA = SDIFSTA | (1 << 16); 		// FIFO reset
	SDIDCON = (block_num << 0) | (3 << 12) | (1 << 14) | (1 << 16) | (1 << 17) | (1 << 20) | (2 << 22);

	//����д�����ָ��
	while (CMD25(addr) != 1)
		SDICSTA = 0xF << 9;

	//��ʼ�������ݵ�������
	while (i < block_num * 128) { 
		status = SDIFSTA;
		
		//�������FIFO���ã���FIFOδ��
		if ((status & 0x2000) == 0x2000) {
			SDIDAT = *buf;
			buf++;
			i++;
		}
	}
	
	SDIDCON = SDIDCON & ~(7 << 12);

	//���ͽ���ָ�� 
	while (CMD12() != 1)
		SDICSTA=0xF<<9;

	//�ȴ����ݷ��ͽ���
	do { 
		status = SDIDSTA;
	} while ((status & 0x2) == 0x2);

	SDIDSTA = status; 
	SDIDSTA = 0xf4;

	return 0;
}
 
void file_delay(u32 i)
{
	while (i--);
}

