#ifndef _MOTOR_h_
#define _MOTOR_h_

//PWM
#define PIT_SP                          (TIM4_PIT)								// 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用

#define MOTOR_DIR_L               			(IO_P26)
#define MOTOR_PWM_L               			(PWMB_CH4_P27)
#define MOTOR_DIR_R               			(IO_P51)
#define MOTOR_PWM_R               			(PWMB_CH3_P25)
#define MOTOR_PWM_M               			(PWMB_CH1_P50)



//编码器
#define ENCODER_DIR_L                 	(TIM1_ENCOEDER)    
#define ENCODER_DIR_DIR_L              	(IO_P24)           
#define ENCODER_DIR_PULSE_L            	(TIM1_ENCOEDER_P35)

#define ENCODER_DIR_R                 	(TIM0_ENCOEDER)    
#define ENCODER_DIR_DIR_R           	(IO_P11)           
#define ENCODER_DIR_PULSE_R       		(TIM0_ENCOEDER_P34)



extern int16 target_speed_L;
extern int16 target_speed_R;
extern int16 real_speed_L;
extern int16 real_speed_R;

extern float alpha;

void motor_control(void);
void read_encoder(void);

void motor_init(void);
void encoder_init(void);


#endif
