#ifndef _CONTROL_H_
#define _CONTROL_H_

extern int16 track_out;     // 方向控制输出

void track_test(void);
void speed_test2(void);     // 速度环调参测试（固定周期切换速度）
void speed_test(void);      // 速度环与循迹综合控制
void gyro_test(void);      // 循迹测试（预留）
void adc_test(void);        // ADC采集与串口输出测试

#endif
