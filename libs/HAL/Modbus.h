/*******************************************************************************
****版本：1.0.0
****平台：STM32G030C
****日期：2021-07-13
****作者：Qitas
****版权：OS-Q
*******************************************************************************/
#ifndef __QITAS_MODBUS_H
#define __QITAS_MODBUS_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define MODBUS_SLAVE_MAX_ADDRESS                    (247)
#define NUMBER_OF_OUTPUT_REGISTERS                  (80)
#define MODBUS_TIMEOUTTIMER                         (800)
#define MODBUS_READ_HOLDING_REGISTERS_ENABLED       (1)
#define MODBUS_WRITE_SINGLE_REGISTER_ENABLED        (1)
#define MODBUS_WRITE_SINGLE_COIL_ENABLED            (1)
#define MODBUS_WRITE_MULTIPLE_REGISTERS_ENABLED     (1)

#define  MODBUS_REGISTERS     (MODBUS_READ_HOLDING_REGISTERS_ENABLED || MODBUS_WRITE_SINGLE_REGISTER_ENABLED || MODBUS_WRITE_MULTIPLE_REGISTERS_ENABLED)

// Buffers for Qitas Modbus RTU Slave
#define MODBUS_RECEIVE_BUFFER_SIZE                 (NUMBER_OF_OUTPUT_REGISTERS*2 + 5)
#define MODBUS_TRANSMIT_BUFFER_SIZE                MODBUS_RECEIVE_BUFFER_SIZE
#define MODBUS_RXTX_BUFFER_SIZE                    MODBUS_TRANSMIT_BUFFER_SIZE

#if MODBUS_WRITE_SINGLE_COIL_ENABLED > 0
#define NUMBER_OF_OUTPUT_COILS                      (3)
#endif
/*******************************ModBus Functions*******************************/
#define MODBUS_READ_COILS                           (1)
#define MODBUS_READ_DISCRETE_INPUTS                 (2)
#define MODBUS_READ_HOLDING_REGISTERS               (3)
#define MODBUS_READ_INPUT_REGISTERS                 (4)
#define MODBUS_WRITE_SINGLE_COIL                    (5)
#define MODBUS_WRITE_SINGLE_REGISTER                (6)
#define MODBUS_AUTO_WARN_BACKOUT                    (7)
#define MODBUS_WRITE_MULTIPLE_COILS                 (15)
#define MODBUS_WRITE_MULTIPLE_REGISTERS             (16)

/****************************End of ModBus Functions***************************/
#define QITAS_FALSE_FUNCTION                        0
#define QITAS_FALSE_SLAVE_ADDRESS                   1
#define QITAS_DATA_NOT_READY                        2
#define QITAS_DATA_READY                            3

typedef enum
{
    QITAS_ERROR_CODE_01=1,      // Function code is not supported
    QITAS_ERROR_CODE_02,        // Register address is not allowed or write-protected
    QITAS_ERROR_CODE_03         // Some data values are out of range, invalid number of register
}QITAS_ERROR_CODE;

typedef enum
{
    QITAS_RXTX_IDLE,
    QITAS_RXTX_START,
    QITAS_RXTX_DATABUF,
    QITAS_RXTX_WAIT_ANSWER,
    QITAS_RXTX_TIMEOUT
}QITAS_MB_STATE;

typedef struct
{
    unsigned char     Address;
    unsigned char     Function;
    unsigned short    DataLen;
    unsigned char     DataBuf[MODBUS_RXTX_BUFFER_SIZE];
}QITAS_MB_DATA;

typedef struct
{
    unsigned char     L;
    unsigned char     H;
}bytes;

typedef union
{
    unsigned short   Val;
    struct
    {
        unsigned char   L;
        unsigned char   H;
    }Char;
}SlaveReg;


extern SlaveReg Registers[NUMBER_OF_OUTPUT_REGISTERS];


extern volatile unsigned short ModbusTimerValue;

void Qitas_TxRTU(void);
extern void  modbus_init(void);
extern void  modbus_task(void);
void HandleModbusWarn(char WarnCode);
/**************************************************************************

**************************************************************************/
#include "ModbusPort.h"
#include "RegConstant.h"

#ifdef __cplusplus
}
#endif
#endif

/*-------------------------(C) COPYRIGHT 2021 matismart ----------------------*/
