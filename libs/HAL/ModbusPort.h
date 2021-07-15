/*******************************************************************************
****版本：1.0.0
****平台：STM32G030C
****日期：2021-07-13
****作者：Qitas
****版权：OS-Q
*******************************************************************************/
#ifndef __MODBUSPORT__H
#define __MODBUSPORT__H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "Modbus.h"

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define  MODBUS_SLAVE_ADDRESS       (1)

/**************************************************************************

**************************************************************************/
extern volatile unsigned char    ReceiveBuffer[MODBUS_RECEIVE_BUFFER_SIZE];
extern volatile unsigned char    ReceiveCounter;

extern void             ModBus_UART_Initialise(uint32_t baud,uint8_t mode);
extern void             ModBus_UART_Char(unsigned char c);
extern unsigned char    ModBus_UART_String(unsigned char *s, unsigned int Length);
extern void             ReceiveInterrupt(unsigned char Data);
extern void             ModBus_TimerValues(void);
#ifdef __cplusplus
}
#endif
#endif
/*-------------------------(C) COPYRIGHT 2021 matismart ----------------------*/
