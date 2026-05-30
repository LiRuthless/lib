#ifndef _ADC_H_
#define _ADC_H_

// ADC通道引脚定义
#define ADC_CHANNEL1            (ADC_CH13_P05)  // 电感通道1
#define ADC_CHANNEL2            (ADC_CH14_P06)  // 电感通道2
#define ADC_CHANNEL3            (ADC_CH8_P00)   // 电感通道3
#define ADC_CHANNEL4            (ADC_CH10_P02)  // 电感通道4

extern uint16 adc_raw[4];       // 四路ADC原始采样值数组
extern int16 adc_filted[4];    // 四路ADC滤波后的输出值数组
extern int16 track_error;

//extern uint16 adc1;
//extern uint16 adc2;
//extern uint16 adc3;
//extern uint16 adc4;

int32 get_track_error(void);        // 计算循迹误差
void front_adc_init(void);      // 初始化四路前置ADC
void read_adc(void);            // 读取并滤波四路ADC

#endif
