// ============================================================
// 文件名: filter.c
// 功能说明: 一阶低通滤波器实现
// 提供简单的一阶IIR低通滤波，用于平滑编码器速度等信号，
// 抑制高频噪声。滤波公式：y[n] = alpha * x[n] + (1-alpha) * y[n-1]
// ============================================================

#include "config.h"

// 函数名: lowpass_init
// 功能: 初始化低通滤波器
// 参数: filt  - 低通滤波器结构体指针
//       alpha - 滤波系数，范围(0,1)。越大响应越快，越小平滑效果越好
void lowpass_init(LowPassFilter *filt, float alpha) {
    filt->last_output = 0.0f;   // 初始输出为0
    filt->alpha = alpha;        // 设置滤波系数
}

// 函数名: lowpass_update
// 功能: 低通滤波器更新（单步计算）
// 参数: filt  - 低通滤波器结构体指针
//       input - 当前输入值
// 返回值: 滤波后的输出值
float lowpass_update(LowPassFilter *filt, float input) {
    // 一阶IIR滤波计算：新输出 = alpha * 输入 + (1-alpha) * 上次的输出
    filt->last_output = filt->alpha * input + (1.0f - filt->alpha) * filt->last_output;
    return filt->last_output;   // 返回滤波结果
}
