// ============================================================
// 文件名: eeprom.c
// 功能说明: EEPROM参数读写模块
// 将PID参数和速度参数存储到片内Flash（IAP）中，
// 实现断电保存与上电加载功能。
// ============================================================

#include "config.h"

int16 air = 0;      // 预留参数（用于扩展存储）

// 函数名: write_pid
// 功能: 将当前PID参数写入EEPROM
// 说明: 存储方向环和速度环的KP、KI、KD参数到Flash地址0x0200起
void write_pid(void)
{
    extern_iap_write_buff(0x0200, (uint8 *)&KP_v, 2);
    extern_iap_write_buff(0x0202, (uint8 *)&KI_v, 2);
    
    extern_iap_write_buff(0x0204, (uint8 *)&KP_x, 2);
    extern_iap_write_buff(0x0206, (uint8 *)&K2P_x, 2);
    extern_iap_write_buff(0x0208, (uint8 *)&KD_x, 2);
}

// 函数名: write_speed
// 功能: 将当前速度参数写入EEPROM
// 说明: 存储基础速度和负压占空比到Flash地址0x0220起
void write_speed(void)
{
    extern_iap_write_buff(0x0220, (uint8 *)&fan_duty, 2);
    extern_iap_write_buff(0x0222, (uint8 *)&base_speed, 2);
    
    extern_iap_write_buff(0x0224, (uint8 *)&air, 2);
    extern_iap_write_buff(0x0226, (uint8 *)&air, 2);
    extern_iap_write_buff(0x0228, (uint8 *)&air, 2);
}

// 函数名: read_pid
// 功能: 从EEPROM读取PID参数
// 说明: 从Flash地址0x0200起加载方向环和速度环PID参数
void read_pid(void)
{
    iap_read_buff(0x0200, (uint8 *)&KP_v, 2);
    iap_read_buff(0x0202, (uint8 *)&KI_v, 2);
    
    iap_read_buff(0x0204, (uint8 *)&KP_x, 2);
    iap_read_buff(0x0206, (uint8 *)&K2P_x, 2);
    iap_read_buff(0x0208, (uint8 *)&KD_x, 2);
}

// 函数名: read_speed
// 功能: 从EEPROM读取速度参数
// 说明: 从Flash地址0x0220起加载基础速度和负压占空比
void read_speed(void)
{
    iap_read_buff(0x0220, (uint8 *)&fan_duty, 2);
    iap_read_buff(0x0222, (uint8 *)&base_speed, 2);
    
    iap_read_buff(0x0224, (uint8 *)&air, 2);
    iap_read_buff(0x0226, (uint8 *)&air, 2);
    iap_read_buff(0x0228, (uint8 *)&air, 2);
}
