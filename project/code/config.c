#include "config.h"

uint8 uart[32];

void All_init(void)
{
	uart_init(UART_1, 115200, UART1_TX_P31, UART1_RX_P30);
	wireless_uart_init();
	
	ips114_init();
//	key_init();
	
	front_adc_init();
	encoder_init();
	motor_init();


	pit_ms_init(PIT_TR, 50);			//方向环中断初始化
	
	pit_ms_init(PIT_SP, 10);			//速度环中断初始化
}

float float_abs(float a)
{
	if(a < 0){
		a = -a;
	}
	return a;
}

