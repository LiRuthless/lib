#ifndef _ADRC_H_
#define _ADRC_H_

// ============================================================
// 文件名: ardc.h
// 功能说明: 一阶线性ADRC速度环控制器头文件
// 采用保守方案A：100Hz采样，b0=8，ω_c=31.4，ω_o=94.2
// ============================================================

// 一阶线性ADRC控制器结构体
typedef struct {
    float z1;       // 状态估计（跟踪速度）
    float z2;       // 扰动估计（总扰动）
    float b0;       // 控制增益估计
    float beta1;    // ESO 增益1 = 2*ω_o
    float beta2;    // ESO 增益2 = ω_o^2
    float kp;       // 控制器增益 = ω_c
} ADRC_Controller;

extern ADRC_Controller adrc_L;  // 左轮 ADRC
extern ADRC_Controller adrc_R;  // 右轮 ADRC

int16 ADRC_L(void);  // 左轮 ADRC 速度环
int16 ADRC_R(void);  // 右轮 ADRC 速度环

#endif
