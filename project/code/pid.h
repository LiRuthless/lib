#ifndef _PID_H_
#define _PID_H_


				 
extern float KP_v;
extern float KI_v;
extern float KD_v;
				 
extern float KP_x;
extern float K2P_x;
extern float KI_x;
extern float KD_x;



float PID_track(float track_error);
int16 PID_L(int16 target_speed_L, int16 real_speed_L);
int16 PID_R(int16 target_speed_R, int16 real_speed_R);

#endif
