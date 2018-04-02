#ifndef __WithTFT_H__
#define __WithTFT_H__

#define NOTIFY_TOUCH_PRESS      0X01   //����������֪ͨ
#define NOTIFY_TOUCH_RELEASE  0X03  //�������ɿ�֪ͨ
#define NOTIFY_WRITE_FLASH_OK  0X0C  //дFLASH�ɹ�
#define NOTIFY_WRITE_FLASH_FAILD  0X0D  //дFLASHʧ��
#define NOTIFY_READ_FLASH_OK  0X0B  //��FLASH�ɹ�
#define NOTIFY_READ_FLASH_FAILD  0X0F  //��FLASHʧ��
#define NOTIFY_MENU                        0X14  //�˵��¼�֪ͨ
#define NOTIFY_TIMER                       0X43  //��ʱ����ʱ֪ͨ
#define NOTIFY_CONTROL                0XB1  //�ؼ�����֪ͨ
#define NOTIFY_SCREEN                   0XB2  //�����л�֪ͨ

enum CtrlType
{
	kCtrlUnknown=0x0,
	kCtrlButton=0x10,  //��ť
	kCtrlText,  //�ı�
	kCtrlProgress,  //������
	kCtrlSlider,    //������
	kCtrlMeter,  //�Ǳ�
	kCtrlDropList, //�����б�
	kCtrlAnimation, //����
	kCtrlRTC, //ʱ����ʾ
	kCtrlGraph, //����ͼ�ؼ�
	kCtrlTable, //���ؼ�
	kCtrlMenu,//�˵��ؼ�
	kCtrlSelector,//ѡ��ؼ�
	kCtrlQRCode,//��ά��
};
#define MAXWithTFTRX 100
typedef union
{
	struct
	{		
		unsigned char  cmd_head;  //֡ͷ
		unsigned char  cmd_type;  //��������(UPDATE_CONTROL)	
		unsigned char  ctrl_msg;   //CtrlMsgType-ָʾ��Ϣ������
		uint   screen_id;  //������Ϣ�Ļ���ID
		uint   control_id;  //������Ϣ�Ŀؼ�ID
		unsigned char    control_type; //�ؼ�����
		unsigned char    data[MAXWithTFTRX-8];
	};
	unsigned char All[MAXWithTFTRX];
}_TFTMsg;
_TFTMsg TFTMsg;
typedef union   
{
	struct
	{		
		unsigned bWithTFTRx:1;
		unsigned bLen:1;
	};
	unsigned char AllFlag;
} _TFTFlags;        // general flags
_TFTFlags TFTFlags;

uint WithTFTRxLen;
unsigned int WithTFTRxCount;
unsigned int WithTFTNeedRxCount;

void InitWithTFT(void);
void EndProcCommandFromTFT(void);
void ProcRxFromTFT(unsigned char rx);
void ProcCommandFromTFT_Main(void);


void SetTxt(unsigned char sID,unsigned char cID,const char* data);
void GetTxt(unsigned char sID,unsigned char cID);
void SetChar(unsigned char sID,unsigned char cID,char data);
void SetTextFlash(unsigned char sID,unsigned char cID,unsigned int time);
void SetButton(unsigned char sID,unsigned char cID,unsigned char status);
void SetScreen(unsigned char sID);
void SetProgress(unsigned char sID,unsigned char cID,unsigned char status);
void DeleteCurve(unsigned char sID,unsigned char cID,unsigned char n);
void AddCurveData(unsigned char sID,unsigned char cID,unsigned char channel,unsigned int data);
void SetCurveVerticalScaling(unsigned char sID,unsigned char cID,unsigned int XScaling,unsigned int YScaling);
void ShowEnterDlg(const char *content);
unsigned char WithTFT_GetTxt(unsigned char sID,unsigned char cID,char* pBuf);
void ShowPic(unsigned char picID,unsigned int x,unsigned int y,unsigned int weight,unsigned int high);
void ShowQueryDlg(const char *content);
#endif
