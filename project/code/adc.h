#ifndef _ADC_H_
#define _ADC_H_

//ADC
#define PIT_TR                          (TIM3_PIT)								// 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用

#define ADC_CHANNEL1            		(ADC_CH13_P05)
#define ADC_CHANNEL2            		(ADC_CH14_P06)
#define ADC_CHANNEL3            		(ADC_CH8_P00)
#define ADC_CHANNEL4            		(ADC_CH10_P02)

extern uint16 adc_raw[4];
extern uint32 adc_filted[4]; 

//extern uint16 adc1;
//extern uint16 adc2;
//extern uint16 adc3;
//extern uint16 adc4;

float track_error(void);
void front_adc_init(void);
void read_adc(void);

#endif
