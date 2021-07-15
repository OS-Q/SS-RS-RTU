/*******************************************************************************
****版本：1.0.0
****平台：STM32G030C
****日期：2021-07-13
****作者：Qitas
****版权：OS-Q
*******************************************************************************/
// #include "config.h"

// #ifdef MB_RTU_EN

#include "Modbus.h"

QITAS_MB_DATA       Qitas_Tx_Data;
unsigned int        Qitas_Tx_Current              = 0;
unsigned int        Qitas_Tx_CRC16                = 0xFFFF;
QITAS_MB_STATE      Qitas_Tx_State                = QITAS_RXTX_IDLE;
unsigned char       Qitas_Tx_Buf[MODBUS_TRANSMIT_BUFFER_SIZE];
unsigned int        Qitas_Tx_Buf_Size             = 0;

QITAS_MB_DATA       Qitas_Rx_Data;
unsigned int        Qitas_Rx_CRC16                = 0xFFFF;
QITAS_MB_STATE      Qitas_Rx_State                = QITAS_RXTX_IDLE;
unsigned char       Qitas_Rx_Data_Available       = FALSE;

volatile unsigned short ModbusTimerValue         = 0;
unsigned char Modbus_reset_flag                  = 0;


/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/

void Qitas_CRC16(const unsigned char Data, unsigned int* crc16)
{
    unsigned int i;
    *crc16 = *crc16 ^(unsigned int) Data;
    for (i = 8; i > 0; i--)
    {
        if (*crc16 & 0x0001)
            *crc16 = (*crc16 >> 1) ^ 0xA001;
        else
            *crc16 >>= 1;
    }
}

/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
unsigned char Qitas_DoSlaveTX(void)
{
    ModBus_UART_String(Qitas_Tx_Buf,Qitas_Tx_Buf_Size);
    Qitas_Tx_Buf_Size = 0;
    return TRUE;
}

/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
unsigned char QitasSendMessage(void)
{
    if (Qitas_Tx_State != QITAS_RXTX_IDLE) return FALSE;
    Qitas_Tx_Current  = 0;
    Qitas_Tx_State    = QITAS_RXTX_START;
    return TRUE;
}
/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/

void HandleModbusError(char ErrorCode)
{
    // Initialise the output buffer. The first byte in the buffer says how many registers we have read
    Qitas_Tx_Data.Function    = ErrorCode | 0x80;
    Qitas_Tx_Data.Address     = MODBUS_SLAVE_ADDRESS;
    Qitas_Tx_Data.DataLen     = 0;
    QitasSendMessage();
}

/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
#ifdef AUTOINFO
void HandleWarnCallBack(char WarnCode)
{
        Qitas_Tx_Data.Function      = MODBUS_AUTO_WARN_BACKOUT;
        Qitas_Tx_Data.Address       = MODBUS_SLAVE_ADDRESS;
        Qitas_Tx_Data.DataLen       = 3;
        Qitas_Tx_Data.DataBuf[0]    = Qitas_Tx_Data.DataLen - 1;
        Qitas_Tx_Data.DataBuf[1]    = 0;
        Qitas_Tx_Data.DataBuf[2]    = WarnCode;
        QitasSendMessage();
}

void HandleModbusWarn(char WarnCode)
{
    if(warn_enable)
    {
        handler.back_times++;
        HandleWarnCallBack(WarnCode);
        if (Qitas_Tx_State != QITAS_RXTX_IDLE) Qitas_TxRTU();
        handler.warn_s = time_s_cnt;
    }
}
#endif
/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
#if MODBUS_READ_HOLDING_REGISTERS_ENABLED > 0
void HandleModbusReadHoldingRegisters(void)
{
    unsigned int    Qitas_StartAddress        = 0;
    unsigned int    Qitas_NumberOfRegisters   = 0;
    unsigned int    Qitas_i                   = 0;
    // The message contains the requested start address and number of registers
    Qitas_StartAddress        = ((unsigned int) (Qitas_Rx_Data.DataBuf[0]) << 8) + (unsigned int) (Qitas_Rx_Data.DataBuf[1]);
    Qitas_NumberOfRegisters   = ((unsigned int) (Qitas_Rx_Data.DataBuf[2]) << 8) + (unsigned int) (Qitas_Rx_Data.DataBuf[3]);
    if(Qitas_StartAddress==1010 && Qitas_NumberOfRegisters==1) Qitas_StartAddress= 16;
    // If it is bigger than RegisterNumber return error to Modbus Master
    if((Qitas_StartAddress+Qitas_NumberOfRegisters)>NUMBER_OF_OUTPUT_REGISTERS) HandleModbusError(QITAS_ERROR_CODE_02);
    else
    {
        // Initialise the output buffer. The first byte in the buffer says how many registers we have read
        Qitas_Tx_Data.Function    = MODBUS_READ_HOLDING_REGISTERS;
        Qitas_Tx_Data.Address     = MODBUS_SLAVE_ADDRESS;
        Qitas_Tx_Data.DataLen     = 1;
        for (Qitas_i = 0; Qitas_i < Qitas_NumberOfRegisters; Qitas_i++)
        {
            unsigned short Qitas_CurrentData = Registers[Qitas_StartAddress+Qitas_i].Val;
            Qitas_Tx_Data.DataBuf[Qitas_Tx_Data.DataLen]        = (unsigned char) ((Qitas_CurrentData & 0xFF00) >> 8);
            Qitas_Tx_Data.DataBuf[Qitas_Tx_Data.DataLen + 1]    = (unsigned char) (Qitas_CurrentData & 0xFF);
            Qitas_Tx_Data.DataLen+= 2;
            Qitas_Tx_Data.DataBuf[0] = Qitas_Tx_Data.DataLen - 1;
        }
        QitasSendMessage();
    }
}
#endif


/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
#if MODBUS_WRITE_SINGLE_REGISTER_ENABLED > 0
void HandleModbusWriteSingleRegister(void)
{
    // Write single numerical output
    unsigned int    Qitas_Address   = 0;
    unsigned int    Qitas_Value     = 0;
    unsigned char   Qitas_i         = 0;

    // The message contains the requested start address and number of registers
    Qitas_Address   = ((unsigned int) (Qitas_Rx_Data.DataBuf[0]) << 8) + (unsigned int) (Qitas_Rx_Data.DataBuf[1]);
    Qitas_Value     = ((unsigned int) (Qitas_Rx_Data.DataBuf[2]) << 8) + (unsigned int) (Qitas_Rx_Data.DataBuf[3]);

    // Initialise the output buffer. The first byte in the buffer says how many registers we have read
    Qitas_Tx_Data.Function    = MODBUS_WRITE_SINGLE_REGISTER;
    Qitas_Tx_Data.Address     = MODBUS_SLAVE_ADDRESS;
    Qitas_Tx_Data.DataLen     = 4;

    if(Qitas_Address>=NUMBER_OF_OUTPUT_REGISTERS)  HandleModbusError(QITAS_ERROR_CODE_03);   //处理寄存器超出异常
    else
    {
        // ModBus_Regs_store(Qitas_Address,Qitas_Value);
        Registers[Qitas_Address].Val=Qitas_Value;
        // Output data buffer is exact copy of input buffer
        for (Qitas_i = 0; Qitas_i < 4; ++Qitas_i)
        {
            Qitas_Tx_Data.DataBuf[Qitas_i] = Qitas_Rx_Data.DataBuf[Qitas_i];
        }
    }
    QitasSendMessage();
    modbus_regs_loop();
}
#endif
/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
#if MODBUS_WRITE_SINGLE_COIL_ENABLED > 0
void Coil_to_Register(unsigned int reg,unsigned int val)
{
//    if(reg==1 && val==0xff00) Registers[CMD_CTRL_REG].Val=handle_upon;
//    else if(reg==2 && val==0xff00) Registers[CMD_CTRL_REG].Val=handle_down;
//    else if(reg==0 && val==0xff00) Registers[CMD_CTRL_REG].Val=handle_reset;
}

void HandleModbusWriteSingleCoil(void)
{
    // Write single numerical output
    unsigned int    Qitas_Address   = 0;
    unsigned int    Qitas_Value     = 0;
    unsigned char   Qitas_i         = 0;

    // The message contains the requested start address and number of registers
    Qitas_Address   = ((unsigned int) (Qitas_Rx_Data.DataBuf[0]) << 8) + (unsigned int) (Qitas_Rx_Data.DataBuf[1]);
    Qitas_Value     = ((unsigned int) (Qitas_Rx_Data.DataBuf[2]) << 8) + (unsigned int) (Qitas_Rx_Data.DataBuf[3]);

    // Initialise the output buffer. The first byte in the buffer says how many registers we have read
    Qitas_Tx_Data.Function    = MODBUS_WRITE_SINGLE_COIL;
    Qitas_Tx_Data.Address     = MODBUS_SLAVE_ADDRESS;
    Qitas_Tx_Data.DataLen     = 4;

    if(Qitas_Address>=NUMBER_OF_OUTPUT_COILS)  HandleModbusError(QITAS_ERROR_CODE_03);   //处理寄存器超出异常
    else
    {
        // ModBus_Regs_store(Qitas_Address,Qitas_Value);
        Coil_to_Register(Qitas_Address,Qitas_Value);
        // Output data buffer is exact copy of input buffer
        for (Qitas_i = 0; Qitas_i < 4; ++Qitas_i)
        {
            Qitas_Tx_Data.DataBuf[Qitas_i] = Qitas_Rx_Data.DataBuf[Qitas_i];
        }
    }
    QitasSendMessage();
    // modbus_regs_loop();
}
#endif
/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
#if MODBUS_WRITE_MULTIPLE_REGISTERS_ENABLED > 0
void HandleModbusWriteMultipleRegisters(void)
{
    // Write single numerical output
    unsigned int    Qitas_StartAddress            =0;
    // unsigned char   Qitas_ByteCount               =0;
    unsigned int    Qitas_NumberOfRegisters       =0;
    unsigned char   Qitas_i                       =0;
    unsigned int    Qitas_Value                   =0;

    // The message contains the requested start address and number of registers
    Qitas_StartAddress        = ((unsigned int) (Qitas_Rx_Data.DataBuf[0]) << 8) + (unsigned int) (Qitas_Rx_Data.DataBuf[1]);
    Qitas_NumberOfRegisters   = ((unsigned int) (Qitas_Rx_Data.DataBuf[2]) << 8) + (unsigned int) (Qitas_Rx_Data.DataBuf[3]);
    // Qitas_ByteCount           = Qitas_Rx_Data.DataBuf[4];

    // If it is bigger than RegisterNumber return error to Modbus Master
    if((Qitas_StartAddress+Qitas_NumberOfRegisters)>NUMBER_OF_OUTPUT_REGISTERS) HandleModbusError(QITAS_ERROR_CODE_03);
    else
    {
        // Initialise the output buffer. The first byte in the buffer says how many outputs we have set
        Qitas_Tx_Data.Function    = MODBUS_WRITE_MULTIPLE_REGISTERS;
        Qitas_Tx_Data.Address     = MODBUS_SLAVE_ADDRESS;
        Qitas_Tx_Data.DataLen     = 4;
        Qitas_Tx_Data.DataBuf[0]  = Qitas_Rx_Data.DataBuf[0];
        Qitas_Tx_Data.DataBuf[1]  = Qitas_Rx_Data.DataBuf[1];
        Qitas_Tx_Data.DataBuf[2]  = Qitas_Rx_Data.DataBuf[2];
        Qitas_Tx_Data.DataBuf[3]  = Qitas_Rx_Data.DataBuf[3];

        // Output data buffer is exact copy of input buffer
        for (Qitas_i = 0; Qitas_i <Qitas_NumberOfRegisters; Qitas_i++)
        {
            Qitas_Value=(Qitas_Rx_Data.DataBuf[5+2*Qitas_i]<<8)+(Qitas_Rx_Data.DataBuf[6+2*Qitas_i]);
            Registers[Qitas_StartAddress+Qitas_i].Val=Qitas_Value;
        }
        QitasSendMessage();
        modbus_regs_loop();
    }
}
#endif


/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
unsigned char Qitas_RxDataAvailable(void)
{
    unsigned char Result    = Qitas_Rx_Data_Available;
    Qitas_Rx_Data_Available = FALSE;
    return Result;
}


/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
unsigned char Qitas_CheckRxTimeout(void)
{
    // A return value of true indicates there is a timeout
    if (ModbusTimerValue>= MODBUS_TIMEOUTTIMER)
    {
        ModbusTimerValue =0;
        ReceiveCounter   =0;
        return TRUE;
    }
    return FALSE;
}


/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
unsigned char CheckModbusBufferComplete(void)
{
    int ExpectedReceiveCount=0;
    if(ReceiveCounter>4)
    {
        if(ReceiveBuffer[0]==MODBUS_SLAVE_ADDRESS || ReceiveBuffer[0]==0)
        {
            if(ReceiveBuffer[1]==0x01 || ReceiveBuffer[1]==0x02 || ReceiveBuffer[1]==0x03 || ReceiveBuffer[1]==0x04 || ReceiveBuffer[1]==0x05 || ReceiveBuffer[1]==0x06)
            {
                ExpectedReceiveCount = 8;
            }
            else if(ReceiveBuffer[1]==0x0F || ReceiveBuffer[1]==0x10)
            {
                ExpectedReceiveCount=ReceiveBuffer[6]+9;
            }
            #ifdef AUTOINFO
            else if(ReceiveBuffer[1]==MODBUS_AUTO_WARN_BACKOUT)
            {
                auto_info_done();
            }
            #endif
            else
            {
                ReceiveCounter=0;
                return QITAS_FALSE_FUNCTION;
            }
        }
        else
        {
            ReceiveCounter=0;
            return QITAS_FALSE_SLAVE_ADDRESS;
        }
    }
    else return QITAS_DATA_NOT_READY;
    if(ReceiveCounter==ExpectedReceiveCount)
    {
        return QITAS_DATA_READY;
    }
    return QITAS_DATA_NOT_READY;
}


/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
void Qitas_RxRTU(void)
{
    unsigned char   Qitas_i;
    unsigned char   Qitas_ReceiveBufferControl=0;

    Qitas_ReceiveBufferControl = CheckModbusBufferComplete();

    if(Qitas_ReceiveBufferControl==QITAS_DATA_READY)
    {
        Qitas_Rx_Data.Address               =ReceiveBuffer[0];
        Qitas_Rx_CRC16                      = 0xffff;
        Qitas_CRC16(Qitas_Rx_Data.Address, &Qitas_Rx_CRC16);
        Qitas_Rx_Data.Function              =ReceiveBuffer[1];
        Qitas_CRC16(Qitas_Rx_Data.Function, &Qitas_Rx_CRC16);

        Qitas_Rx_Data.DataLen=0;

        for(Qitas_i=2;Qitas_i<ReceiveCounter;Qitas_i++)
        {
            Qitas_Rx_Data.DataBuf[Qitas_Rx_Data.DataLen++]=ReceiveBuffer[Qitas_i];
        }
        Qitas_Rx_State =QITAS_RXTX_DATABUF;
        ReceiveCounter=0;
    }

    Qitas_CheckRxTimeout();

    if ((Qitas_Rx_State == QITAS_RXTX_DATABUF) && (Qitas_Rx_Data.DataLen >= 2))
    {
        // Finish off our crc16 check
        Qitas_Rx_Data.DataLen -= 2;
        for (Qitas_i = 0; Qitas_i < Qitas_Rx_Data.DataLen; ++Qitas_i)
        {
            Qitas_CRC16(Qitas_Rx_Data.DataBuf[Qitas_i], &Qitas_Rx_CRC16);
        }
        if (((unsigned int) Qitas_Rx_Data.DataBuf[Qitas_Rx_Data.DataLen] + ((unsigned int) Qitas_Rx_Data.DataBuf[Qitas_Rx_Data.DataLen + 1] << 8)) == Qitas_Rx_CRC16)
        {
            Qitas_Rx_Data_Available = TRUE;
        }
        Qitas_Rx_State = QITAS_RXTX_IDLE;
    }
}


/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
void Qitas_TxRTU(void)
{
    Qitas_Tx_CRC16                =0xFFFF;
    Qitas_Tx_Buf_Size             =0;
    Qitas_Tx_Buf[Qitas_Tx_Buf_Size++]   =Qitas_Tx_Data.Address;
    Qitas_CRC16(Qitas_Tx_Data.Address, &Qitas_Tx_CRC16);
    Qitas_Tx_Buf[Qitas_Tx_Buf_Size++]   =Qitas_Tx_Data.Function;
    Qitas_CRC16(Qitas_Tx_Data.Function, &Qitas_Tx_CRC16);

    for(Qitas_Tx_Current=0; Qitas_Tx_Current < Qitas_Tx_Data.DataLen; Qitas_Tx_Current++)
    {
        Qitas_Tx_Buf[Qitas_Tx_Buf_Size++]=Qitas_Tx_Data.DataBuf[Qitas_Tx_Current];
        Qitas_CRC16(Qitas_Tx_Data.DataBuf[Qitas_Tx_Current], &Qitas_Tx_CRC16);
    }

    Qitas_Tx_Buf[Qitas_Tx_Buf_Size++] = Qitas_Tx_CRC16 & 0x00FF;
    Qitas_Tx_Buf[Qitas_Tx_Buf_Size++] =(Qitas_Tx_CRC16 & 0xFF00) >> 8;

    Qitas_DoSlaveTX();
    Qitas_Tx_State    =QITAS_RXTX_IDLE;
}




/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
void modbus_init(void)
{
//    switch (Qitas.CFG.baud)
//    {
//        case 1:
//            ModBus_UART_Initialise(2400,Qitas.CFG.mode);
//        break;
//        case 2:
//            ModBus_UART_Initialise(4800,Qitas.CFG.mode);
//        break;
//        case 3:
//            ModBus_UART_Initialise(9600,Qitas.CFG.mode);
//        break;
//        case 4:
//            ModBus_UART_Initialise(19200,Qitas.CFG.mode);
//        break;
//        case 5:
//            ModBus_UART_Initialise(115200,Qitas.CFG.mode);
//        break;
//        default:
//            ModBus_UART_Initialise(9600,Qitas.CFG.mode);
//        break;
//    }
    modbus_regs_init();
}


/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
void modbus_task(void)
{
    if (Qitas_Tx_State != QITAS_RXTX_IDLE) Qitas_TxRTU();
    Qitas_RxRTU();
    if (Qitas_RxDataAvailable())
    {
        if((Qitas_Rx_Data.Address == MODBUS_SLAVE_ADDRESS) || (Qitas_Rx_Data.Address == 0))
        {
            switch (Qitas_Rx_Data.Function)
            {
                #if MODBUS_READ_HOLDING_REGISTERS_ENABLED > 0
                case MODBUS_READ_HOLDING_REGISTERS:
                {
                    HandleModbusReadHoldingRegisters();
                    break;
                }
                #endif
                #if MODBUS_WRITE_SINGLE_COIL_ENABLED > 0
                case MODBUS_WRITE_SINGLE_COIL:
                {
                    HandleModbusWriteSingleCoil();
                    break;
                }
                #endif
                #if MODBUS_WRITE_SINGLE_REGISTER_ENABLED > 0
                case MODBUS_WRITE_SINGLE_REGISTER:
                {
                    HandleModbusWriteSingleRegister();
                    break;
                }
                #endif
                #if MODBUS_WRITE_MULTIPLE_REGISTERS_ENABLED > 0
                case MODBUS_WRITE_MULTIPLE_REGISTERS:
                {
                    HandleModbusWriteMultipleRegisters();
                    break;
                }
                #endif
                default:
                {
                    HandleModbusError(QITAS_ERROR_CODE_01);
                    break;
                }
            }
            if (Qitas_Tx_State != QITAS_RXTX_IDLE) Qitas_TxRTU();
        }
    }
}
// #endif

/*-------------------------(C) COPYRIGHT 2021 matismart ----------------------*/
