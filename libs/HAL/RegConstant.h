/*******************************************************************************
****版本：1.0.0
****平台：STM32G030C
****日期：2021-07-13
****作者：Qitas
****版权：OS-Q
*******************************************************************************/
#ifndef __MODBUS_CONST_H
#define __MODBUS_CONST_H
#ifdef __cplusplus
extern "C" {
#endif

#include "Modbus.h"
// #include "data.h"

typedef enum
{
    COM_ADDR_REG,       //通信地址
    COM_BAUD_REG,       //串口通信配置（低字节: 1=2400、2=4800、3=9600、4=19200、5=115200）
    COM_PAIR_REG,       //通信校验位
    COM_STOP_REG,       //通信停止位
    SYS_CTRL_REG,       //系统状态控制
    SYS_UPDT_REG,       //系统升级控制
    SYS_CLKH_REG,       //SYS运行计时
    SYS_CLKL_REG,       //SYS运行计秒
    VER_HARD_REG,       //硬件版本号
    VER_SOFT_REG,       //软件版本号
    AUTO_EN_REG=10,     //自动控制使能
    AUTO_TIME_RES,      //系统倒计时
    LED_STAT_REG,       //显示状态
    AUX_STAT_REG,       //显示状态
    LCK_STAT_REG,       //锁定状态
    POS_STAT_REG,       //霍尔状态
    CMD_STAT_REG,       //重合闸状态输出
    CMD_CTRL_REG,       //重合闸状态控制
    USER_MODE_CTL,      //用户控制模式
    AUTO_LOCK_CNT,      //设备锁定次数、运行自动重合次数
    MOTO_UPON_REG=20,   //合闸补偿
    MOTO_DOWN_REG,      //分闸补偿
    UPON_FREE_REG,      //合闸复位补偿
    DOWN_FREE_REG,      //分闸复位补偿
    RUN_UPON_MS,
    RUN_DOWN_MS,
    UPON_FREE_MS,
    DOWN_FREE_MS,
    RUN_LOCK_MS,
    RUN_REST_MS,
    OUPA_EN_REG=30,     //过欠压使能
    OUPA_TIME_CNT,      //过欠压恢复时间
    ADC_OUPA_UE,        //电压
    ADC_OUPA_HZ,        //电压频率
    OUPA_STAT_REG,      //过欠压状态
    ADC_V_VOLTMAX,      //电压最大值
    ADC_V_VOLTMIN,      //电压最小值
    ADC_A_VOLTAGE,      //A相电压
    ADC_B_VOLTAGE,      //B相电压
    ADC_C_VOLTAGE,      //C相电压
    AUTO_BACK_EN=40,    //主动上报使能
    AUTO_BACK_VAL,      //主动上报状态
    AUTO_BACK_CNT,      //主动上报计数
    CMD_UPON_CNT,       //指令合闸次数
    CMD_DOWN_CNT,       //指令分闸次数
    CMD_LOCK_CNT,       //指令锁定次数
    A_TIMES_CNT,        //电流故障跳闸次数
    V_TIMES_CNT,        //电压故障跳闸次数
    AUTO_LOCK_S,        //已经锁定的时长
    AUTO_DONE_NO,       //正在执行的自动重合标记
    AUTO_DELY_S0=50,    //自动合闸延时 s
    AUTO_DELY_S1,       //自动合闸延时 s
    AUTO_DELY_S2,       //自动合闸延时 s
    AUTO_DELY_S3,       //自动合闸延时 s
    AUTO_DELY_S4,       //自动合闸延时 s
    AUTO_DELY_S5,       //自动合闸延时 s
    AUTO_DELY_S6,       //自动合闸延时 s
    AUTO_DELY_S7,       //自动合闸延时 s
    AUTO_DELY_S8,       //自动合闸延时 s
    AUTO_DELY_S9,       //自动合闸延时 s
    AUTO_HOLD_S0=60,    //合闸稳定时间 s
    AUTO_HOLD_S1,       //合闸稳定时间 s
    AUTO_HOLD_S2,       //合闸稳定时间 s
    AUTO_HOLD_S3,       //合闸稳定时间 s
    AUTO_HOLD_S4,       //合闸稳定时间 s
    AUTO_HOLD_S5,       //合闸稳定时间 s
    AUTO_HOLD_S6,       //合闸稳定时间 s
    AUTO_HOLD_S7,       //合闸稳定时间 s
    AUTO_HOLD_S8,       //合闸稳定时间 s
    AUTO_HOLD_S9,       //合闸稳定时间 s
    CHIP_ID_REG,        //96bits UID
    DATA1,
    DATA2,
    CRC32H,
    CRC32L
}QITAS_RTU_DEF;

/**************************************************************************

**************************************************************************/
uint8_t modbus_regs_init(void);
uint8_t modbus_regs_loop(void);
uint8_t modbus_reg_val(uint8_t reg,uint16_t val);

#ifdef __cplusplus
}
#endif
#endif

/*-------------------------(C) COPYRIGHT 2021 matismart ----------------------*/
