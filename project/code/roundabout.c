#include "config.h"


uint8 current_state = STATE_NORMAL;
uint8 round1_flag = 0; 			// 环岛1方向标志（0左/1右）
uint8 round_flag = 0;			//用作判断进出岛

uint16 enter_distance1 = 5000; 	// 环岛1入环距离
uint16 out_distance1 = 5000; 	// 环岛1出环距离

int16 enter_angle1 = 40;
int16 pre_out_angle1 = 320; 		// 环岛1预出环角度
int16 out_angle1 = 360; 			// 环岛1出环目标角度



void L_enter_judge(void)
{
	if(round_flag == 0 && round1_flag == 0 && (adc_filted[0] > 2300 &&adc_filted[1] > 400 &&adc_filted[2] > 1500 &&adc_filted[3] > 1500))
	{
		round_flag = 1;
		current_state = ISLAND_LPREENTER;
	}
}		


void L_ahead_judge(void)
{
	if(Distance >= enter_distance1)
	{
		current_state = ISLAND_TURN_LEFT;
	}
}

void entered_judge(void)
{
	if (float_abs(angle_err) < 2.0)  // 角度误差小于2度，认为入环姿态已调整好 
	{
		current_state = ISLAND_IN;
	}		
}

void pre_out_judge(void)
{
	if (angle_x > (pre_out_angle1) || angle_x < -(pre_out_angle1)) 
	{
		current_state = ISLAND_EXIT;
	}
}
void exit_judge(void)
{
	if (float_abs(angle_err) < 2.0)  // 角度误差小于2度，认为入环姿态已调整好 
	{
		current_state = ISLAND_OUT;
	}		
}

void outed_judge(void)
{
	if( Distance > out_distance1 )  // 行驶足够距离，确认出环完成 
	{
		round_flag = 0;		
		current_state = STATE_NORMAL;
	}
}

void judge(void)
{
	switch (current_state)  // 赛道元素状态机主分支
	{
        case STATE_NORMAL:  // 正常循迹状态 0
						
		// 若choose_flag=1（菜单中启用环岛功能），则根据当前环岛编号调用对应的入环判断函数
//			if(choose_flag == 1)                      
//			{
				// 环岛1：同时判断左入环和右入环条件（根据round1_flag决定哪个方向生效）
				L_ahead_judge();		
//			}
//			if(cross_flag == 1)		crossroad_judge();
		break;
			
		case ISLAND_LPREENTER://1  // 左预入环状态
			
			//识别到即将入岛
			//当距离达到设定值 进行入岛
			L_enter_judge();
		break;
						
		case ISLAND_RPREENTER://2  // 右预入环状态

			//识别到即将入岛
			//当距离达到设定值 进行入岛
//			renter_judge();								
		break;
				
		case ISLAND_LENTER://1  // 入环判断状态

			//识别到即将入岛
			//当距离达到设定值 进行入岛
			entered_judge();	
			break;
						
		case ISLAND_RENTER://2  // 入环判断状态

			//识别到即将入岛
			//当距离达到设定值 进行入岛
			entered_judge();									
		break;
						
		case ISLAND_TURN_LEFT://3  // 左转入环执行状态

			entered_judge();
		break;

		case ISLAND_TURN_RIGHT://4  // 右转入环执行状态

//			entered_judge();
		break;	
				
        case ISLAND_IN:                   //     5  // 环岛内部循迹状态

//			send_end((float)STATE_IN_ISLAND);
            // 检测环岛出口
            pre_out_judge();
        break;
		
        case ISLAND_EXIT:                 //     4  // 出环执行状态
			//电感值再次回归正常
            exit_judge();
        break;
				
//		case CROSSROAD:  // 十字交叉状态

//			crossroad_out();
//		break;
				
	}
}


