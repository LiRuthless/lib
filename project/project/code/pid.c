// ============================================================
// 文件名: pid.c
// 功能说明: PID控制器实现模块
// 包含位置式循迹PID（带非线性P2项）和增量式速度PID（左右轮独立），
// 用于智能车电磁循迹系统的方向控制和速度闭环控制。
// ============================================================

#include "config.h"

float KP_v = 0.0f;              // 速度环比例系数
float KI_v = 0.0f;              // 速度环积分系数
float KD_v = 0.0f;              // 速度环微分系数

float KP_x = 0.0f;              // 方向环比例系数
float K2P_x = 0.0f;             // 方向环非线性二次比例系数
float KI_x = 0.0f;              // 方向环积分系数（预留）
float KD_x = 0.0f;              // 方向环微分系数


// 函数名: PID_track
// 功能: 位置式循迹PID控制器
// 参数: track_error - 循迹偏差输入（由adc.c计算得到）
// 返回值: 舵机/方向控制输出，范围[-2.7, 2.7]
// 说明: 采用改进型位置式PID，增加了非线性P2项（K2P_x * error * |error|），
//       在小偏差时响应柔和，大偏差时快速修正。
float PID_track(float track_error)
{   
    static float P_out = 0.0,       // P环节输出
                 P2_out = 0.0,      // P2环节输出（非线性项）
//                 I_out = 0.0,
                 D_out = 0.0;       // D环节输出
    
    static float error = 0.0,       // 当前偏差
                 error_last = 0.0,   // 上次偏差
                 error_sum = 0.0;    // 偏差累计（积分项）
    
    static float PID_out = 0.0;     // PID总输出
    
    error = track_error;            // 更新当前偏差
//    error_sum += error;
    
    P_out = KP_x * error;                               // 计算P环节输出
    P2_out = K2P_x * error * float_abs(error);          // 计算非线性P2项
//    I_out += KI * error;                                // 计算I环节输出（已禁用）
    D_out = KD_x * (error - error_last);                // 计算D环节输出（微分项）
    
    error_last = error;             // 更新上次偏差，供下次微分计算使用
    
    PID_out = (P_out + P2_out/*I_out*/ + D_out);        // 汇总PID各环节输出
    
    if(PID_out >  2.7) PID_out =  2.7;      // 输出限幅上限
    if(PID_out < -2.7) PID_out = -2.7;      // 输出限幅下限
    
    return PID_out;                 // 返回PID控制输出
}

// 函数名: PID_L
// 功能: 左轮增量式速度PID控制器
// 参数: target_speed_L - 左轮目标速度
//       real_speed_L   - 左轮实际速度（编码器反馈）
// 返回值: 左轮电机PWM控制增量输出，范围[-5000, 5000]
// 说明: 采用增量式PID算法，输出为PWM增量，抗积分饱和。
//       微分项使用二阶差分：error - 2*Last_error + Previous_error
int16 PID_L(int16 target_speed_L, int16 real_speed_L)
{
    static float P_outL = 0.0,      // P环节输出
                 I_outL = 0.0,      // I环节输出
                 D_outL = 0.0;      // D环节输出
    
    static int16 PID_sumL = 0,      // PID累计输出
                 errorL = 0,        // 当前偏差
                 Last_errorL = 0,   // 上次偏差
                 Previous_errorL = 0;   // 上上次偏差
    
    // 串口打印目标速度和实际速度
//    sprintf(uart, "%d,%d\r\n", target_speed_L, real_speed_L);
//    uart_write_buffer(UART_1, uart, strlen(uart));
    
    errorL = target_speed_L - real_speed_L;     // 计算速度偏差 = 目标速度 - 实际速度
    
    P_outL = KP_v * (errorL - Last_errorL);     // P环节：比例控制（基于偏差变化率）
    I_outL = KI_v *  errorL;                    // I环节：积分控制（累积偏差）
    D_outL = KD_v * (errorL - 2.0 * Last_errorL + Previous_errorL); // D环节：微分控制
    
    PID_sumL += (P_outL + I_outL + D_outL);     // PID总输出累加
    
    Previous_errorL = Last_errorL;              // 更新上上次偏差
    Last_errorL = errorL;                       // 更新上次偏差
    
    if(PID_sumL > 5000) PID_sumL = 5000;        // 输出限幅上限
    if(PID_sumL < -5000) PID_sumL = -5000;      // 输出限幅下限
    
    return PID_sumL;                            // 返回左轮电机PWM控制值
}

// 函数名: PID_R
// 功能: 右轮增量式速度PID控制器
// 参数: target_speed_R - 右轮目标速度
//       real_speed_R   - 右轮实际速度（编码器反馈）
// 返回值: 右轮电机PWM控制增量输出，范围[-5000, 5000]
// 说明: 与PID_L对称，实现右轮独立速度闭环控制。
int16 PID_R(int16 target_speed_R, int16 real_speed_R)
{
    static float P_outR = 0.0,      // P环节输出
                 I_outR = 0.0,      // I环节输出
                 D_outR = 0.0;      // D环节输出
    
    static int16 PID_sumR = 0,      // PID累计输出
                 errorR = 0,        // 当前偏差
                 Last_errorR = 0,   // 上次偏差
                 Previous_errorR = 0;   // 上上次偏差
    
    errorR = target_speed_R - real_speed_R;     // 计算速度偏差 = 目标速度 - 实际速度
                
    P_outR = KP_v * (errorR - Last_errorR);     // P环节：比例控制
    I_outR = KI_v *  errorR;                    // I环节：积分控制
    D_outR = KD_v * (errorR - 2.0 * Last_errorR + Previous_errorR); // D环节：微分控制
                
    PID_sumR += (P_outR + I_outR + D_outR);     // PID总输出累加
    
    Previous_errorR = Last_errorR;              // 更新上上次偏差
    Last_errorR = errorR;                       // 更新上次偏差
    
    if(PID_sumR > 5000) PID_sumR = 5000;        // 输出限幅上限
    if(PID_sumR < -5000) PID_sumR = -5000;      // 输出限幅下限
    
    return PID_sumR;                            // 返回右轮电机PWM控制值
}
