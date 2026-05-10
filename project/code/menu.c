#include "config.h"

//uint8 page = 0;
//uint8 key = 0;


uint8 key_scan(void)
{
	uint8 key_status = 0;
	uint16 key_adc1 = 0,
		   key_adc2 = 0;
	
	key_adc1 = adc_mean_filter_convert(KEY_CHANNEL1, 3);
	key_adc2 = adc_mean_filter_convert(KEY_CHANNEL2, 3);
	
	switch( key_adc1 / 100 )
	{
		case 5 : key_status = UP;	 	break;		//542
		case 12: key_status = DOWN;	 	break;		//1262		//上下_选项
		case 18: key_status = OK;	 	break;		//1840      //中_确认
		case 24: key_status = LEFT;	 	break;		//2467      //左右_数值
		case 30: key_status = RIGHT; 	break;		//3082		
		default:						break;
	}		
	
	
	switch( key_adc2 / 100 )
	{
		case 5 : key_status = RST;	 	break;		//542		//初始化屏幕
		case 12: key_status = TWO;	 	break;		//1262		//贰叁肆_未定义 
		case 18: key_status = THREE;	break;		//1840		
		case 24: key_status = FOUR;	 	break;		//2467
		case 30: key_status = BACK; 	break;		//3082		//伍_返回
		default:						break;
	}
	
	return key_status;
}


//uint8 key_scan(void)
//{
//	uint8 key_status = 0;
//	if 		( gpio_get_level(KEY_CHANNEL1) == 0 ) key_status = OK;
//	else if ( gpio_get_level(KEY_CHANNEL2) == 0 ) key_status = UP;		//按键没焊好
//	else if ( gpio_get_level(KEY_CHANNEL3) == 0 ) key_status = DOWN;
//	else if ( gpio_get_level(KEY_CHANNEL4) == 0 ) key_status = BACK;
//	
//	return key_status;
//}

void menu(void)
{
	static uint8 page = 3;
	static uint8 O = 0;			//选项
	static uint8 key = 0,
				 last_key = 0;
	static bit D = 0;
	
	key = key_scan();
	system_delay_ms(10);
	
	if ( key != last_key )
	{
		switch( key )
		{
			case OK:				//Ok
				if ( page == 0 ) {
					if 		( O == 0 ) { page = 1; O = 0; }
					else if ( O == 1 ) { page = 2; O = 0; }
					else if ( O == 2 ) Start = 1;
					else if ( O == 3 ) Run = 1;
				}
				else if ( page == 2 ) {
					if	 	( O == 0 ) { page = 3; O = 0; }
					else if ( O == 1 ) { page = 4; O = 0; }
					else if ( O == 2 ) { page = 5; O = 0; }
				}
				else if ( page == 3||page == 4||page == 5 ) {
					D = 1;
				}
				system_delay_ms(10);
				ips114_clear(RGB565_WHITE);
			break;
				
			case UP:				//Up
				if ( page == 0 ) {
					if ( O == 0 ) O = 4;
					O--;
				}
				else if ( page == 2 ){
					if ( O == 0 ) O = 3;
					O--;
				}
				else if ( page == 3 ){
					if ( D == 1 ){
						if 		( O == 0 ) KP_x += 0.01;
						else if ( O == 1 ) K2P_x += 0.01;
						else if ( O == 2 ) KD_x += 0.01;
						else if ( O == 3 ) KP_v += 0.01;
						else if ( O == 4 ) KI_v += 0.01;
						else if ( O == 5 ) real_speed_L += 100;
					}
					else {
						if ( O == 0 ) O = 6;
						O--;
					}
				}
				else if ( page == 4 ){
					if ( O == 0 ) O = 6;
					O--;
				}
				else if ( page == 5 ){
					if ( O == 0 ) O = 6;
					O--;
				}
				
				system_delay_ms(10);
				ips114_clear(RGB565_WHITE);
			break;
				
			case DOWN:				//Down
				if ( page == 0 ) {
					O++;
					if ( O > 3 ) O = 0;
				}
				else if ( page == 2 ){
					O++;
					if ( O > 2 ) O = 0;
				}
				else if ( page == 3 ){
					if ( D == 1 ){
						if 		( O == 0 ) KP_x -= 0.01;
						else if ( O == 1 ) K2P_x -= 0.01;
						else if ( O == 2 ) KD_x -= 0.01;
						else if ( O == 3 ) KP_v -= 0.01;
						else if ( O == 4 ) KI_v -= 0.01;
						else if ( O == 5 ) real_speed_L -= 100;
					}
					else {
						O++;
						if ( O > 5 ) O = 0;
					}
				}
				else if ( page == 4 ){
					O++;
					if ( O > 5 ) O = 0;
				}
				else if ( page == 5 ){
					O++;
					if ( O > 5 ) O = 0;
				}
				
				system_delay_ms(10);
				ips114_clear(RGB565_WHITE);
			break;
				
			case BACK:				//Back
				if ( D == 1 ) D = 0;
				else page = 0;
				
				system_delay_ms(10);
				ips114_clear(RGB565_WHITE);
			break;
			
			case RST:				//Rst			
				ips114_init();
//				key_init();
				system_delay_ms(10);
				ips114_clear(RGB565_WHITE);
			break;
			
			default:
    		break;
		}
	}
	
	if ( D == 1 ) ips114_show_char(210, 16*O, '<');
	ips114_show_string(220, 16*O, "<<");
	
	switch( page )
	{
		case 0:
			ips114_show_string(2, 16*0, "Inductance");
			ips114_show_string(2, 16*1, "Mode");
			ips114_show_string(2, 16*2, "Start");
			ips114_show_string(2, 16*3, "Run");
		break;
		case 1:	
			ips114_show_int16(2, 16*0, target_speed_L);
//			ips114_show_int16(2, 16*1, pV_R);
		
//			ips114_show_uint16(2, 16*5, adc1);
//			ips114_show_uint16(2, 16*6, adc2);
		
//			ips114_show_uint16(2, 16*2, adc3);
//			ips114_show_uint16(2, 16*3, adc4);
//		
//			ips114_show_float(2, 16*5, diff, 3, 6);
//			ips114_show_float(2, 16*6, track_out, 3, 6);
		
		break;                          
		case 2:                         
			ips114_show_string(2, 16*0, "Steady");
			ips114_show_string(2, 16*1, "Ordinary");
			ips114_show_string(2, 16*2, "Fast");                      
		break;                          
		case 3:                         
			ips114_show_string(2, 16*0, "KP_x");	ips114_show_float(50, 16*0, KP_x, 1, 2);
			ips114_show_string(2, 16*1, "K2P_x");   ips114_show_float(50, 16*1, K2P_x, 1, 2);
			ips114_show_string(2, 16*2, "KD_x");    ips114_show_float(50, 16*2, KD_x, 1, 2);
			ips114_show_string(2, 16*3, "KP_v");	ips114_show_float(50, 16*3, KP_v, 1, 2);
			ips114_show_string(2, 16*4, "KI_v");    ips114_show_float(50, 16*4, KI_v, 1, 2);
			ips114_show_string(2, 16*5, "KD_v");    ips114_show_int16(50, 16*5, real_speed_L);
		break;
		case 4:
			ips114_show_string(2, 16*0, "KP_x");
			ips114_show_string(2, 16*1, "KI_x");
			ips114_show_string(2, 16*2, "KD_x");
			ips114_show_string(2, 16*3, "KP_v");
			ips114_show_string(2, 16*4, "KI_v");
			ips114_show_string(2, 16*5, "KD_v");
		break;                          
		case 5:                         
			ips114_show_string(2, 16*0, "KP_x");
			ips114_show_string(2, 16*1, "KI_x");
			ips114_show_string(2, 16*2, "KD_x");
			ips114_show_string(2, 16*3, "KP_v");
			ips114_show_string(2, 16*4, "KI_v");
			ips114_show_string(2, 16*5, "KD_v");
		break;
		
	}
	
	last_key = key;
}

void key_init(void)
{
//	gpio_init(KEY_CHANNEL1, GPI, 0, GPI_PULL_UP);
//	gpio_init(KEY_CHANNEL2, GPI, 0, GPI_PULL_UP);
//	gpio_init(KEY_CHANNEL3, GPI, 0, GPI_PULL_UP);
//	gpio_init(KEY_CHANNEL4, GPI, 0, GPI_PULL_UP);
	adc_init(KEY_CHANNEL1, ADC_12BIT);
	adc_init(KEY_CHANNEL2, ADC_12BIT);
}
