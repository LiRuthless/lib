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

    // 绑定定时器中断回调函数
    tim4_irq_handler = pit_speed;       // TIM4中断 -> 速度环处理
    tim3_irq_handler = pit_track;       // TIM3中断 -> 循迹控制处理

    All_init();                         // 调用各模块统一初始化

    WDT_CONTR = 0x21;                   // 使能看门狗，预分频4（约140ms溢出）

    KP_v  = 3.46;       // 速度环比例系数			//4.35  0.1
    KI_v  = 0.39;       // 速度环积分系数			//3.46  0.39
//	KD_v  = 0.0 ;        // 速度环微分系数
	
    KP_x  = 0.24;       // 方向环比例系数
    K2P_x = 0.00;       // 方向环非线性系数
    KD_x  = 0.04;       // 方向环微分系数

    base_speed = 600;   // 设置基础速度
    fan_duty = 6000;       // 初始化负压电机占空比为60%占空比



    // 主循环
    while(1)
    {       
    
//        adc1 = adc_mean_filter_convert(KEY_CHANNEL1, 3);
//        adc2 = adc_mean_filter_convert(KEY_CHANNEL2, 3);
        
//        menu();  // 菜单处理函数，包含按键扫描和功能选择
		
		
		sprintf(uart,"%d,%d,%.2f,",target_speed_L,real_speed_L,PID_outL);
	    uart_write_buffer(UART_1,uart,strlen(uart));
		
		sprintf(uart,"%d,%d,%.2f\n",target_speed_R,real_speed_R,PID_outR);
 	    uart_write_buffer(UART_1,uart,strlen(uart));


//		uart_adjust();
		system_delay_ms(100);
    }
}

// 函数名: pit_speed
// 功能: 速度环定时中断服务函数（10ms周期）
// 说明: 由TIM4定时触发，读取编码器、执行电机速度闭环控制。
void pit_speed (void)
{
    read_encoder();     // 读取编码器并计算实际速度      
    motor_control();                // 执行电机PID控制
    
    CLR_WDT = 1;        // 在10ms中断中喂狗，不受主循环menu()耗时影响
   
	
//	sprintf(uart, "%d,%d\r\n", target_speed_L, target_speed_R);
//	uart_write_buffer(UART_1, uart, strlen(uart)); 
}

// 函数名: pit_track
// 功能: 循迹控制定时中断服务函数（50ms周期）
// 说明: 由TIM3定时触发，读取电感值、计算循迹偏差、调整左右轮差速。
void pit_track (void)
{
    speed_test();
//	speed_test2();
//	adc_test();
}                                                       
