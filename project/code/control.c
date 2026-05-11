//#include "pid.h"
//#include "control.h"
//#include "zf_common_headfile.h"
// 
// 

///*******************************************************************************
// * 函 数 名       :EP_test()
// * 函数功能		   : 方向环测试函数
// * 输    入       : 无
// * 输    出    	 : 无
// *******************************************************************************/
// void EP_test(void) 
//{
//			caculation();				// 数据处理（电感归一化）数据处理（偏差值计算）
//			judge();
//	
//			position_new();

//	if(adc_data_filted[0]<300&&adc_data_filted[1]<300&&adc_data_filted[2]<300&&adc_data_filted[3]<300)		
//	{
//		  pwm_base = Error_angle_out = angle_out = 0;
//	      menu_flag =1;
//		  menu_room=0;
//		 
//	}
//			left_duty =  spd_out_L;
//			right_duty =  spd_out_R;
//	
//			if (left_duty > MAX_SPEED)  left_duty = MAX_SPEED;
//            if (left_duty < MIN_SPEED)  left_duty = MIN_SPEED;
//	
//	        if (right_duty > MAX_SPEED) right_duty = MAX_SPEED;
//            if (right_duty < MIN_SPEED) right_duty = MIN_SPEED;
//	    pwm_set_duty(PWM_L,left_duty);
//		pwm_set_duty(PWM_R,right_duty);

//}

// 

//void PID_test(void)//
//{
////		exp_speed_l = exp_speed_r=test_speed;
//		target_speed_L = target_speed_R =test_speed;
//	
//		speed_pid_calc_left(float target_speed_L, float real_speed_L)
//	    speed_pid_calc_left(float target_speed_R, float real_speed_R)
//		
//		left_duty =  spd_out_L;
//     	right_duty =  spd_out_R;

//		if (right_duty > MAX_SPEED) right_duty = MAX_SPEED;
//        if (right_duty < MIN_SPEED) right_duty = MIN_SPEED;

//		//L_sum_send = L_duty/100;
//		
//		 motor_set_duty_L ;
//	     motor_set_duty_R ;
//}


//void Tract(void)//
//	{
//	
//	
//	caculation();				// 数据处理（电感归一化）数据处理（偏差值计算）
//	judge();																																							// 决策（元素判断）
//	position_new();
//	if(reg_flag== 0)
//		exp_speed_l = straight_speed;// 没懂这是啥
//	
//	 speed_pid_calc_left(float target_speed_L, float real_speed_L)
//	 speed_pid_calc_left(float target_speed_R, float real_speed_R)
//		

//	if(adc_data_filted[0]<50&&adc_data_filted[1]<50&&adc_data_filted[2]<50&&adc_data_filted[3]<50 && reg_flag == 1)		
//	{
//		  L_Zduty = R_Zduty = L_duty = R_duty = Error_angle_out = angle_out = 0;
//	      menu_flag = 1;
//	  	  menu_room=0;
//	  	 
//	}
//		L_duty = L_duty + L_Zduty;
//		if (left_duty > MAX_SPEED)  left_duty = MAX_SPEED;
//        if (left_duty < MIN_SPEED)  left_duty = MIN_SPEED;
//	
//	
//		if(reg_flag == 0)                                    // 跑直线进入赛道
//		{
//			L_pwm_out = L_duty;
//			R_pwm_out = L_duty;
//		}
//		else
//		{
//			L_pwm_out = L_duty - Error_angle_out + angle_out;
//			R_pwm_out = L_duty + Error_angle_out - angle_out;
//		}
//		
//		if (left_duty > MAX_SPEED)  left_duty = MAX_SPEED;
//        if (left_duty < MIN_SPEED)  left_duty = MIN_SPEED;
//		if (right_duty > MAX_SPEED) right_duty = MAX_SPEED;
//        if (right_duty < MIN_SPEED) right_duty = MIN_SPEED;
//		
//		motor_set_duty_L ;
//	    motor_set_duty_R ;
//}

