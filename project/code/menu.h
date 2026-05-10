#ifndef _menu_h_
#define _menu_h_


#define OK				1
#define UP				2
#define DOWN			3
#define LEFT			4
#define RIGHT			5
#define BACK			6
#define RST				7
#define TWO				8
#define THREE			9
#define FOUR		   10

#define KEY_CHANNEL1            		(ADC_CH3_P13)
#define KEY_CHANNEL2            		(ADC_CH4_P14)

//#define KEY_CHANNEL1            		(IO_P05)
//#define KEY_CHANNEL2            		(IO_P06)
//#define KEY_CHANNEL3            		(IO_P46)
//#define KEY_CHANNEL4            		(IO_P07)


//extern uint8 page;
//extern uint8 key;
extern float track_out;
	
uint8 key_scan(void);
void menu(void);
void key_init(void);

#endif
