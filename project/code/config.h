#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "zf_common_headfile.h"
#include "pid.h"
#include "motor.h"
#include "menu.h"
#include "adc.h"
#include "filter.h"
#include "control.h"
#include "eeprom.h"
#include "gyroscope.h"

// 定时器PIT通道定义（原分散在 adc.h / motor.h，现统一集中于此）
#define PIT_TR      (TIM3_PIT)      // 循迹控制中断（50ms）
#define PIT_SP      (TIM4_PIT)      // 速度环中断（10ms）

extern uint8 uart[32];      // 串口数据发送缓冲区
extern uint8 dat[32];
extern bit Start;           // 启动标志位
extern bit Run;             // 运行标志位
extern int16 track_out;     // 方向控制输出

void All_init(void);        // 系统初始化
void uart_adjust(void);
void pit_speed (void);      // 速度环中断服务函数
void pit_track (void);      // 循迹控制中断服务函数

float float_abs(float a);   // 浮点数绝对值

#endif
