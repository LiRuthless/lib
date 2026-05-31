#ifndef _EEPROM_H_
#define _EEPROM_H_

void write_pid(void);       // 将当前PID参数写入EEPROM
void write_speed(void);     // 将当前速度参数写入EEPROM
void read_pid(void);        // 从EEPROM读取PID参数
void read_speed(void);      // 从EEPROM读取速度参数

#endif
