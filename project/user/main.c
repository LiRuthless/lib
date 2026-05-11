// ============================================================
// 文件名: main.c
// 功能说明: 主程序入口
// 智能车电磁循迹系统主循环，包含系统初始化、PID参数设定、
// 定时中断配置以及主循环空转。
// ============================================================

#include "config.h"



// 函数名: main
// 功能: 主函数入口
void main()
{
    // 系统时钟初始化
    clock_init(SYSTEM_CLOCK_40M);       // 设置系统时钟为40MHz
    
    debug_init();                       // 调试串口初始化

    // 绑定定时器中断回调函数
    tim4_irq_handler = pit_speed;       // TIM4中断 -> 速度环处理
    tim3_irq_handler = pit_track;       // TIM3中断 -> 循迹控制处理

    All_init();                         // 调用各模块统一初始化

    // 初始化方向环PID参数
    KP_v  = 0.8;        // 速度环比例系数
    KI_v  = 0.05;       // 速度环积分系数
    KD_v  = 0.0;        // 速度环微分系数
    KP_x  = 0.3;        // 方向环比例系数
    K2P_x = 0.01;       // 方向环非线性系数
    KD_x  = 0.0;        // 方向环微分系数

    base_speed = 200;   // 设置基础速度

    // 将启动时的PID参数保存为平稳模式默认值，防止首次加载模式时参数被清零
    save_current_param(MODE_STEADY);

    // 主循环
    while(1)
    {       
    
//        adc1 = adc_mean_filter_convert(KEY_CHANNEL1, 3);
//        adc2 = adc_mean_filter_convert(KEY_CHANNEL2, 3);
        
        menu();  // 菜单处理函数，包含按键扫描和功能选择
	
		// 串口打印四路滤波后的ADC值
    	sprintf(uart, "%d,%d,%d,%d\r\n", adc_filted[0], adc_filted[1], adc_filted[2], adc_filted[3]);
   		uart_write_buffer(UART_1, uart, strlen(uart));

    }
}

// 函数名: pit_speed
// 功能: 速度环定时中断服务函数（10ms周期）
// 说明: 由TIM4定时触发，读取编码器、执行电机速度闭环控制。
void pit_speed (void)
{
    read_encoder();     // 读取编码器并计算实际速度
    
//    sprintf(uart, "%d,%d\r\n", target_speed_L, target_speed_R);
//    uart_write_buffer(UART_1, uart, strlen(uart));
        
    // 只有在运行状态下才输出电机控制
    if( Run == 1 ) 
    {
        motor_control();                // 执行电机PID控制
        pwm_set_duty(MOTOR_PWM_M, 5000); // 设置舵机PWM（固定值）
    }
    
}

// 函数名: pit_track
// 功能: 循迹控制定时中断服务函数（50ms周期）
// 说明: 由TIM3定时触发，读取电感值、计算循迹偏差、调整左右轮差速。
void pit_track (void)
{
    read_adc();     // 读取四路电感ADC值
    
    
    // 检测电感值总和是否大于阈值（判断是否在赛道上）
    if( adc_filted[0] + adc_filted[1] + adc_filted[2] + adc_filted[3] > 800 )
    {
        // 在赛道上，计算循迹偏差并差速控制
        track_out = PID_track(track_error());
        
        // 根据偏差方向分配左右轮速度
        if( track_out > 0 ){
            target_speed_L = base_speed + 1 * track_out;    // 左轮加速
            target_speed_R = base_speed - 2 * track_out;    // 右轮减速
        }
        else{
            target_speed_L = base_speed - 2 * track_out;    // 左轮减速
            target_speed_R = base_speed + 1 * track_out;    // 右轮加速
        }
        Run = 1;        // 允许电机输出
        Start = 1;      // 标记已启动
    }
    else // 电感值过低，认为出赛道或停止线
    {    
        Start = 0;      // 清除启动标志
        Run = 0;        // 停止电机输出
    }
    
    // 未启动状态下停止电机
    if( Start == 0 )
    {
        pwm_set_duty(MOTOR_PWM_L, 0);    // 左轮停止
        pwm_set_duty(MOTOR_PWM_R, 0);    // 右轮停止
        pwm_set_duty(MOTOR_PWM_M, 1000); // 舵机回中
    }
}                                                       
