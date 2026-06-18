// ============================================================
// 文件名: main.c
// 功能说明: 主程序入口
// 智能车电磁循迹系统主循环，包含系统初始化、PID参数设定、
// 定时中断配置以及主循环空转。//，PWM（发出和接受）速度环的问题（电感读值，计算过程，设定值过大，预设上限）
//电流过大，打火，复现
// ============================================================

#include "config.h"

uint32 bb = 0;							// 通用计数变量（预留调试用）

// 函数名: main
// 功能: 主函数入口
void main()
{
    clock_init(SYSTEM_CLOCK_40M);       // 设置系统时钟为40MHz

    tim4_irq_handler = pit_speed;       // TIM4中断 -> 速度环处理
    tim3_irq_handler = pit_track;       // TIM3中断 -> 循迹控制处理

    All_init();                         // 调用各模块统一初始化
//	read_menu();						// 从Flash加载菜单状态

    WDT_CONTR = 0x21;                   // 使能看门狗，预分频4（约140ms溢出）

    KP_v  = 15.0;       				// 速度环比例系数			//15.0  0.85
    KI_v  = 0.85;      					// 速度环积分系数			//3.46  0.39!!!!!!!
	

//  base_speed = 0;   // 设置基础速度
//	fan_duty = 4000;       // 初始化负压电机占空比为60%占空比

//	accel_calibrate();
	gyro_calibrate();

    // 主循环
    while(1)
    {
		key_start();
		
 //     menu();  // 菜单处理函数，包含按键扫描和功能选择

//		
//	
//		sprintf(uart,"%d,%d\n",track_error,track_out);
//		uart_write_buffer(UART_1,uart,strlen(uart));

//		uart_adjust();
//		system_delay_ms(10);
    }
}



// 函数名: pit_track
// 功能: TIM3中断服务函数（5ms周期）
// 说明: 执行循迹控制，检测赛道并计算差速目标速度。
//       未启动或出赛道时强制停车。
void pit_track (void)
{

	if( Start_flag )
	{
       
//		sprintf(uart,"%d,%d,%d,",target_speed_L,real_speed_L,PID_outL);
//		uart_write_buffer(UART_1,uart,strlen(uart));
//		sprintf(uart,"%d,%d,%d\n",target_speed_R,real_speed_R,PID_outR);
//		uart_write_buffer(UART_1,uart,strlen(uart));
//		sprintf(uart, "%d,%d,%d,%d\n", adc_filted[0], adc_filted[1], adc_filted[2], adc_filted[3]);
//		uart_write_buffer(UART_1,uart,strlen(uart));

//	
		roundabout_test();
//		track_test();
//		speed_test();
 //		speed_test2();
//	adc_test();
//		gyro_test();

	}

	if( adc_filted[0] + adc_filted[1] + adc_filted[2] + adc_filted[3] > 300 )
    {
        Run_flag = 1;      // 标记已启动
    }
    else // 电感值过低，认为出赛道或停止线
    {    
        Run_flag = 0;      // 清除启动标志
		Start_flag = 0;
    }
	    // 未启动状态下停止电机
    if( !Run_flag || !Start_flag )
    {
		target_speed_L = 0;
		target_speed_R = 0;
		pwm_set_duty(MOTOR_PWM_M, 1000); // 负压电机回默认占空比
    }
	

}                                                       



// 函数名: pit_speed
// 功能: TIM4中断服务函数（2ms周期）
// 说明: 喂看门狗、读取编码器、执行电机速度PID闭环控制。
void pit_speed (void)
{
	CLR_WDT = 1;        // 在2ms中断中喂狗，不受主循环menu()耗时影响

    read_encoder();     
	read_gyro_angle();
    motor_control();    
   	
//	sprintf(uart,"%d,%d,%f,",target_speed_L,real_speed_L,PID_outL);
//	uart_write_buffer(UART_1,uart,strlen(uart));
//		
//	sprintf(uart,"%d,%d,%f\n",target_speed_R,real_speed_R,PID_outR);
// 	uart_write_buffer(UART_1,uart,strlen(uart));
}
