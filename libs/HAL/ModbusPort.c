
/*******************************************************************************
****版本：1.0.0
****平台：STM32G030C
****日期：2021-07-13
****作者：Qitas
****版权：OS-Q
*******************************************************************************/
// #include "config.h"
// #ifdef MB_RTU_EN
#include "ModbusPort.h"
#include "usart.h"

// Modbus RTU Variables
volatile unsigned char   ReceiveBuffer[MODBUS_RECEIVE_BUFFER_SIZE];         // Buffer to collect data from hardware
volatile unsigned char   ReceiveCounter=0;                                  // Collected data number
/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
void ModBus_UART_Initialise(uint32_t baud,uint8_t mode)
{
    // uart1_init(baud,mode);
    // #ifdef RS485_R
    // RS485_R;
    // #endif
}
/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/

// This is used for send one character
void ModBus_UART_Char(unsigned char c)
{
    // RS485_W;
    //  uart1_sendchr(c);
    // UART_SendData(UART1,c);
    // while(!UART_GetFlagStatus(UART1, UART_FLAG_TXEPT));
    HAL_UART_Transmit(&huart1, &c, 1, 1000);
    // RS485_R;
}

/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/

// This is used for send string, better to use DMA for it)
unsigned char ModBus_UART_String(unsigned char *s, unsigned int Length)
{
    //Set Max485 in Transmitter mode
    // RS485_W;
    // uart1_sendstr(s, Length);
    HAL_UART_Transmit(&huart1, s, Length, 1000);
    // RS485_R;
    return TRUE;
}
/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
void ReceiveInterrupt(unsigned char Data)
{
    ReceiveBuffer[ReceiveCounter] = Data;
    ReceiveCounter++;
    if(ReceiveCounter>MODBUS_RECEIVE_BUFFER_SIZE) ReceiveCounter=0;
    ModbusTimerValue=0;
}
/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
void ModBus_TimerValues(void)
{
    ModbusTimerValue++;
}
// #endif
/*-------------------------(C) COPYRIGHT 2021 matismart ----------------------*/
