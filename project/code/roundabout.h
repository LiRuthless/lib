#ifndef _ROUNFABOUT_H_
#define _ROUNDABOUT_H_


#define STATE_NORMAL 		0
#define ISLAND_LPREENTER 	1    	//左预进岛
#define ISLAND_RPREENTER 	2    	//右预进岛
#define ISLAND_TURN_LEFT 	3   	//左打角进岛
#define ISLAND_TURN_RIGHT 	4       //右打角进岛
#define ISLAND_IN          	5		//岛中
#define ISLAND_EXIT  		6		//出岛
#define ISLAND_LENTER 		9   	//走距离准备左进岛
#define ISLAND_RENTER 		10   	//走距离准备右进岛
#define ISLAND_OUT          11      // 走距离准备出岛

extern uint8 current_state;
extern uint8 round_flag;//用作判断进出岛

extern int8 enter_angle1;
extern int8 pre_out_angle1; // 环岛1预出环角度
extern int8 out_angle1;	 // 环岛1出环目标角度

//extern uint8 choose_flag; 			// 环岛使能标志
//extern uint8 round1_flag; 			// 环岛1方向标志（0左/1右）
//extern int16 lround_distance11; 	// 环岛1左入环距离
//extern int16 outdistance1; 			// 环岛1出环距离


void L_enter_judge(void);
void L_ahead_judge(void);
void entered_judge(void);
void pre_out_judge(void);
void exit_judge(void);
void outed_judge(void);
void judge(void);




#endif