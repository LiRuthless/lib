#include "config.h"


int16 fu_pwm = 0;

float track_out = 0.0f;
int16 speed_outL = 0;
int16 speed_outR = 0;
int16 base_speed = 0;


bit Start = 0;
bit Run = 0;



void main()
{
	
    clock_init(SYSTEM_CLOCK_40M);				// 系统时钟
	debug_init();								// 调试串口
    	
	tim4_irq_handler = pit_speed;				//
	tim3_irq_handler = pit_track;				//
	
	All_init();                                 // 初始化

	
		KP_v = 0.8;
		KI_v = 0.05;
		KD_v = 0.0;
		KP_x = 0.3;
	    K2P_x = 0.01;
	    KD_x = 0.0;
	
		base_speed  = 200;
	
	
    while(1)
	{		

//		adc1 = adc_mean_filter_convert(KEY_CHANNEL1, 3);
//		adc2 = adc_mean_filter_convert(KEY_CHANNEL2, 3);
		
//		menu();

    }
}

void pit_speed (void)
{
	
	read_encoder();
//	sprintf(uart, "%d,%d\r\n", target_speed_L, target_speed_R);
//	uart_write_buffer(UART_1, uart, strlen(uart));
		
	if( Run == 1 ) 
	{
		motor_control();
		pwm_set_duty(MOTOR_PWM_M, 5000);
	}
	
}

void pit_track (void)
{
	
	read_adc();
	
	sprintf(uart, "%d,%d,%d,%d\r\n", adc_filted[0],adc_filted[1],adc_filted[2],adc_filted[3]);
	uart_write_buffer(UART_1, uart, strlen(uart));
	
		//正常循迹
	
		
		if( adc_filted[0]+adc_filted[1]+adc_filted[2]+adc_filted[3] > 800 )
		{
			track_out = PID_track(track_error());
			
			if( track_out > 0 ){
				target_speed_L = base_speed + 1*track_out;
				target_speed_R = base_speed - 2*track_out;
			}
			else{
				target_speed_L = base_speed - 2*track_out;
				target_speed_R = base_speed + 1*track_out;
			}
			Run = 1;
			Start = 1;
		}
		else //出界
		{	
			Start = 0;
			Run = 0;
		}
	
	
	if( Start == 0 ) 		//停车 期望速度为0
	{
		pwm_set_duty(MOTOR_PWM_L, 0);
		pwm_set_duty(MOTOR_PWM_R, 0);	
		pwm_set_duty(MOTOR_PWM_M, 1000);
	}
}                                                       

