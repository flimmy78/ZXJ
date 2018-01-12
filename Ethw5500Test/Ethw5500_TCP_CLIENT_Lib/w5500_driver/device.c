#include "main.h"
#include "config.h"

#include "util.h"
#include "w5500.h"
#include "device.h"
#include "types.h"

#include <stdio.h> 
#include <string.h>

CONFIG_MSG  ConfigMsg, RecvMsg;

uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};

//public buffer for DHCP, DNS, HTTP
uint8 pub_buf[1460];

void Reset_W5500(void)
{
  GPIO_ResetBits(GPIOA, WIZ_RESET);
  Delay_us(2);  
  GPIO_SetBits(GPIOA, WIZ_RESET);
  Delay_ms(800);
}
//reboot 
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  JumpAddress = *(vu32*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}
void USART1_Init(void)
{
  USART_InitTypeDef USART_InitStructure;

  /* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //Enable rx enable, 
  
  /* Configure the USARTx */ 
  USART_Init(USART1, &USART_InitStructure);
  /* Enable the USARTx */
  USART_Cmd(USART1, ENABLE);
}



void set_network(void)
{
  uint8 ip[4];
  w5500_SetMac(ConfigMsg.mac);/*����Mac��ַ*/
  w5500_SetSubnetMask(ConfigMsg.sub);/*������������*/
  w5500_SetGateWay(ConfigMsg.gw);/*����Ĭ������*/
  w5500_SetIP(ConfigMsg.lip);/*����Ip��ַ*/

  //Init. TX & RX Memory size of w5500
  sysinit(txsize, rxsize); /*��ʼ��8��socket*/
  
  w5500_Set_RetransmissionTime(2000);/*�������ʱ��ֵ*/
  w5500_Set_NumOfRetransmission(3);/*����������·��ʹ���*/
  

  w5500_Get_IP (ip);

  printf("IP : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);

  w5500_GetSubnetMask(ip);

  printf("SN : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);

  getGAR(ip);

  printf("GW : %d.%d.%d.%d\r\n", ip[0],ip[1],ip[2],ip[3]);
 
}

static void set_default(void)
{
  
  uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};
  uint8 lip[4]={192,168,10,111};
  uint8 sub[4]={255,255,255,0};
  uint8 gw[4]={192,168,10,1};
  uint8 dns[4]={8,8,8,8};
  memcpy(ConfigMsg.lip, lip, 4);
  memcpy(ConfigMsg.sub, sub, 4);
  memcpy(ConfigMsg.gw,  gw, 4);
  memcpy(ConfigMsg.mac, mac,6);
  memcpy(ConfigMsg.dns,dns,4);
  /*
  uint8 dhcp;
  uint8 debug;

  uint16 fw_len;
  uint8 state;
  */
  ConfigMsg.dhcp=0;
  ConfigMsg.debug=1;
  ConfigMsg.fw_len=0;
  
  ConfigMsg.state=NORMAL_STATE;
  ConfigMsg.sw_ver[0]=FW_VER_HIGH;
  ConfigMsg.sw_ver[1]=FW_VER_LOW;
  
}
void get_config(void)
{
  uint8_t tmp=0;
	uint16_t i;
  for (i =0; i < CONFIG_MSG_LEN; i++) 
  {
//     tmp=at24c16_read(i);
    *(ConfigMsg.mac+i) = tmp;
  }
  if((ConfigMsg.mac[0]==0xff)&&(ConfigMsg.mac[1]==0xff)&&(ConfigMsg.mac[2]==0xff)&&(ConfigMsg.mac[3]==0xff)&&(ConfigMsg.mac[4]==0xff)&&(ConfigMsg.mac[5]==0xff))
  {
    set_default();
  }
}
