#include "config.h"


// 函数名: speed_test
// 功能: 速度环调参
// 说明: 
//  
void speed_test2(void)
{
static uint16 cnt = 0; 

	
    cnt++;
    if(cnt >= 80)               // 20 * 10ms = 200ms
    {
        cnt = 0;
        if(base_speed != 1000){
            base_speed =1000;
        }
        else{
            base_speed = 600;
        }
    }
	target_speed_L = base_speed;
	target_speed_R = base_speed;
}



void speed_test(void)
{
static uint16 cnt = 0; 
	
	read_adc();     // 读取四路电感ADC值 
	
    cnt++;
    if(cnt >= 80)               // 20 * 10ms = 200ms
    {
        cnt = 0;
        if(base_speed != 1000)
        {
            base_speed =1000;
//			KP_x = 1.6f;
//			KD_x = 0.6f;
        }
        else
        {
            base_speed = 600;
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
		
        Start = 1;      // 标记已启动
    }
    else // 电感值过低，认为出赛道或停止线
    {    
        Start = 0;      // 清除启动标志
    }
    
    // 未启动状态下停止电机
    if( Start == 0 )
    {
		target_speed_L = 0;
		target_speed_R = 0;
		pwm_set_duty(MOTOR_PWM_M, 1000); // 负压电机回默认占空比
    }

}

void track_test(void)
{






}

void adc_test(void)
{
	read_adc();     // 读取四路电感ADC值 
	
	track_error = get_track_error();
    track_out = PID_track();
	
	sprintf(uart, "%d,%d,%d,%d,", adc_filted[0], adc_filted[1], adc_filted[2], adc_filted[3]);
 	uart_write_buffer(UART_1,uart,strlen(uart));
	
	sprintf(uart,"%d,%d\n",track_error,track_out);
 	uart_write_buffer(UART_1,uart,strlen(uart));
	
}