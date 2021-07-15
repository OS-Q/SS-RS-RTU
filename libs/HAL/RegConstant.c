/*******************************************************************************
****版本：1.0.0
****平台：STM32G030C
****日期：2021-07-13
****作者：Qitas
****版权：OS-Q
*******************************************************************************/
#include "RegConstant.h"

#if MODBUS_REGISTERS
SlaveReg  Registers[NUMBER_OF_OUTPUT_REGISTERS];
#endif

/*******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
uint8_t modbus_reg_val(uint8_t reg,uint16_t val)
{
    Registers[reg].Val=val;
    return 0;
}


/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
uint8_t modbus_regs_init(void)
{
    uint8_t i=0,base=CHIP_ID_REG;
    uint16_t uuid[6]={1,2,3,4,5,6};
    // chip_info_get(uuid);
    Registers[base++].Val = uuid[i++];
    Registers[base++].Val = uuid[i++];
    Registers[base++].Val = uuid[i++];
    Registers[base++].Val = uuid[i++];
    Registers[base++].Val = uuid[i++];
    Registers[base++].Val = uuid[i++];
    // data_read(CONFIG_DATA_ADDR,Qitas.ptr,REG_RAM_LEN); //读取数据
    #ifdef  RS485
    modbus_reg_val(COM_ADDR_REG,Qitas.CFG.addr);
    modbus_reg_val(COM_BAUD_REG,Qitas.CFG.baud);
    modbus_reg_val(COM_PAIR_REG,Qitas.CFG.mode);
    modbus_reg_val(COM_STOP_REG,Qitas.CFG.stop);
		#endif
    #ifdef OUPA
    modbus_reg_val(OUPA_EN_REG,Qitas.CFG.oupa_en);
    modbus_reg_val(OUPA_TIME_CNT,Qitas.CFG.resume_s);
    modbus_reg_val(ADC_OUPA_UE,Qitas.CFG.Ue);
    modbus_reg_val(ADC_OUPA_HZ,Qitas.CFG.Hz);
    #endif
    #ifdef AUTO
    modbus_reg_val(AUTO_EN_REG,auto_enable);
    modbus_reg_val(AUTO_LOCK_CNT,reclose_times);
    for(uint8_t i=0;i<RC_TIMES_MAX;i++)
    {
        Registers[AUTO_DELY_S0+i].Val = Qitas.CFG.auto_delay[i] ;
        Registers[AUTO_HOLD_S0+i].Val = Qitas.CFG.auto_watch[i];
    }
    #else
    modbus_reg_val(AUTO_EN_REG,0);
    // for(uint8_t i=0;i<reclose_times;i++)
    // {
    //     Registers[AUTO_DELY_S0+i].Val = 0;
    //     Registers[AUTO_HOLD_S0+i].Val = 0;
    // }
    #endif
    #ifdef AUTOINFO
    modbus_reg_val(AUTO_BACK_EN,Qitas.CFG.warn_en);
    #else
    modbus_reg_val(AUTO_BACK_EN,0);
    #endif
    // modbus_reg_val(MOTO_UPON_REG,moto_upon_more_ms);
    // modbus_reg_val(MOTO_DOWN_REG,moto_down_more_ms);
    // modbus_reg_val(UPON_FREE_REG,upon_free_more_ms);
    // modbus_reg_val(DOWN_FREE_REG,down_free_more_ms);
    return 0;
}


/******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：无
**输出参数 ：无
*******************************************************************************/
uint8_t modbus_regs_loop(void)
{
    uint8_t mark=0;
		#ifdef  RS485
    if(Registers[COM_ADDR_REG].Val!=Qitas.CFG.addr && Registers[COM_ADDR_REG].Val<=MODBUS_SLAVE_MAX_ADDRESS) {
        mark=1;
        Qitas.CFG.addr = Registers[COM_ADDR_REG].Val;
    }
    if(Registers[COM_BAUD_REG].Val!=Qitas.CFG.baud && Registers[COM_BAUD_REG].Val<4){
        mark=1;
        Qitas.CFG.baud = Registers[COM_BAUD_REG].Val;
    }
		if(Registers[COM_PAIR_REG].Val!=Qitas.CFG.mode && Registers[COM_PAIR_REG].Val<4){
        mark=1;
        Qitas.CFG.mode = Registers[COM_PAIR_REG].Val;
    }
		if(Registers[COM_STOP_REG].Val!=Qitas.CFG.stop && Registers[COM_STOP_REG].Val<4){
        mark=1;
        Qitas.CFG.stop = Registers[COM_STOP_REG].Val;
    }
    #endif
    // if(Registers[AUTO_EN_REG].Val!=auto_enable && Registers[AUTO_EN_REG].Val<2){
    //     mark=1;
    //     auto_enable = Registers[AUTO_EN_REG].Val;
    // }
    // if(Registers[AUTO_BACK_EN].Val!=warn_enable && Registers[AUTO_BACK_EN].Val<2){
    //     mark=1;
    //     warn_enable = Registers[AUTO_BACK_EN].Val;
    // }
    // if(Registers[AUTO_LOCK_CNT].Val!=reclose_times && Registers[AUTO_LOCK_CNT].Val<=RC_TIMES_MAX){
    //     mark=1;
    //     reclose_times = Registers[AUTO_LOCK_CNT].Val;
    // }
    // for(uint8_t i=0;i<RC_TIMES_MAX;i++)
    // {
    //     if(Registers[AUTO_DELY_S0+i].Val<RC_TIME_MAX && Registers[AUTO_DELY_S0+i].Val!=reclose_delay[i])
    //     {
    //         mark=1;
    //         reclose_delay[i] = Registers[AUTO_DELY_S0+i].Val;
    //     }
    //     if(Registers[AUTO_HOLD_S0+i].Val!=reclose_watch[i]  && Registers[AUTO_HOLD_S0+i].Val<RC_TIME_MAX)
    //     {
    //         mark=1;
    //         reclose_watch[i] = Registers[AUTO_HOLD_S0+i].Val;
    //     }
    // }
    // if(Registers[MOTO_UPON_REG].Val!=moto_upon_more_ms && Registers[MOTO_UPON_REG].Val<200){
    //     mark=1;
    //     moto_upon_more_ms = Registers[MOTO_UPON_REG].Val;
    // }
    // if(Registers[MOTO_DOWN_REG].Val!=moto_down_more_ms && Registers[MOTO_DOWN_REG].Val<200){
    //     mark=1;
    //     moto_down_more_ms = Registers[MOTO_DOWN_REG].Val;
    // }
    // if(Registers[UPON_FREE_REG].Val!=upon_free_more_ms && Registers[UPON_FREE_REG].Val<200){
    //     mark=1;
    //     upon_free_more_ms = Registers[UPON_FREE_REG].Val;
    // }
    // if(Registers[DOWN_FREE_REG].Val!=down_free_more_ms && Registers[DOWN_FREE_REG].Val<200){
    //     mark=1;
    //     down_free_more_ms = Registers[DOWN_FREE_REG].Val;
    // }
    #ifdef USER_MODE
    if(Registers[USER_MODE_CTL].Val!=user_ctrl_mode && Registers[USER_MODE_CTL].Val && Registers[USER_MODE_CTL].Val<=USER_MODE)
    {
        mark=1;
        user_ctrl_mode = Registers[USER_MODE_CTL].Val;
    }
    #endif
    #ifdef OUPA
    if(Registers[OUPA_EN_REG].Val!=oupa_enable && Registers[OUPA_EN_REG].Val<2){
        mark=1;
        oupa_enable = Registers[OUPA_EN_REG].Val;
    }
    if(Registers[OUPA_TIME_CNT].Val!=Qitas.CFG.resume_s && Registers[OUPA_TIME_CNT].Val<=3600){
        mark=1;
        Qitas.CFG.resume_s = Registers[OUPA_TIME_CNT].Val;
    }
    if(Registers[ADC_OUPA_UE].Val!=Qitas.CFG.Ue && Registers[ADC_OUPA_UE].Val>=200 && Registers[ADC_OUPA_UE].Val<=250){
        mark=1;
        Qitas.CFG.Ue = Registers[ADC_OUPA_UE].Val;
    }
    if(Registers[ADC_OUPA_HZ].Val!=Qitas.CFG.resume_s && Registers[ADC_OUPA_HZ].Val>=40 && Registers[ADC_OUPA_HZ].Val<=60){
        mark=1;
        Qitas.CFG.Hz = Registers[ADC_OUPA_HZ].Val;
    }
    if(HG.flashmark!=Qitas.CFG.OUPA_flag && HG.flashmark<=adc_endstatus){
        mark=1;
        Qitas.CFG.OUPA_flag = HG.flashmark;
    }
    #endif
//    if(mark) return cfg_data_update();
    return 0;
}
/*-------------------------(C) COPYRIGHT 2021 matismart ----------------------*/
