#include "config.h"

int16 target_speed_L = 0;
int16 target_speed_R = 0;
int16 real_speed_L = 0;
int16 real_speed_R = 0;

float alpha = 0.4;

LowPassFilter filt_encoder_L;
LowPassFilter filt_encoder_R;


void motor_control(void)  // 电机控制函数
{
	static int16 speed_outL = 0,                        // 左轮速度输出
					 speed_outR = 0;                    // 右轮速度输出
		   
	speed_outL = PID_L(target_speed_L, real_speed_L);   // 左轮速度PID计算
	speed_outR = PID_R(target_speed_R, real_speed_R);   // 右轮速度PID计算
	
	sprintf(uart, "%d,%d,%d\r\n", target_speed_L,real_speed_L,speed_outL);  // 串口打印调试数据
	uart_write_buffer(UART_1, uart, strlen(uart));
	
			
	if(speed_outL >= 0)                                 // 正转（速度为正）
       {
           gpio_set_level(MOTOR_DIR_L, GPIO_LOW);       // DIR输出低电平
           pwm_set_duty(MOTOR_PWM_L, speed_outL);       // 设置正占空比
       }
    else                                                // 反转（速度为负）
       {
           gpio_set_level(MOTOR_DIR_L, GPIO_HIGH);      // DIR输出高电平
           pwm_set_duty(MOTOR_PWM_L, -speed_outL);      // 设置正占空比（取绝对值）
       }
	
	if(speed_outR >= 0)                                 // 正转（速度为正）
       {
           gpio_set_level(MOTOR_DIR_R, GPIO_LOW);       // DIR输出低电平
           pwm_set_duty(MOTOR_PWM_R, speed_outR);       // 设置正占空比
       }
    else                                                // 反转（速度为负）
       {
           gpio_set_level(MOTOR_DIR_R, GPIO_HIGH);      // DIR输出高电平
           pwm_set_duty(MOTOR_PWM_R, -speed_outR);      // 设置正占空比（取绝对值）
       }

}

void read_encoder(void)  // 读取编码器并计算实际速度
{
	static int16 encoder_L = 0,                         // 左轮编码器累计值
					 encoder_R = 0;                     // 右轮编码器累计值
	
	encoder_L = -encoder_get_count(ENCODER_DIR_L);      // 读取左轮编码器计数值（取反）
	encoder_R = encoder_get_count(ENCODER_DIR_R);       // 读取右轮编码器计数值

//	if (gpio_get_level(ENCODER_DIR_DIR_L) == 1)         // 方向检测（已禁用）
//	{
//		encoder_L = -encoder_L;
//	}
//	if (gpio_get_level(ENCODER_DIR_DIR_R) == 1)
//	{
//		encoder_R = -encoder_R;
//	}

	real_speed_L = lowpass_update(&filt_encoder_L, encoder_L);  // 左轮速度低通滤波
	real_speed_R = lowpass_update(&filt_encoder_R, encoder_R);  // 右轮速度低通滤波
	
//	ldistance=ldistance + L_speed;     
//	rdistance=rdistance + R_speed;			
//	distance=(ldistance+rdistance)/2;
//	if(menu_flag==4)
//	{
//		test_ldistance = test_ldistance + L_speed;
//		test_rdistance = test_rdistance + R_speed;
//		test_distance = (test_ldistance + test_rdistance);
//	}

	encoder_clear_count(ENCODER_DIR_L);                 // 清除左轮编码器计数值
    encoder_clear_count(ENCODER_DIR_R);                 // 清除右轮编码器计数值
}

void motor_init(void)  // 电机初始化函数
{
	gpio_init(MOTOR_DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);  // 左轮DIR GPIO初始化：输出模式、默认高电平、推挽输出
    pwm_init(MOTOR_PWM_L, 17000, 0);                        // 左轮PWM初始化：频率17KHz、初始占空比0
    gpio_init(MOTOR_DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);  // 右轮DIR GPIO初始化：输出模式、默认高电平、推挽输出
    pwm_init(MOTOR_PWM_R, 17000, 0);                        // 右轮PWM初始化：频率17KHz、初始占空比0
	pwm_init(MOTOR_PWM_M, 17000, 0);                        // 中舵机PWM初始化：频率17KHz、初始占空比0
}

void encoder_init(void)
{
    encoder_dir_init(ENCODER_DIR_L, ENCODER_DIR_DIR_L, ENCODER_DIR_PULSE_L);   	
	encoder_dir_init(ENCODER_DIR_R, ENCODER_DIR_DIR_R, ENCODER_DIR_PULSE_R);    
	lowpass_init(&filt_encoder_L, alpha);
	lowpass_init(&filt_encoder_R, alpha);
}