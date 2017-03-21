#include "MPU6050.h"
#include "I2C.h"
#include "define.h"
#include "control.h"
#include "hardware_I2C.h"
#include "imu.h"
#include "math.h"

unsigned char mpu6050_buffer[14]={0};
int mpu6050buffer[14]={0};
#define Gyr_Gain 0.061;
extern struct DATA_XYZ ACC_OFFSET;
extern struct DATA_XYZ GYR_OFFSET;
extern struct DATA_XYZ ACC;
extern struct DATA_XYZ GYR;
extern struct DATA_XYZ GYR_RATE;
extern struct DATA_XYZ GYR_CTR;

struct DATA_XYZ last_ACC={0,0,0};
struct DATA_XYZ_F gyr_offset_f={0.0,0.0,0.0};
/**************************ʵ�ֺ���********************************************
//��iic��ȡ�������ݷֲ�,������Ӧ�Ĵ���
*******************************************************************************/
void MPU6050_CONVENT(void)
{
	ACC.X=((unsigned short)(mpu6050_buffer[0] << 8) | mpu6050_buffer[1])-ACC_OFFSET.X;
	ACC.Y=((unsigned short)(mpu6050_buffer[2] << 8) | mpu6050_buffer[3])-ACC_OFFSET.Y;
	ACC.Z=((unsigned short)(mpu6050_buffer[4] << 8) | mpu6050_buffer[5])-ACC_OFFSET.Z;
	
	GYR_RATE.X=((unsigned short)(mpu6050_buffer[8] << 8) | mpu6050_buffer[9]);//-GYR_OFFSET.X;
	GYR_RATE.Y=((unsigned short)(mpu6050_buffer[10] << 8) | mpu6050_buffer[11]);//-GYR_OFFSET.Y;
	GYR_RATE.Z=((unsigned short)(mpu6050_buffer[12] << 8) | mpu6050_buffer[13]);//-GYR_OFFSET.Z;

	GYR.X= GYR_RATE.X-GYR_OFFSET.X;
	GYR.Y= GYR_RATE.Y-GYR_OFFSET.Y;
	GYR.Z= GYR_RATE.Z-GYR_OFFSET.Z;
	
}
void MPU6050_CONVENT1(void)
{
	ACC.X=((unsigned short)(mpu6050_buffer[0] << 8) | mpu6050_buffer[1])-ACC_OFFSET.X;
	ACC.Y=((unsigned short)(mpu6050_buffer[2] << 8) | mpu6050_buffer[3])-ACC_OFFSET.Y;
	ACC.Z=((unsigned short)(mpu6050_buffer[4] << 8) | mpu6050_buffer[5])-ACC_OFFSET.Z-7168;
	
	GYR_RATE.X=((unsigned short)(mpu6050_buffer[8] << 8) | mpu6050_buffer[9]);//-GYR_OFFSET.X;
	GYR_RATE.Y=((unsigned short)(mpu6050_buffer[10] << 8) | mpu6050_buffer[11]);//-GYR_OFFSET.Y;
	GYR_RATE.Z=((unsigned short)(mpu6050_buffer[12] << 8) | mpu6050_buffer[13]);//-GYR_OFFSET.Z;

	GYR.X= GYR_RATE.X-GYR_OFFSET.X-15;
	GYR.Y= GYR_RATE.Y-GYR_OFFSET.Y-55;
	GYR.Z= GYR_RATE.Z-GYR_OFFSET.Z-20;
	
}
/**************************ʵ�ֺ���********************************************
//��iic��ȡ�������ݷֲ�,������Ӧ�Ĵ���,����MPU6050_Last
*******************************************************************************/
void MPU6050_READ(void)
{
	hw_i2cRead(0x68,MPU6050_RA_ACCEL_XOUT_H,14,mpu6050_buffer);
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
*��������:	    �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �е�1��λ
����	dev  Ŀ���豸��ַ
reg	   �Ĵ�����ַ
bitNum  Ҫ�޸�Ŀ���ֽڵ�bitNumλ
data  Ϊ0 ʱ��Ŀ��λ������0 ���򽫱���λ
����   �ɹ� Ϊ1 
ʧ��Ϊ0
*******************************************************************************/ 
void IICwriteBit(unsigned char dev, unsigned char reg, unsigned char bitNum, unsigned char data){
	unsigned char b;
	hw_i2cRead(dev, reg, 1, &b);Delayms(1);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	hw_Single_Write(dev, reg, b);
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
*��������:	    �� �޸� д ָ���豸 ָ���Ĵ���һ���ֽ� �еĶ��λ
����	dev  Ŀ���豸��ַ
reg	   �Ĵ�����ַ
bitStart  Ŀ���ֽڵ���ʼλ
length   λ����
data    ��Ÿı�Ŀ���ֽ�λ��ֵ
����   �ɹ� Ϊ1 
ʧ��Ϊ0
*******************************************************************************/ 
void IICwriteBits(unsigned char dev,unsigned char reg,unsigned char bitStart,unsigned char length,unsigned char data)
{	
	unsigned char b;
	unsigned char mask;
	hw_i2cRead(dev, reg, 1, &b);Delayms(1);
	mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
	data <<= (8 - length);
	data >>= (7 - bitStart);
	b &= mask;
	b |= data;
	hw_Single_Write(dev, reg, b);
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_setClockSource(uint8_t source)
*��������:	    ����  MPU6050 ��ʱ��Դ
* CLK_SEL | Clock Source
* --------+--------------------------------------
* 0       | Internal oscillator
* 1       | PLL with X Gyro reference
* 2       | PLL with Y Gyro reference
* 3       | PLL with Z Gyro reference
* 4       | PLL with external 32.768kHz reference
* 5       | PLL with external 19.2MHz reference
* 6       | Reserved
* 7       | Stops the clock and keeps the timing generator in reset
*******************************************************************************/
void MPU6050_setClockSource(unsigned char source){
	IICwriteBits(0x68, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
	
}
/** Set full-scale gyroscope range.
* @param range New full-scale gyroscope range value
* @see getFullScaleRange()
* @see MPU6050_GYRO_FS_250
* @see MPU6050_RA_GYRO_CONFIG
* @see MPU6050_GCONFIG_FS_SEL_BIT
* @see MPU6050_GCONFIG_FS_SEL_LENGTH
*/
void MPU6050_setFullScaleGyroRange(unsigned char range) {
	IICwriteBits(0x68, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_setFullScaleAccelRange(uint8_t range)
*��������:	    ����  MPU6050 ���ٶȼƵ��������
*******************************************************************************/
void MPU6050_setFullScaleAccelRange(unsigned char range) {
	IICwriteBits(0x68, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_setSleepEnabled(uint8_t enabled)
*��������:	    ����  MPU6050 �Ƿ����˯��ģʽ
enabled =1   ˯��
enabled =0   ����
*******************************************************************************/
void MPU6050_setSleepEnabled(unsigned char enabled) {
	IICwriteBit(0x68, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_setI2CMasterModeEnabled(uint8_t enabled)
*��������:	    ���� MPU6050 �Ƿ�ΪAUX I2C�ߵ�����
*******************************************************************************/
void MPU6050_setI2CMasterModeEnabled(unsigned char enabled) {
	IICwriteBit(0x68, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_setI2CBypassEnabled(uint8_t enabled)
*��������:	    ���� MPU6050 �Ƿ�ΪAUX I2C�ߵ�����
*******************************************************************************/
void MPU6050_setI2CBypassEnabled(unsigned char enabled) {
	IICwriteBit(0x68, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}

void MPU6050_setDLPF(unsigned char mode)
{
	IICwriteBits(0x68, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, mode);
}
void MPU6050_setRATE(unsigned char rate)
{
	IICwriteBits(0x68, MPU6050_RA_SMPLRT_DIV, 0, 8, rate);
}
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_initialize(void)
*��������:	    ��ʼ�� 	MPU6050 �Խ������״̬��
*******************************************************************************/
void MPU6050_INIT(void)
{
	//i2cInit();
	//GPIO_Configuration();
	Delayms(50);
	MPU6050_setClockSource(MPU6050_CLOCK_PLL_XGYRO); //����ʱ��  0x6b   0x01
	Delayms(10);//delay_ms(2);
	//MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_2000);//������������� +-2000��ÿ��
	//Delayms(5);//delay_ms(2);
	//MPU6050_setFullScaleAccelRange(0x3);	//���ٶȶ�������� +-4G
	//Delayms(5);//delay_ms(2);
	
 //	MPU6050_setRATE(15);								   /////////
	//Delayms(5);//delay_ms(2);
	MPU6050_setDLPF(MPU6050_DLPF_BW_42);
	Delayms(10);//delay_ms(2);
	MPU6050_setSleepEnabled(0); //���빤��״̬
	Delayms(10);//delay_ms(2);
	MPU6050_setI2CMasterModeEnabled(0);	 //����MPU6050 ����AUXI2C
	Delayms(10);//delay_ms(2);
	MPU6050_setI2CBypassEnabled(1);	 //����������I2C��	MPU6050��AUXI2C	ֱͨ������������ֱ�ӷ���HMC5883L
	Delayms(10);//delay_ms(2);
	//hw_Single_Write(0x68,MPU6050_RA_ACCEL_CONFIG, 0x08);Delayms(5);
	MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_2000);//������������� +-2000��ÿ��
	Delayms(10);
	MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_4);	//���ٶȶ�������� +-4G
	Delayms(10);
	hw_Single_Write(0x1E,0x00,0x78);//��ÿ�β��������ѡ�����ƽ����Ϊ8  ���������75Hz  û��ƫ�õ�ѹ
	Delayms(10);
	hw_Single_Write(0x1E,0x01,0x20);//�������ų���Χ��1.3Ga     1090(ȱʡ)
	Delayms(10);
	hw_Single_Write(0x1E,0x02,0x00);//��������ģʽ
	Delayms(10);
}
void Get_OFFSET(unsigned int average_times)
{
	unsigned short ct=0;
	long ACCSUM_X=0;
	long ACCSUM_Y=0;
	long ACCSUM_Z=0;
	long GYRSUM_X=0;
	long GYRSUM_Y=0;
	long GYRSUM_Z=0;
	for(ct=0;ct<average_times;ct++)
	{
//		if(ct%20==0)  LED_GRE_TST;		
		MPU6050_READ();
		MPU6050_CONVENT();	
	 	ACCSUM_X+=ACC.X;
		ACCSUM_Y+=ACC.Y;
		ACCSUM_Z+=ACC.Z-8192;
		GYRSUM_X+=GYR.X;
		GYRSUM_Y+=GYR.Y;
		GYRSUM_Z+=GYR.Z;
		//delay_ms(3)	;
		I2C_delay();
	}
	ACC_OFFSET.X=ACCSUM_X/average_times; 
	ACC_OFFSET.Y=ACCSUM_Y/average_times;
	ACC_OFFSET.Z=ACCSUM_Z/average_times;
	GYR_OFFSET.X=GYRSUM_X/average_times;
	GYR_OFFSET.Y=GYRSUM_Y/average_times;
	GYR_OFFSET.Z=GYRSUM_Z/average_times;
	gyr_offset_f.X=GYR_OFFSET.X*Gyr_Gain;
	gyr_offset_f.Y=GYR_OFFSET.Y*Gyr_Gain;
	gyr_offset_f.Z=GYR_OFFSET.Z*Gyr_Gain;
	GYR_CTR.X=GYR_OFFSET.X;
	GYR_CTR.Y=GYR_OFFSET.Y;
}
extern int YAW_X,YAW_Y,YAW_Z;
extern unsigned char BUF_send1[6];
extern int YAW_INIT_X,YAW_INIT_Y,YAW_INIT_Z;
extern float YAW_INIT;
extern float Yaw;
extern float YAW_Angle;
extern struct DATA_XYZ_F GYR_F;
extern struct DATA_XYZ ACC_AVG;

void Get_YAW_INIT(unsigned int average_times)
{
	unsigned short ct=0;
	long YAWSUM_X=0;
	long YAWSUM_Y=0;
	long YAWSUM_Z=0;
	delay14ms();
	for(ct=0;ct<average_times;ct++)
	{
//		if(ct%20==0)  LED_GRE_TST;
		hw_i2cRead(0x1E,0x03,6,BUF_send1);//��ȡHMC5883L  xyz  ���������
		YAW_X=BUF_send1[0]<<8 | BUF_send1[1];
		YAW_Z=BUF_send1[2]<<8 | BUF_send1[3];
		YAW_Y=BUF_send1[4]<<8 | BUF_send1[5];
		YAWSUM_X+=YAW_X;
		YAWSUM_Y+=YAW_Y;
		YAWSUM_Z+=YAW_Z;
		delay14ms();
//		delay14ms();
	}
	YAW_INIT_X=YAWSUM_X/average_times;
	YAW_INIT_Y=YAWSUM_Y/average_times;
	YAW_INIT_Z=YAWSUM_Z/average_times;
/*
	for(ct=0;ct<10;ct++)
	{
		MPU6050_READ();
		MPU6050_CONVENT();
		ACC_SMOOTH(10);
		I2C_delay();
	}
	MahonyAHRSupdate(GYR_F.X,GYR_F.Y,GYR_F.Z,(float)ACC_AVG.X,(float)ACC_AVG.Y,(float)ACC_AVG.Z, YAW_INIT_X, YAWSUM_Y, YAW_INIT_Z);
*/
	YAW_INIT= atan2((double)YAW_Y,(double)YAW_X)*(180/3.14159265);//+180
	Yaw=YAW_INIT;
//	YAW_INIT=Yaw;
}

