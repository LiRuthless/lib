#ifndef _FILTER_H_
#define _FILTER_H_

// 一阶低通滤波器结构体
typedef struct {
    float last_output;  // 上一次滤波输出值
    float alpha;        // 滤波系数 0~1
} LowPassFilter;

void lowpass_init(LowPassFilter *filt, float alpha);    // 初始化低通滤波器
float lowpass_update(LowPassFilter *filt, float input); // 低通滤波器单步更新

#endif
