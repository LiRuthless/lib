// ============================================================
// 文件名: eeprom.c
// 功能说明: EEPROM参数读写模块
// 将PID参数和速度参数存储到片内Flash（IAP）中，
// 实现断电保存与上电加载功能。
// ============================================================

#include "config.h"

int16 air = 0;      // 预留参数（用于扩展存储）


void write_menu(void)
{  
    extern_iap_write_buff(0x0110, (uint8 *)&page, 2);
    extern_iap_write_buff(0x0112, (uint8 *)&arrow, 2);
    extern_iap_write_buff(0x0114, (uint8 *)&air, 2);
	extern_iap_write_buff(0x0116, (uint8 *)&air, 2);
	extern_iap_write_buff(0x0118, (uint8 *)&air, 2);
	
	extern_iap_write_buff(0x0120, (uint8 *)&air, 2);
    extern_iap_write_buff(0x0122, (uint8 *)&air, 2);
    extern_iap_write_buff(0x0124, (uint8 *)&air, 2);
	extern_iap_write_buff(0x0126, (uint8 *)&air, 2);
	extern_iap_write_buff(0x0128, (uint8 *)&air, 2);
}


// 函数名: write_pid
// 功能: 将当前PID参数写入EEPROM
// 说明: 存储方向环和速度环的KP、KI、KD参数到Flash地址0x0200起
void write_pid_v(void)
{
    extern_iap_write_buff(0x0200, (uint8 *)&KP_v, 2);
    extern_iap_write_buff(0x0202, (uint8 *)&KI_v, 2);
}


void write_speed_low(void)
{  
    extern_iap_write_buff(0x0210, (uint8 *)&KP_x, 2);
    extern_iap_write_buff(0x0212, (uint8 *)&K2P_x, 2);
    extern_iap_write_buff(0x0214, (uint8 *)&KD_x, 2);
	extern_iap_write_buff(0x0216, (uint8 *)&air, 2);
	extern_iap_write_buff(0x0218, (uint8 *)&base_speed, 2);
	
	extern_iap_write_buff(0x0220, (uint8 *)&fan_duty, 2);
    extern_iap_write_buff(0x0222, (uint8 *)&air, 2);
    extern_iap_write_buff(0x0224, (uint8 *)&air, 2);
	extern_iap_write_buff(0x0226, (uint8 *)&air, 2);
	extern_iap_write_buff(0x0228, (uint8 *)&air, 2);
}

// 函数名: write_speed
// 功能: 将当前速度参数写入EEPROM
// 说明: 存储基础速度和负压占空比到Flash地址0x0220起
void write_speed_high(void)
{
    extern_iap_write_buff(0x0230, (uint8 *)&KP_x, 2);
    extern_iap_write_buff(0x0232, (uint8 *)&K2P_x, 2);
    extern_iap_write_buff(0x0234, (uint8 *)&KD_x, 2);
	extern_iap_write_buff(0x0236, (uint8 *)&air, 2);
	extern_iap_write_buff(0x0238, (uint8 *)&base_speed, 2);
}

void read_all(void)
{
	read_menu();
}




void read_menu(void)
{  
    iap_read_buff(0x0110, (uint8 *)&page, 2);
    iap_read_buff(0x0112, (uint8 *)&arrow, 2);
    iap_read_buff(0x0114, (uint8 *)&air, 2);
	iap_read_buff(0x0116, (uint8 *)&air, 2);
	iap_read_buff(0x0118, (uint8 *)&air, 2);
	
	iap_read_buff(0x0120, (uint8 *)&air, 2);
    iap_read_buff(0x0122, (uint8 *)&air, 2);
    iap_read_buff(0x0124, (uint8 *)&air, 2);
	iap_read_buff(0x0126, (uint8 *)&air, 2);
	iap_read_buff(0x0128, (uint8 *)&air, 2);
}



// 函数名: read_pid
// 功能: 从EEPROM读取PID参数
// 说明: 从Flash地址0x0200起加载方向环和速度环PID参数
void read_pid_v(void)
{
    iap_read_buff(0x0200, (uint8 *)&KP_v, 2);
    iap_read_buff(0x0202, (uint8 *)&KI_v, 2);
}

// 函数名: read_speed
// 功能: 从EEPROM读取速度参数
// 说明: 从Flash地址0x0220起加载基础速度和负压占空比
void read_speed_low(void)
{
    iap_read_buff(0x0210, (uint8 *)&KP_x, 2);
    iap_read_buff(0x0212, (uint8 *)&K2P_x, 2);
    iap_read_buff(0x0214, (uint8 *)&KD_x, 2);
	iap_read_buff(0x0216, (uint8 *)&air, 2);
	iap_read_buff(0x0218, (uint8 *)&base_speed, 2);
}

void read_speed_high(void)
{
    iap_read_buff(0x0230, (uint8 *)&KP_x, 2);
    iap_read_buff(0x0232, (uint8 *)&K2P_x, 2);
    iap_read_buff(0x0234, (uint8 *)&KD_x, 2);
	iap_read_buff(0x0236, (uint8 *)&air, 2);
	iap_read_buff(0x0238, (uint8 *)&base_speed, 2);
}