#include "nrf24l01.h"
#include "24l01.h"
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//unsigned int rdata;

unsigned char tmp_buf2[32]={0X22};
unsigned char TX_ADDRESS[5]  = {0x34,0x43,0x10,0x10,0x01};//·�ɽڵ��ַ������2-5ͨ��ʱ�ĵ�һ����ַ����
unsigned char RX_ADDRESS[5]  = {0x34,0x43,0x10,0x10,0x01};//·�ɽڵ��ַ������2-5ͨ��ʱ�ĵ�һ����ַ����
unsigned char buf[5]={'1','d','a','4','5'};
unsigned char buf1[5]={'a','b','c','d','e'};
unsigned int a=5,c=5;
unsigned int b=5,d=5;
unsigned int num=0,num1=0;
unsigned int sta=0x55,sta1=0x55;

void delay_loop()
{
    long      i;
    for (i = 0; i <1000 ; i++) {}//1000000
}
void delay()
{
    long      i;
    for (i = 0; i <10 ; i++) {}//1000000
}
void error(void)
{
    asm("     ESTOP0");						// Test failed!! Stop!
    for (;;);
}
void spi_init()
{    
	SpiaRegs.SPICCR.all =0x0007;	   //0x47          // Reset on, rising edge, 16-bit char bits  
	SpiaRegs.SPICTL.all =0x000e;    //0x06		     // Enable master mode, normal phase,
                                                 // enable talk, and SPI int disabled.
	SpiaRegs.SPIBRR =0x0004;									
    SpiaRegs.SPICCR.all =0x0087;		//0xc7         // Relinquish SPI from Reset   
    SpiaRegs.SPIPRI.bit.FREE = 1;                // Set so breakpoints don't disturb xmission
}
void led_init()
{
	EALLOW;
	GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;  // CE
	GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;  // ��������
    GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1;   // ���
	EDIS;
}
//��ʼ��24L01��IO��
void GPIO_Conf_SPI(void)
{
    EALLOW;
	/*GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;  // SCN
	GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;  // ��������
    GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;   // ���
    GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;  // SCN
	GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;  // ��������
    GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;   // ���*/
    
    GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 0;  // CE
	GpioCtrlRegs.GPBPUD.bit.GPIO40 = 0;  // ��������
    GpioCtrlRegs.GPBDIR.bit.GPIO40 = 1;   // ���
    GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 0;  // IRQ
	GpioCtrlRegs.GPBPUD.bit.GPIO41 = 0;  // ��������
    GpioCtrlRegs.GPBDIR.bit.GPIO41 = 0;   // ����
    GpioCtrlRegs.GPBQSEL1.bit.GPIO41 = 0;  //��ϵͳʱ��ͬ��
    EDIS;
}     
void spi_fifo_init()										
{
// Initialize SPI FIFO registers
    SpiaRegs.SPIFFTX.all=0xE040;
    SpiaRegs.SPIFFRX.all=0x204f;
    SpiaRegs.SPIFFCT.all=0x0;
}  
void NRF24L01_Init(void)
{
	
	//spi_fifo_init();  
	//���NRF���ص��޸�SPI������
	//SPI1_SetSpeed(SPI_SPEED_4); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��    // ����ÿ�ζ�����SPI�ٶ�
	InitSpiaGpio();
	GPIO_Conf_SPI();
	spi_init();    		//��ʼ��SPI	
	NRF24L01_CE_L; 			//ʹ��24L01
	//NRF24L01_SCN_H;			//SPIƬѡȡ��	 		 	 
}
void spi_xmit(Uint16 a)
{
	//if(SpiaRegs.SPISTS.bit.BUFFULL_FLAG==1)
       SpiaRegs.SPITXBUF=a;
    //while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG==0)
}    

//SPI ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
unsigned char SPI_ReadWriteByte(unsigned char TxData)
{
	unsigned char rdata=0;
	if(SpiaRegs.SPISTS.bit.BUFFULL_FLAG==0)
    SpiaRegs.SPITXBUF=(TxData<<8);
    while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG==1);
    if(SpiaRegs.SPISTS.bit.INT_FLAG==1)
    rdata = SpiaRegs.SPIRXBUF;
    return rdata;
	/*spi_xmit(TxData);
     // Wait until data is received
     while(SpiaRegs.SPIFFRX.bit.RXFFST !=1) { } 			
     rdata = SpiaRegs.SPIRXBUF;
     return rdata;*/
}
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
unsigned char NRF24L01_Write_Reg(unsigned char reg,unsigned char value)
{
	char status;	
   	//NRF24L01_SCN_L;                 //ʹ��SPI����
  	status =SPI_ReadWriteByte(reg);//���ͼĴ����� 
  	SPI_ReadWriteByte(value);      //д��Ĵ�����ֵ
  	//NRF24L01_SCN_H;                 //��ֹSPI����	   
  	return(status);       			//����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
unsigned char NRF24L01_Read_Reg(unsigned char reg)
{
	char reg_val;	    
// 	NRF24L01_SCN_L;          //ʹ��SPI����		
  	SPI_ReadWriteByte(reg);   //���ͼĴ�����
  	SPI_ReadWriteByte(0XFF);
  	reg_val=SPI_ReadWriteByte(0XFF);//��ȡ�Ĵ�������
//  	NRF24L01_SCN_H;          //��ֹSPI����		    
  	return(reg_val);           //����״ֵ̬
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
unsigned char NRF24L01_Read_Buf(unsigned char reg,unsigned char *pBuf,unsigned char len)
{
	unsigned char status,u8_ctr;       
  	//NRF24L01_SCN_L;           //ʹ��SPI����
  	status=SPI_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   
  	SPI_ReadWriteByte(0XFF);	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI_ReadWriteByte(0XFF);//��������
  	//NRF24L01_SCN_H;       //�ر�SPI����
  	return status;        //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
unsigned char NRF24L01_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char len)
{
	unsigned char status,u8_ctr;	    
 	//NRF24L01_SCN_L;          //ʹ��SPI����
  	status = SPI_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI_ReadWriteByte(*pBuf++); //д������	 
  	//NRF24L01_SCN_H;       //�ر�SPI����
  	return status;          //���ض�����״ֵ̬
}				   
 
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
unsigned char VAL=0;
unsigned char sss=0;
unsigned char NRF24L01_RxPacket(unsigned char *rxbuf,unsigned char*txbuf)
{
	unsigned char sta;	 							   
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ   		
	NRF24L01_Write_Reg(WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//�������
	{	 	 
		VAL=NRF24L01_Read_Reg(0X60);
	//	NRF24L01_Write_Buf(W_ACK_PYLOD,txbuf,VAL);			// ����ACK ���ݰ�
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,VAL);			// ��ȡ�ѽ�������
		NRF24L01_Write_Reg(FLUSH_RX,0xff);					//���RX FIFO�Ĵ��� 
		return 	128;
	}   
	return 1;//û�յ��κ�����
}				    
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
unsigned char NRF24L01_TxPacket(unsigned char *txbuf)
{
	//u8 sta;
 	//SPIx_SetSpeed(SPI_SPEED_8);//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��  
 	 
 	
	NRF24L01_CE_L;delay();//Ҫ��ҪnRF24L01_Flush_TX_FIFO();// ��� FIFO
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,0x12);//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE_H;delay();//��������	   
 	
	while(NRF24L01_IRQ!=0);//�ȴ��������
	delay_loop();
	sta1=NRF24L01_Read_Reg(NRF_FIFO_STATUS);delay();  //��ȡ״̬�Ĵ�����ֵ	
	sta=NRF24L01_Read_Reg(STATUS);delay();  //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(WRITE_REG+STATUS,sta);delay(); //���TX_DS��MAX_RT�жϱ�־
	
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(sta&TX_OK)//�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}

//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������
unsigned char FF1=0,DYN=0; 		   
void NRF24L01_RX_Mode(void)
{
	/*NRF24L01_CE_L;	  
  	NRF24L01_Write_Buf(WRITE_REG+RX_ADDR_P0,(unsigned char*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
  	NRF24L01_Write_Buf(WRITE_REG+TX_ADDR,(unsigned char*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
			  
  	NRF24L01_Write_Reg(WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
	NRF24L01_Write_Reg(WRITE_REG+SETUP_RETR,0X1A);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  	NRF24L01_Write_Reg(WRITE_REG+RF_CH,40);	     //����RFͨ��Ƶ��		
	
    NRF24L01_Write_Reg(NRF_WRITE_REG|NRF_FEATURE , NRD_EN_DPL | NRF_EN_ACK_PAYLOAD);
    if(NRF24L01_Read_Reg(NRF_FEATURE)== 0x00 && NRF24L01_Read_Reg(NRF_DYNPD) == 0x00)
	{
        NRF24L01_Write_Reg(NRF_ACTIVATE, 0x73);
    }
    NRF24L01_Write_Reg(NRF_WRITE_REG|NRF_FEATURE , NRD_EN_DPL | NRF_EN_ACK_PAYLOAD);
    NRF24L01_Write_Reg(NRF_WRITE_REG|NRF_DYNPD, NRF_DPL_P0 | NRF_DPL_P1| NRF_DPL_P2| NRF_DPL_P3| NRF_DPL_P4| NRF_DPL_P5);
    	    	  
  	NRF24L01_Write_Reg(WRITE_REG+RF_SETUP,0x27);//����TX�������,0db����,1Mbps,���������濪��   
  	NRF24L01_Write_Reg(WRITE_REG+CONFIG, 0x0f);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
  	NRF24L01_CE_H; //CEΪ��,�������ģʽ */
  	NRF24L01_Write_Reg(WRITE_REG+EN_AA,0x00);    //ʹ��ͨ��0���Զ�Ӧ��  
  	NRF24L01_Write_Reg(WRITE_REG + EN_RXADDR, 0x00);    // �رս���ͨ��0  
	NRF24L01_Write_Reg(WRITE_REG+SETUP_RETR,0X00);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
}						 
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE_L;	    
  	NRF24L01_Write_Buf(WRITE_REG+TX_ADDR,(unsigned char*)TX_ADDRESS,5);delay();//дTX�ڵ��ַ
  	NRF24L01_Write_Buf(WRITE_REG+RX_ADDR_P0,(unsigned char*)RX_ADDRESS,5);delay(); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK

  	NRF24L01_Write_Reg(WRITE_REG+EN_AA,0x00);delay();     //ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(WRITE_REG+EN_RXADDR,0x00);delay(); //ʹ��ͨ��0�Ľ��յ�ַ  
  	NRF24L01_Write_Reg(WRITE_REG+SETUP_RETR,0x00);delay();//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  	NRF24L01_Write_Reg(WRITE_REG+RF_CH,40);delay();       //����RFͨ��Ϊ40
  	NRF24L01_Write_Reg(WRITE_REG+RF_SETUP,0x07);delay();  //����TX�������,0db����,2Mbps,���������濪��   
  	
  	NRF24L01_Write_Reg(WRITE_REG+CONFIG_24l01,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF24L01_CE_H;delay();//CEΪ��,10us����������
}		  

//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
unsigned char NRF24L01_Check(void)
{
	unsigned char buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	unsigned char i;
	NRF24L01_Write_Buf(WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	delay_loop();
	NRF24L01_Read_Buf(TX_ADDR,buf1,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf1[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
}	
