#ifndef _EEPROM_H_
#define _EEPROM_H_

extern int16 air;      // 预留参数（用于扩展存储）

void write_menu(void);
void write_pid_v(void);
void write_speed_low(void);
void write_speed_high(void);

void read_menu(void);
void read_pid_v(void);
void read_speed_low(void);
void read_speed_high(void);


#endif
