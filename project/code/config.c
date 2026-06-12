// ============================================================
// 文件名: config.c
// 功能说明: 系统初始化与通用工具函数模块
// 本文件包含系统外设统一初始化函数 All_init()，
// 以及浮点数绝对值等通用辅助函数。
// ============================================================

#include "config.h"

uint8 uart[32];     		// 串口数据发送缓冲区
uint8 dat[32];						// 串口数据接收缓冲区

int16 battery = 0;					// 电池电压ADC原始值
int16 battery_filt = 0;				// 电池电压滤波值

bit Start_flag = 0;              // 启动标志位（按键控制）
bit Run_flag = 0;                // 运行标志位（赛道检测控制）

LowPassFilter filt_battery;			// 电池电压低通滤波器

// 函数名: All_init
// 功能: 系统所有外设统一初始化
// 说明: 依次初始化串口、无线串口、显示屏、ADC、编码器、电机、定时器中断。
void All_init(void)
{
    uart_init(UART_1, 115200, UART1_TX_P31, UART1_RX_P30); // 初始化串口1，波特率115200
    wireless_uart_init();   // 初始化无线串口
	uart_rx_interrupt(UART_1, ENABLE);	// 使能串口1接收中断
	uart_rx_start_buff(UART_1);			// 启动串口1接收缓冲
	
	iap_init();							// 初始化IAP（Flash模拟EEPROM）
	
	adc_init(ADC_CH11_P03, ADC_12BIT);	// 初始化电池电压检测ADC，12位精度
	gpio_init(IO_P10, GPO, 0, GPO_PUSH_PULL);	// 初始化蜂鸣器GPIO（P10），推挽输出
	lowpass_init(&filt_battery, 0.2);	// 初始化电池电压低通滤波器
    
    ips114_init();          // 初始化IPS114显示屏
	key_init();             // 初始化按键ADC
    
    front_adc_init();       // 初始化前置四路电感ADC
    encoder_init();         // 初始化编码器
    motor_init();           // 初始化电机PWM
	
	imu660rb_init();					// 初始化IMU660RB六轴传感器
	
    pit_ms_init(PIT_TR, 5);    // 初始化5ms定时中断（循迹控制）
    pit_ms_init(PIT_SP, 2);    // 初始化2ms定时中断（速度环）
}


// 函数名: key_start
// 功能: 启动按键检测与电池电压监测
// 说明: 检测IO_P22按键下降沿翻转Start标志，同时监测电池电压。
//       电池电压过低时蜂鸣器报警并强制停止。
void key_start(void)
{
	static uint8 last_key = 1; 		 // 上一次按键电平
	uint8 cur_key = 1;
	
	cur_key = gpio_get_level(IO_P22);

	if(last_key && !cur_key){		 // 检测到下降沿
		Start_flag = !Start_flag;
	}
	
	last_key = cur_key;
	
//	sprintf(uart,"%d,%d\n",cur_key, Start);
// 	uart_write_buffer(UART_1,uart,strlen(uart));

	battery = adc_convert(ADC_CH11_P03);
	battery_filt = (int16)lowpass_update(&filt_battery, (float)battery);
	
//	sprintf(uart,"%d\n",battery_filt);
// 	uart_write_buffer(UART_1,uart,strlen(uart));
	

//	while( battery < 1200 && battery > 300 )		//max1308-12.6v  min1182-11.4  1192-11.5
//	{
//		Start = 0;
//		
//		P10 = 1;
//		system_delay_ms(100);
//		P10 = 0;
//		system_delay_ms(100);

//	battery = adc_convert(ADC_CH11_P03);
//	battery_filt = (int16)lowpass_update(&filt_battery, (float)battery);
//	}
	
	if( Start_flag )
	{
		PID_outL = 0.0;				// 启动时清零左轮PID累计输出
		PID_outR = 0.0;				// 启动时清零右轮PID累计输出
	}
}

// 函数名: uart_adjust
// 功能: 串口无线调参解析
// 说明: 通过无线串口接收指令，解析并修改KP_v、KI_v、KP_x、KI_x等PID参数
void uart_adjust(void)
{
	static float figure = 0.0;

	uint32 len = wireless_uart_read_buffer(dat,sizeof(dat));
	
	
	switch( len ){
		case 4: figure = 0.01 * (dat[2]-'0');		break;
		case 5: figure = 0.01 * (dat[3]-'0') + 0.1 * (dat[2]-'0');		break;
		case 6: figure = 0.01 * (dat[4]-'0') + 0.1 * (dat[3]-'0') + 1 * (dat[2]-'0');		break;
		case 7: figure = 0.01 * (dat[5]-'0') + 0.1 * (dat[4]-'0') + 1 * (dat[3]-'0') + 10 * (dat[2]-'0');		break;
	}
	
	switch( dat[0] ){
		case 'v':
			switch( dat[1] ){
				case 'p':  KP_v = figure;	break;
				case 'i':  KI_v = figure;	break;
			}
		break;
			
		case 'x':
			switch( dat[1] ){
				case 'p':  KP_x = figure;	break;
				case 'd':  KI_x = figure;	break;
			}
		break;	
	}
	
	memset(dat, 0, len);
	
//	sprintf(uart, "%f,%f\r\n", KP_v, figure);
//	uart_write_buffer(UART_1, uart, strlen(uart)); 
	
}




// 函数名: float_abs
// 功能: 计算浮点数的绝对值
// 参数: a - 输入浮点数
// 返回值: a的绝对值
float float_abs(float a)
{
    if(a < 0){      // 判断是否为负数
        a = -a;     // 负数取反
    }
    return a;       // 返回绝对值
}
