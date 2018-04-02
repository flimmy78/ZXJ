#include "HardwareProfile.h"
#include "w5500_Conf.h"
#include ".\w5500\Ethernet\wizchip_conf.h"

wiz_NetInfo gWIZNETINFO = { 
	.mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
	.ip = {192, 168, 1, 205},
	.sn = {255,255,255,0},
	.gw = {192, 168, 1, 1},
	.dns = {0,0,0,0},
	.dhcp = NETINFO_STATIC 
};
uint8_t pc_ip[4]={192,168,1,103};/*����Ŀ���������IP��ַ*/
uint16_t DESPORT=6000;
uint8_t NetBuf[1024];
uint16_t myPort = 50000;
#define UseSocket 0

unsigned char NetTxList[100];
unsigned char NetTxCount;
void loopback_tcpc(void)
{
	int32_t ret;
	uint16_t size,i,j,sentsize;
	// Socket Status Transitions
	// Check the W5500 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
	switch(getSn_SR(UseSocket))
	{
	case SOCK_ESTABLISHED :
		if(getSn_IR(UseSocket) & Sn_IR_CON)	// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
		{
			setSn_IR(UseSocket, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
		}
		//////////////////////////////////////////////////////////////////////////////////////////////
		// Data Transaction Parts; Handle the [data receive and send] process
		//////////////////////////////////////////////////////////////////////////////////////////////
		if((size = getSn_RX_RSR(UseSocket)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
		{
			if(size > DATA_BUF_SIZE) 
				size = DATA_BUF_SIZE; // DATA_BUF_SIZE means user defined buffer size (array)
			ret = recv(UseSocket, NetBuf, size); // Data Receive process (H/W Rx socket buffer -> User's buffer)

			if(ret <= 0) // If the received data length <= 0, receive failed and process end
			{
				gFlags.bNet=0;
				return;
			}		
			CP1616_NoAddr_EndProcCommand();
			for(j=0;j<size;j++)
			{
				if(CP1616_NoAddr_DataProc(NetBuf[j])==1)
				{
					switch(CP1616_NoAddr_RxList[pCP1616_NoAddr_CommandIndex])
					{
					//16 16 00 01 01 00 00 2E 0d
					case 0x01:
 						NetTxList[0] = 0x16;
 						NetTxList[1] = 0x16;
 						NetTxList[2] = CP1616_NoAddr_RxList[pCP1616_NoAddr_CommandIndex];
						NetTxCount=2+12+10+6+4+4+4;
 						NetTxList[3] = HIGH_BYTE(NetTxCount);
 						NetTxList[4] = LOW_BYTE(NetTxCount);
						NetTxList[pCP1616_NoAddr_Data]=Temperature.u8H;
						NetTxList[pCP1616_NoAddr_Data+1]=Temperature.u8L;
 						for (i = 0; i < 12; i++)
 						{
 							NetTxList[pCP1616_NoAddr_Data+2+i] = TCMData[i];
 						}
						for (i = 0; i < 10; i++)
						{
							NetTxList[pCP1616_NoAddr_Data+2+12+i] = GPS_GPRMC.UTCTime[i];
						}
						for (i = 0; i < 6; i++)
						{
							NetTxList[pCP1616_NoAddr_Data+2+12+10+i] = GPS_GPRMC.UTCDate[i];
						}
						for (i = 0; i < 4; i++)
						{
							NetTxList[pCP1616_NoAddr_Data+2+12+10+6+i] = MS8607_Temperature.u8s[i];
						}
						for (i = 0; i < 4; i++)
						{
							NetTxList[pCP1616_NoAddr_Data+2+12+10+6+4+i] = MS8607_Pressure.u8s[i];
						}
						for (i = 0; i < 4; i++)
						{
							NetTxList[pCP1616_NoAddr_Data+2+12+10+6+4+4+i] = MS8607_RH.u8s[i];
						}
 						NetTxList[pCP1616_NoAddr_Data+NetTxCount] = GetVerify_Sum(NetTxList, pCP1616_NoAddr_Data+NetTxCount);
 						NetTxList[pCP1616_NoAddr_Data+NetTxCount + 1] = 0x0d;

						sentsize=0;
						// Data sentsize control
						while(sentsize!=(pCP1616_NoAddr_Data+NetTxCount + 2))
						{
							ret=send(UseSocket, NetTxList, pCP1616_NoAddr_Data+NetTxCount + 2);
							if(ret<0)
							{
								close(UseSocket); // socket close
								gFlags.bNet=0;
								//return ret;
							}
							sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
						}						
						break;
					}
					CP1616_NoAddr_EndProcCommand();
				}
				
			}
		}
		//////////////////////////////////////////////////////////////////////////////////////////////
		break;
	case SOCK_CLOSE_WAIT :
		if((ret=disconnect(UseSocket)) != SOCK_OK) 
		{
			gFlags.bNet=0;
			//return ret;
		}
		break;
	case SOCK_INIT :
		if( (ret = connect(UseSocket, pc_ip, DESPORT)) != SOCK_OK) 
		{
			gFlags.bNet=0;
		}//return ret;	//	Try to TCP connect to the TCP server (destination)
		break;

	case SOCK_CLOSED:
		close(UseSocket);
		if((ret=socket(UseSocket, Sn_MR_TCP, myPort++, 0x00)) != UseSocket) 
		{
			gFlags.bNet=0;
		}//return ret; // TCP socket open with 'any_port' port number
		break;
	default:
		break;
	}
	//return 1;
}



void w5500LibInit( void )
{
	uint8_t tmp;
	uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);	//ע���ٽ�������
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);//ע��SPIƬѡ�źź���
	reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);	//ע���д����
	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
	{
		gFlags.bNet=0;
		return;
	}
	if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
	{
		gFlags.bNet=0;
		return;
	}
 	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	gFlags.bNet=1;
}
/**
  * @brief  �����ٽ���
  * @retval None
  */
void SPI_CrisEnter(void)
{
	//__set_PRIMASK(1);
}
/**
  * @brief  �˳��ٽ���
  * @retval None
  */
void SPI_CrisExit(void)
{
	//__set_PRIMASK(0);
}

/**
  * @brief  Ƭѡ�ź�����͵�ƽ
  * @retval None
  */
void SPI_CS_Select(void)
{
	w5500_CSoff();
}
/**
  * @brief  Ƭѡ�ź�����ߵ�ƽ
  * @retval None
  */
void SPI_CS_Deselect(void)
{
	w5500_CSon();
}

/**
  * @brief  д1�ֽ����ݵ�SPI����
  * @param  TxData д�����ߵ�����
  * @retval None
  */
void SPI_WriteByte(uint8_t TxData)
{				 
	SimSPI_Proc(TxData);		
}
/**
  * @brief  ��SPI���߶�ȡ1�ֽ�����
  * @retval ����������
  */
uint8_t SPI_ReadByte(void)
{	
	return SimSPI_Proc(0xff);
// 	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	//�ȴ���������			  
// 	SPI1->DR=0xFF;	 	  										//����һ�������ݲ����������ݵ�ʱ�� 
// 	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0); //�ȴ�������һ��byte  
// 	return SPI1->DR;  						    
}

/**
  * @brief  Intialize the network information to be used in WIZCHIP
  * @retval None
  */
/*
void network_init(void)
{
   uint8_t tmpstr[6];
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);

	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
// 	printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
// 	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
// 		  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
// 	printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
// 	printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
// 	printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
// 	printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
// 	printf("======================\r\n");
}
*/
