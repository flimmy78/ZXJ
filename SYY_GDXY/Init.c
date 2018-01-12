#include "HardwareProfile.h"
#include <string.h>
#include <ctype.h>
void InitMCU(void)             //ϵͳ��ʼ������
{
	//AD��ʼ��
	ADCON1=0b00001111;  //���ֿ�
	CMCON=0x07;


	LED1_W=0;
	LED2_W=0;
	LED3_W=0;
	LED4_W=0;
	LED5_W=0;
	LED6_W=0;
	LED1_DIR=0;
	LED2_DIR=0;
	LED3_DIR=0;
	LED4_DIR=0;
	LED5_DIR=0;
	LED6_DIR=0;

	UcControl_W=1;
	UcControl_DIR=0;

	ADGainControl_W=0;//1С���棬0������
	ADGainControl_DIR=0;

	KEY1_DIR=1;
	KEY2_DIR=1;
	KEY3_DIR=1;
	KEY4_DIR=1;

	LCDBusy_DIR=1;
	/************************************************************************/
	/* ����2                                                                */
	/* ���ͣ�����װ��TXREG��ʼ����                                        */
	/* ���գ����յ����RCREG�ж�����                                        */
	/************************************************************************/
	TRISG2=1;
	TRISG1=0;
	TXSTA2=0b00100110;//�첽��ʽ��ʹ�ܷ��͡�9λ�ֳ����Ͳ�����
	RCSTA2=0b10010000; //ʹ�ܴ��ڡ�ʹ�ܽ��ա��첽��ʽ��9λ�ֳ�
#ifdef Proteus
	SPBRG2=103;
#else
	BRG162=1;
	SPBRG2=33;	//Baud Rate = Fosc/(4(SPBRG+1)) = 115200
#endif
	
	RC2IP=0;//�����ȼ�
	RC2IE=1;//ʹ���ж�
	RC2IF=0;//���жϱ�־
	/************************************************************************/
	/* TMR0   ��ʱ						         	*/
	/************************************************************************/
	T0CON=0b10000010;//�رգ���ʱ����1:8,2us
	SetTMR0();
	TMR0IP=0;//�����ȼ�
	TMR0IE=1;//ʹ���ж�
	TMR0IF=0;//���жϱ�־
	/************************************************************************/
	/* ����1																*/
	/************************************************************************/
	TRISC7=1;
	TRISC6=1;
	TXSTA=0b00100110;//�첽��ʽ��ʹ�ܷ���(bit6=1��9λ����)��λ�ֳ����߲�����(bit2����0)
	RCSTA=0b10010000; //ʹ�ܴ��ڡ�ʹ�ܽ��ա��첽��ʽ��(bit6=1��9λ����)��λ�ֳ�
	//BAUDCON1=0x40;//�����Զ��������Զ����
	SPBRG=0x67;	//Baud Rate = Fosc/(16(SPBRG+1)) = 9600
	RCIP=0;//�����ȼ�
	RCIE=1;//ʹ���ж�
	RCIF=0;//���жϱ�־	

	/************************************************************************/
	/* ȫ���ж�																*/
	/************************************************************************/
	//IPEN=1;//ʹ���жϵ����ȼ�
	PEIE=1;
	GIE=1;

// 	TRISG0=0;
// 	M24XX_WP_W=1;
}
void InitVariable(void)
{
	unsigned char i;
	Curve2Max=300;
	Curve1Max=300;
	MeasureIndex=0;
	if(GetRomData()==0)
	{
		for(i=0;i<6;i++)
		{
			RomParams.hWorkCurrent[i]=50;
			RomParams.hCorrection[i]=0.99;
			RomParams.FACorrection[i]=0.99;
			RomParams.FAWorkCurrent[i]=50;
		}
		RomParams.exWorkTepy.AllFlag=0;
		strcpy(RomParams.ServerIP,"192.168.1.105");
		strcpy(RomParams.ServerPort,"8000");
		strcpy(RomParams.WifiSSID,"Pic238Hou");
		strcpy(RomParams.WifiPASS,"1Q2W3E4R");	
		strcpy(RomParams.SitNum,"??");
		SaveRomData();
	}
	StudentClass[0]='\0';
	StudentNum[0]='\0';
	workScreen=0;
	GotoScreen=0;
	for(i=0;i<6;i++)
	{
		ManVAIndex[i]=0xff;
		ManGQIndex[i]=0xff;
		Curve1Flags[i]=0;
	}
	for(i=0;i<4;i++)
	{
		Curve2Flags[i]=0;
	}
	bCalcH=0;
}
void InitSystem( void )
{
	unsigned int i;
	InitMCU();
	InitVariable();
	EndProcCommandFromTFT();	
	ATCommand_Init();	
/************************************************************************/
/* ��ʼ��ADS1120                                                        */
/************************************************************************/
	SimSPI_Init();
	ADS1120_SPI_Proc(0b01000011);//д�Ĵ���ָ��
	ADS1120_SPI_Proc(0b00110000);
	ADS1120_SPI_Proc(0b00000000);
	ADS1120_SPI_Proc(0b11000000);
	ADS1120_SPI_Proc(0b00000000);
/************************************************************************/
/* DA��ʼ��                                                             */
/************************************************************************/
	TLV56XX_Init();//5623
	TLV56XX_WriteData(0x0000);
	TLV56XX_2_Init();
	TLV56XX_2_WriteData(0x9000);
	TLV56XX_2_WriteData(0x0000);

	InitWithTFT();
	InitWifi();
	if(Wifi_ErrState==WifiNoErr)
	{
		__delay_20ms(100);
		SetScreen(sID_Screen1);
	}
	else
	{
		EnterDlgReNum=sID_Screen1;
		ShowEnterDlg(strNo8266);
	}
}
