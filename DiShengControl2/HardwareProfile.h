#ifndef _HARDWARE_PROFILE_H_
#define _HARDWARE_PROFILE_H_

#define NOP() Nop()
#define FCY 16000000
#define GetSystemClock() FCY   
#define GetInstructionClock() GetSystemClock()

#include "p24FJ128GB106.h"
#include <libpic30.h>
#include "TypeDefine.h"
#include "afx.h"
#include "FlagDefine.h"
//#define Verify_CRC16_Init 0x0000
#define Verify_CRC16
#include "Verify.h"
#include "PPS.h"

/************************************************************************/
/* IO����                                                               */
/************************************************************************/
#define sensorPower_DIR _TRISB3 //�¶�����ˮ��
#define sensorPower_W _LATB3

#define sensorPower_ON 0
#define sensorPower_OFF 1

#define LouShui1_DIR _TRISE3 //©ˮ���1
#define LouShui1_R _RE3

#define LouShui2_DIR _TRISE2 //©ˮ���2
#define LouShui2_R _RE2


#define LED_DIR _TRISE1
#define LED_W _LATE1

#define LED_ON 0
#define LED_OFF 1


// #define RS485_DE_DIR _TRISF3
// #define RS485_DE_W _LATF3 
/************************************************************************/
/* ��������                                                             */
/************************************************************************/
/************************************************************************/
/* ��λ��                                                               */
/************************************************************************/
#define Uart_Uart1
#define Uart1_TRMT U1STAbits.TRMT
#define Uart1_TXREG U1TXREG

#include "TCM.h"
/************************************************************************/
/* �¶ȴ�������Modbus_RTU                                               */
/************************************************************************/
#define Uart_Uart2
#define Uart2_TRMT U2STAbits.TRMT
#define Uart2_TXREG U2TXREG
#define ModbusRTU_Master_TxByte_Ex
#define ModbusRTU_Master_VerifyTxByte(x) Uart2_VerifyCRC16AddTxByte(x)
#define ModbusRTU_Master_TxByte(x) Uart2_PutChar(x)

#define ModbusRTU_RS485DE_W _LATF3
#define ModbusRTU_RS485DE_DIR _TRISF3
#define ModbusRTU_Master_Uart_TRMT U2STAbits.TRMT
#include "ModbusRTU_Master.h"
/************************************************************************/
/* �����ƽӿڰ�                                                         */
/************************************************************************/
// #define Uart_Uart2
// #define Uart2_TRMT U2STAbits.TRMT
// #define Uart2_TXREG U2TXREG


/************************************************************************/
/* ��ʱ                                                                 */
/************************************************************************/
#define Uart_Uart4
#define Uart4_TRMT U3STAbits.TRMT
#define Uart4_TXREG U3TXREG

#include "NMEA0183.h"
#include "GPS_GPRMC.h"
#include "Uart.h"
/************************************************************************/
/* w5500                                                                */
/************************************************************************/
#define SimSPI_SPI3

#define SimSPI_SCL_DIR _TRISD10
#define SimSPI_SDI_DIR _TRISD9
#define SimSPI_SDO_DIR _TRISD8

#define SimSPI_SCL_W _LATD10
#define SimSPI_SDI_R _RD9
#define SimSPI_SDO_W _LATD8

#define SimSPI_Delay_Ex
#define SimSPI_Delay() Nop();Nop();Nop()

#include "SimSPI.h"
#define w5500_CS_W _LATC14
#define w5500_CS_DIR _TRISC14
#include "CP1616_NoAddr.h"
#include "w5500_Conf.h"
#include ".\w5500\Ethernet\wizchip_conf.h"
#include ".\w5500\Ethernet\socket.h"

/************************************************************************/
/* MS8607�¶�ʪ��ѹ��������:ģ��I2C�ӿ�                                 */
/************************************************************************/
#define SimI2C_CLK_IO _TRISF5
#define SimI2C_DATA_IO _TRISF4

#define SimI2C_CLK_W _LATF5
#define SimI2C_DATA_W _LATF4
#define SimI2C_DATA_R _RF4
#define SimI2C_Delay_Ex
#define SimI2C_Delay() __delay_ms(1);
#include "SimI2C.h"
#define MS8607_SimI2C
#define MS8607_I2C_CLK_IO _TRISF5
#define MS8607_I2C_CLK_R _RF5
#define MS8607_I2C_CLK_W _LATF5
#include "MS8607.h"
/************************************************************************/
/* �ⲿ����                                                             */
/************************************************************************/
void InitSystem( void );
void ProcCommandFromPC( void );
extern GFlags gFlags;
extern ErrFlags errFlags;
/************************************************************************/
/* ϵͳ����                                                             */
/************************************************************************/
extern unsigned int SystemTick;

extern uint Temperature;
extern unsigned char TCMData[12];
#endif
