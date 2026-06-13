// ============================================================
// 文件名: roundabout.c
// 功能说明: 环岛（roundabout）识别与控制模块
// 实现基于电感阈值和角度积分的环岛状态机，
// 包括预入环、打角入环、环内循迹、预出环、出环等阶段。
// ============================================================

#include "config.h"


uint8 current_state = STATE_NORMAL;      // 当前赛道元素状态机状态
uint8 round1_flag = 0;                   // 环岛1方向标志（0左/1右）
uint8 round_flag = 0;                    // 环岛进出岛判断标志（1表示正在处理环岛）

uint16 enter_distance1 = 5000;           // 环岛1入环前直行距离
uint16 out_distance1 = 5000;             // 环岛1出环后直行距离

int16 enter_angle1 = 40;                 // 环岛1入环初始打角角度
int16 pre_out_angle1 = 320;              // 环岛1预出环判断角度阈值
int16 out_angle1 = 360;                  // 环岛1出环目标角度


// 函数名: L_enter_judge
// 功能: 左环岛入环条件判断
// 说明: 当电感值满足左环岛特征（左外侧、左内侧、右内侧、右外侧均较强）且未进入环岛时，
//       将状态切换为左预入环状态。
void L_enter_judge(void)
{
	if(round_flag == 0 && round1_flag == 0 && (adc_filted[0] > 2300 && adc_filted[1] > 400 && adc_filted[2] > 1500 && adc_filted[3] > 1500))
	{
		round_flag = 1;
		current_state = ISLAND_LPREENTER;
	}
}		


// 函数名: L_ahead_judge
// 功能: 左环岛预入环阶段距离判断
// 说明: 在预入环状态下直线行驶，当累计距离达到设定入环距离后，
//       切换为左转入环执行状态。
void L_ahead_judge(void)
{
	if(Distance >= enter_distance1)
	{
		current_state = ISLAND_TURN_LEFT;
	}
}

// 函数名: entered_judge
// 功能: 入环打角完成判断
// 说明: 当角度误差绝对值小于2度时，认为入环姿态已调整好，
//       切换为环岛内部循迹状态。
void entered_judge(void)
{
	if (float_abs(angle_err) < 2.0)  // 角度误差小于2度，认为入环姿态已调整好 
	{
		current_state = ISLAND_IN;
	}		
}

// 函数名: pre_out_judge
// 功能: 环岛预出环判断
// 说明: 在环内循迹时，当车身角度超过预出环角度阈值（正向或负向），
//       认为已对准出口，切换为出环执行状态。
void pre_out_judge(void)
{
	if (angle_x > (pre_out_angle1) || angle_x < -(pre_out_angle1)) 
	{
		current_state = ISLAND_EXIT;
	}
}

// 函数名: exit_judge
// 功能: 出环打角完成判断
// 说明: 当角度误差绝对值小于2度时，认为出环姿态已调整好，
//       切换为出环后直行状态。
void exit_judge(void)
{
	if (float_abs(angle_err) < 2.0)  // 角度误差小于2度，认为出环姿态已调整好 
	{
		current_state = ISLAND_OUT;
	}		
}

// 函数名: outed_judge
// 功能: 出环完成判断
// 说明: 出环后直线行驶足够距离，确认已完全离开环岛，
//       清零环岛标志并返回正常循迹状态。
void outed_judge(void)
{
	if( Distance > out_distance1 )  // 行驶足够距离，确认出环完成 
	{
		round_flag = 0;		
		current_state = STATE_NORMAL;
	}
}

// 函数名: judge
// 功能: 赛道元素状态机主分支（预留扩展框架）
// 说明: 根据 current_state 分发到不同的环岛处理阶段。
//       当前主要实现左环岛流程，右环岛相关分支保留供后续扩展。
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

