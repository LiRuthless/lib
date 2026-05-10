#include "config.h"




float KP_v = 0.0f; 
float KI_v = 0.0f;
float KD_v = 0.0f;

float KP_x = 0.0f;
float K2P_x = 0.0f;
float KI_x = 0.0f;
float KD_x = 0.0f;



//λ��ʽ����PPD

float PID_track(float track_error)
{   
	static float P_out = 0.0,                 // P环节输出
				 P2_out = 0.0,                // P2环节输出（非线性项）
//				 I_out = 0.0,
				 D_out = 0.0;                 // D环节输出
	
	static float error = 0.0,                 // 当前偏差
				 error_last = 0.0,            // 上次偏差
				 error_sum = 0.0;             // 偏差累计（积分项）
	
	static float PID_out = 0.0;               // PID总输出
	
	error = track_error;                      // 更新当前偏差
//	error_sum += error;
	
	P_out = KP_x * error;                     // 计算P环节输出
	P2_out = K2P_x * error * float_abs(error);// 计算非线性P2项（偏差绝对值加权）
//	I_out += KI * error;                      // 计算I环节输出（积分项，已禁用）
	D_out = KD_x * (error - error_last);      // 计算D环节输出（微分项）
	
	error_last = error;                       // 更新上次偏差，供下次微分计算使用
	
	PID_out = (P_out + P2_out/*I_out*/ + D_out);// 汇总PID各环节输出
	
	if(PID_out >  2.7) PID_out =  2.7;        // 输出限幅上限
	if(PID_out < -2.7) PID_out = -2.7;        // 输出限幅下限
	
	return PID_out;                           // 返回PID控制输出
}

//����ʽ�ٶ�PI

int16 PID_L(int16 target_speed_L, int16 real_speed_L)  //左轮速度PID控制器：目标速度、实际速度
{
	static float P_outL = 0.0,                 // P环节输出
				 I_outL = 0.0,                 // I环节输出
				 D_outL = 0.0;                 // D环节输出
	
	static int16 PID_sumL = 0,                 // PID累计输出
				 errorL = 0,                   // 当前偏差
				 Last_errorL=0,                // 上次偏差
				 Previous_errorL=0;            // 上上次偏差
	
	sprintf(uart, "%d,%d\r\n", target_speed_L, real_speed_L);  // 串口打印目标速度和实际速度
	uart_write_buffer(UART_1, uart, strlen(uart));
	
	errorL = target_speed_L - real_speed_L;    // 计算速度偏差 = 目标速度 - 实际速度
	
	P_outL = KP_v * (errorL - Last_errorL);    // P环节：比例控制（基于偏差变化率）
	I_outL = KI_v *  errorL;                   // I环节：积分控制（累积偏差）
	D_outL = KD_v * (errorL - 2.0 * Last_errorL + Previous_errorL);  // D环节：微分控制（二阶差分）
			    
	PID_sumL += (P_outL + I_outL + D_outL);    // PID总输出累加
	
	Previous_errorL = Last_errorL;             // 更新上上次偏差
	Last_errorL = errorL;                      // 更新上次偏差
	
	if(PID_sumL > 5000) PID_sumL = 5000;       // 输出限幅上限
	if(PID_sumL < -5000) PID_sumL = -5000;     // 输出限幅下限
	
	return PID_sumL;                           // 返回左轮电机PWM控制值
}

//����ʽ�ٶ�PI

int16 PID_R(int16 target_speed_R, int16 real_speed_R)  //右轮速度PID控制器：目标速度、实际速度
{
	static float P_outR = 0.0,                 // P环节输出
				 I_outR = 0.0,                 // I环节输出
				 D_outR = 0.0;                 // D环节输出
	
	static int16 PID_sumR = 0,                 // PID累计输出
				 errorR = 0,                   // 当前偏差
				 Last_errorR=0,                // 上次偏差
				 Previous_errorR=0;            // 上上次偏差
	
	errorR = target_speed_R - real_speed_R;    // 计算速度偏差 = 目标速度 - 实际速度
				
	P_outR = KP_v * (errorR - Last_errorR);    // P环节：比例控制（基于偏差变化率）
	I_outR = KI_v *  errorR;                   // I环节：积分控制（累积偏差）
	D_outR = KD_v * (errorR - 2.0 * Last_errorR + Previous_errorR);  // D环节：微分控制（二阶差分）
			    
	PID_sumR += (P_outR + I_outR + D_outR);    // PID总输出累加
	
	Previous_errorR = Last_errorR;             // 更新上上次偏差
	Last_errorR = errorR;                      // 更新上次偏差
	
	if(PID_sumR > 5000) PID_sumR = 5000;       // 输出限幅上限
	if(PID_sumR < -5000) PID_sumR = -5000;     // 输出限幅下限
	
	return PID_sumR;                           // 返回右轮电机PWM控制值
}