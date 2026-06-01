// ============================================================
// 文件名: ardc.c
// 功能说明: 一阶线性ADRC速度环控制器实现
// 采用保守方案A：100Hz采样，b0=8，ω_c=31.4，ω_o=94.2
// 替换原位置式PID（PID_L_pos / PID_R_pos）为ADRC
// ============================================================

#include "config.h"
#include "ardc.h"

// 左右轮ADRC实例初始化（保守方案A参数）
ADRC_Controller adrc_ctrl_L = {0.0f, 0.0f, 8.0f, 188.4f, 8874.0f, 31.4f};
ADRC_Controller adrc_ctrl_R = {0.0f, 0.0f, 8.0f, 188.4f, 8874.0f, 31.4f};

// 函数名: ADRC_L
// 功能: 左轮一阶线性ADRC速度控制器（含快速制动与坡道保持）
// 参数: target_speed_L - 左轮目标速度（全局变量）
//       real_speed_L   - 左轮实际速度（全局变量，编码器反馈）
// 返回值: 左轮电机PWM控制输出，范围[-MAX_SPD_OUT, MAX_SPD_OUT]
// 说明: 采用ESO实时估计总扰动z2，通过扰动补偿将系统还原为积分器串联型，
//       再用比例控制实现期望动态。
//       快速制动：target=0时自动输出反向制动力矩（z1跟踪实际速度，r=0产生负反馈）。
//       坡道保持：z2估计含坡道重力在内的总扰动，target=0时自动补偿，无需积分累积。
//       死区前馈、输出限幅沿用原有机制；方向切换无需额外保护（ESO自动跟踪扰动变化）。
int16 ADRC_L(void)
{
    float e  = 0.0f;    // 观测误差
    float u0 = 0.0f;    // 线性反馈输出（期望加速度）
    
    // ESO: 观测误差 = 估计速度 - 实际速度
    e = adrc_ctrl_L.z1 - (float)real_speed_L;
    
    // ESO: 状态更新（前向欧拉离散化，T=0.01s）
    // z1_dot = z2 - beta1*e + b0*u_actual
    // z2_dot = -beta2*e
    // 其中 u_actual 必须为上一次【实际施加到电机】的控制量（已含死区前馈与限幅）。
    // PID_outL 是全局变量，存储的正是上一周期最终输出的 PWM 值，此处复用作 ESO 反馈。
    // （变量名含"PID"为历史遗留，实际语义是"速度环控制器输出"，ADRC 与 PID 共用。）
    adrc_ctrl_L.z1 += 0.01f * (adrc_ctrl_L.z2 - adrc_ctrl_L.beta1 * e + adrc_ctrl_L.b0 * PID_outL);
    adrc_ctrl_L.z2 += 0.01f * (-adrc_ctrl_L.beta2 * e);
    
    // 线性控制律: u0 = kp * (r - z1)
    u0 = adrc_ctrl_L.kp * ((float)target_speed_L - adrc_ctrl_L.z1);
    
    // 扰动补偿 + 控制增益归一化: u = (u0 - z2) / b0
    // 结果存入 PID_outL（历史变量名，语义为"速度环控制器总输出"，供调试打印与下次 ESO 反馈）
    PID_outL = (u0 - adrc_ctrl_L.z2) / adrc_ctrl_L.b0;
    
    // 死区前馈（沿用原有机制：target=0时不补偿，确保能真正停车）
    if      (target_speed_L > 0) PID_outL += (float)MOTOR_DEAD_ZONE;
    else if (target_speed_L < 0) PID_outL -= (float)MOTOR_DEAD_ZONE;
    
    // 输出限幅（沿用原有机制）
    if(PID_outL >  MAX_SPD_OUT) PID_outL =  MAX_SPD_OUT;
    if(PID_outL < -MAX_SPD_OUT) PID_outL = -MAX_SPD_OUT;
    
    return (int32)PID_outL;
}

// 函数名: ADRC_R
// 功能: 右轮一阶线性ADRC速度控制器（含快速制动与坡道保持）
// 说明: 与ADRC_L对称，实现右轮独立速度闭环控制。
int16 ADRC_R(void)
{
    float e  = 0.0f;
    float u0 = 0.0f;
    
    e = adrc_ctrl_R.z1 - (float)real_speed_R;
    
    // ESO 反馈同样复用 PID_outR（历史变量名，实际存储上一周期最终 PWM 输出）
    adrc_ctrl_R.z1 += 0.01f * (adrc_ctrl_R.z2 - adrc_ctrl_R.beta1 * e + adrc_ctrl_R.b0 * PID_outR);
    adrc_ctrl_R.z2 += 0.01f * (-adrc_ctrl_R.beta2 * e);
    
    u0 = adrc_ctrl_R.kp * ((float)target_speed_R - adrc_ctrl_R.z1);
    
    // 结果存入 PID_outR（历史变量名，语义同上）
    PID_outR = (u0 - adrc_ctrl_R.z2) / adrc_ctrl_R.b0;
    
    if      (target_speed_R > 0) PID_outR += (float)MOTOR_DEAD_ZONE;
    else if (target_speed_R < 0) PID_outR -= (float)MOTOR_DEAD_ZONE;
    
    if(PID_outR >  MAX_SPD_OUT) PID_outR =  MAX_SPD_OUT;
    if(PID_outR < -MAX_SPD_OUT) PID_outR = -MAX_SPD_OUT;
    
    return (int32)PID_outR;
}
