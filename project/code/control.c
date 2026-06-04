// ============================================================
// 文件名: control.c
// 功能说明: 控制策略与测试函数模块
// 包含速度环测试、循迹测试、ADC测试等控制策略实现。
// ============================================================

#include "config.h"


int16 track_out = 0;     // 方向控制输出（由循迹PID计算）

void track_test(void)
{
	read_adc();     // 读取四路电感ADC值 
	
	fan_duty = 6000;       // 初始化负压电机占空比为60%占空比
    base_speed = 200;
//	KP_x = 0.15;
//	KD_x = 0.30;
	
//	KP_x = 0.181;
//	KD_x = 0.1;
	KP_x = 0.02;
	KD_x = 0.03;


	
    // 检测电感值总和是否大于阈值（判断是否在赛道上）
    if( adc_filted[0] + adc_filted[1] + adc_filted[2] + adc_filted[3] > 300 )
    {
        // 在赛道上，计算循迹偏差并差速控制
		track_error = get_track_error();
        track_out = PID_track();
        
        // 根据偏差方向分配左右轮速度
        if( track_out >= 0 ){
            target_speed_L = base_speed - 3 * track_out;    // 左轮减速
            target_speed_R = base_speed + 1 * track_out;    // 右轮加速
        }
        else{
            target_speed_L = base_speed - 1 * track_out;    // 左轮加速
            target_speed_R = base_speed + 3 * track_out;    // 右轮减速
        }
		
		pwm_set_duty(MOTOR_PWM_M, fan_duty); // 设置负压电机占空比
		
        Run = 1;      // 标记已启动
    }
    else // 电感值过低，认为出赛道或停止线
    {    
        Run = 0;      // 清除启动标志
		Start = 0;
    }
	
//	sprintf(uart, "%d,%d,%d,%d,", adc_filted[0], adc_filted[1], adc_filted[2], adc_filted[3]);
// 	uart_write_buffer(UART_1,uart,strlen(uart));
//	
//	sprintf(uart,"%d,%d\n",track_error,track_out);
// 	uart_write_buffer(UART_1,uart,strlen(uart));
    
    // 未启动状态下停止电机
//    if( Run == 0 )
//    {
//		target_speed_L = 0;
//		target_speed_R = 0;
//		pwm_set_duty(MOTOR_PWM_M, 1000); // 负压电机回默认占空比
//    }

}



// 函数名: speed_test2
// 功能: 速度环调参测试（固定周期切换速度）
// 说明: 每800ms在1000和600之间切换基础速度，用于测试速度环响应
void speed_test2(void)
{
static uint16 cnt = 0; 

    cnt++;
//    if(cnt >= 80)               // 80 * 10ms = 800ms
//    {
//        cnt = 0;
//        if(base_speed != 1000){
//            base_speed =1000;
//        }
//        else{
//            base_speed = 600;
//        }
//    }
//	
	    if(cnt >= 200)               // 80 * 10ms = 800ms
    {
        cnt = 0;
        if(base_speed != 600){
            base_speed =600;
        }
        else{
            base_speed = 300;
        }
    }
	target_speed_L = base_speed;
	target_speed_R = base_speed;
}


// 函数名: speed_test
// 功能: 速度环与循迹综合控制
// 说明: 周期性切换基础速度，同时进行循迹检测与差速控制。
//       当电感值总和超过阈值时，认为在赛道上，启动循迹PID并差速；
//       否则停止电机。
void speed_test(void)
{
static uint16 cnt = 0; 
	
	read_adc();     // 读取四路电感ADC值 
	
    cnt++;
    if(cnt >= 80)               // 80 * 50ms
    {
        cnt = 0;
        if(base_speed != 600)
        {
            base_speed =600;
//			KP_x = 1.6f;
//			KD_x = 0.6f;
        }
        else
        {
            base_speed = 300;
//			KP_x = 0.5f;
//			KD_x = 0.6f;
        }
    }
	
    // 检测电感值总和是否大于阈值（判断是否在赛道上）
    if( adc_filted[0] + adc_filted[1] + adc_filted[2] + adc_filted[3] > 300 )
    {
        // 在赛道上，计算循迹偏差并差速控制
		track_error = get_track_error();
        track_out = PID_track();
        
        // 根据偏差方向分配左右轮速度
        if( track_out >= 0 ){
            target_speed_L = base_speed - 3 * track_out;    // 左轮减速
            target_speed_R = base_speed + 1 * track_out;    // 右轮加速
        }
        else{
            target_speed_L = base_speed - 1 * track_out;    // 左轮加速
            target_speed_R = base_speed + 3 * track_out;    // 右轮减速
        }
		
		pwm_set_duty(MOTOR_PWM_M, fan_duty); // 设置负压电机占空比
		
        Run = 1;      // 标记已启动
    }
    else // 电感值过低，认为出赛道或停止线
    {    
        Run = 0;      // 清除启动标志
		Start = 0;
    }
    
    // 未启动状态下停止电机
//    if( Run == 0 )
//    {
//		target_speed_L = 0;
//		target_speed_R = 0;
//		pwm_set_duty(MOTOR_PWM_M, 1000); // 负压电机回默认占空比
//    }

}

// 函数名: gyro_test
// 功能: 循迹测试（预留）
// 说明: 预留接口，可用于单独测试循迹功能
void gyro_test(void)
{
	imu660rb_get_gyro();
	
	gyro_x = imu660rb_gyro_transition(imu660rb_gyro_x);
	gyro_y = imu660rb_gyro_transition(imu660rb_gyro_y);
	gyro_z = imu660rb_gyro_transition(imu660rb_gyro_z);
	
	sprintf(uart,"%f,%f,%f,",gyro_x,gyro_y,gyro_z);
	uart_write_buffer(UART_1,uart,strlen(uart));
		
	sprintf(uart,"%d,%d,%d\n",imu660rb_gyro_x,imu660rb_gyro_y,imu660rb_gyro_z);
 	uart_write_buffer(UART_1,uart,strlen(uart));
	
	
}

// 函数名: adc_test
// 功能: ADC采集与串口输出测试
// 说明: 读取四路ADC并计算循迹偏差，通过串口输出电感值和偏差数据，用于调参调试
void adc_test(void)
{
	read_adc();     // 读取四路电感ADC值 
	
	KP_x = 0.02;
	KD_x = 0.36;
	
	track_error = get_track_error();
    track_out = PID_track();
	
	sprintf(uart, "%d,%d,%d,%d,", adc_filted[0], adc_filted[1], adc_filted[2], adc_filted[3]);
 	uart_write_buffer(UART_1,uart,strlen(uart));
	
	sprintf(uart,"%d,%d\n",track_error,track_out);
 	uart_write_buffer(UART_1,uart,strlen(uart));
	
}
