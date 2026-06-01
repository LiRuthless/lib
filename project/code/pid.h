#ifndef _PID_H_
#define _PID_H_

extern float KP_v;      // 速度环比例系数
extern float KI_v;      // 速度环积分系数
extern float KD_v;      // 速度环微分系数
                 
extern float KP_x;      // 方向环比例系数
extern float K2P_x;     // 方向环非线性二次比例系数
extern float KI_x;      // 方向环积分系数
extern float KD_x;      // 方向环微分系数

extern float PID_outL;              // 速度环控制器总输出（历史变量名，兼容PID/ADRC，供调试打印与ESO反馈）
extern float PID_outR;              // 速度环控制器总输出（历史变量名，兼容PID/ADRC，供调试打印与ESO反馈）

#define MAX_DIR_OUT                 (2000)
#define MAX_SPD_OUT                 (6000)

// 电机PWM死区补偿值
// 说明: 当PID输出绝对值小于此值时，电机实际不转或转动不稳定。
//       通过死区补偿将输出直接提升到该阈值，确保电机响应灵敏。
//       注意: 输出为0时不补偿，以保证目标速度为0时能真正停车。
#define MOTOR_DEAD_ZONE             (1250)


int16 PID_track(void);                             // 循迹PID
int16 PID_L(void);          // 左轮增量式速度PID
int16 PID_R(void);          // 右轮增量式速度PID
int16 PID_L_pos(void);      // 左轮位置式速度PID（含快速制动与坡道保持）
int16 PID_R_pos(void);      // 右轮位置式速度PID（含快速制动与坡道保持）

#endif
