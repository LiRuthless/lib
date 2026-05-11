#ifndef _PID_H_
#define _PID_H_

extern float KP_v;      // 速度环比例系数
extern float KI_v;      // 速度环积分系数
extern float KD_v;      // 速度环微分系数
                 
extern float KP_x;      // 方向环比例系数
extern float K2P_x;     // 方向环非线性二次比例系数
extern float KI_x;      // 方向环积分系数
extern float KD_x;      // 方向环微分系数

float PID_track(float track_error);                             // 循迹PID
int16 PID_L(int16 target_speed_L, int16 real_speed_L);          // 左轮速度PID
int16 PID_R(int16 target_speed_R, int16 real_speed_R);          // 右轮速度PID

#endif
