// ============================================================
// 文件名: config.c
// 功能说明: 系统初始化与通用工具函数模块
// 本文件包含系统外设统一初始化函数 All_init()，
// 以及浮点数绝对值等通用辅助函数。
// ============================================================

#include "config.h"

uint8 uart[32];     		// 串口数据发送缓冲区

float track_out = 0.0f;     // 方向控制输出（由循迹PID计算）
int16 base_speed = 0;       // 基础目标速度

bit Start = 0;              // 启动标志位
bit Run = 0;                // 运行标志位

// 函数名: All_init
// 功能: 系统所有外设统一初始化
// 说明: 依次初始化串口、无线串口、显示屏、ADC、编码器、电机、定时器中断。
void All_init(void)
{
    uart_init(UART_1, 115200, UART1_TX_P31, UART1_RX_P30); // 初始化串口1，波特率115200
    wireless_uart_init();   // 初始化无线串口
    
    ips114_init();          // 初始化IPS114显示屏
//    key_init();           // 按键初始化（已禁用）
    
    front_adc_init();       // 初始化前置四路ADC
    encoder_init();         // 初始化编码器
    motor_init();           // 初始化电机PWM

    pit_ms_init(PIT_TR, 50);    // 初始化50ms定时中断（循迹控制）
    pit_ms_init(PIT_SP, 10);    // 初始化10ms定时中断（速度环）
}

// 函数名: float_abs
// 功能: 计算浮点数的绝对值
// 参数: a - 输入浮点数
// 返回值: a的绝对值
float float_abs(float a)
{
    if(a < 0){      // 判断是否为负数
        a = -a;     // 负数取反
    }
    return a;       // 返回绝对值
}
