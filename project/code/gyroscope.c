// ============================================================
// 文件名: gyroscope.c
// 功能说明: 陀螺仪与加速度计数据变量定义
// 定义IMU（六轴传感器）各轴的加速度和角速度全局变量。
// ============================================================

#include "config.h"

float accel_x = 0;      // X轴加速度
float accel_y = 0;      // Y轴加速度
float accel_z = 0;      // Z轴加速度
float gyro_x = 0;       // X轴陀螺仪角速度
float gyro_y = 0;       // Y轴陀螺仪角速度
float gyro_z = 0;       // Z轴陀螺仪角速度

float velocity_x = 0;
float velocity_y = 0;
float velocity_z = 0;
float angle_x = 0;
float angle_y = 0;
float angle_z = 0;

static float accel_offset_x = 0;	 // X轴加速度零偏
static float accel_offset_y = 0;	 // Y轴加速度零偏
static float accel_offset_z = 0;	 // Z轴加速度零偏
static float gyro_offset_x = 0; 	 // X轴陀螺仪零偏
static float gyro_offset_y = 0;	 	 // Y轴陀螺仪零偏
static float gyro_offset_z = 0;	 	 // Z轴陀螺仪零偏


void read_accel_velocity(void)
{
	static float accel_last_x = 0;
	static float accel_last_y = 0;
	static float accel_last_z = 0;
	
	imu660rb_get_gyro();
	
	accel_x = ((float)imu660rb_acc_x - accel_offset_x) / 14.3f;
	accel_y = ((float)imu660rb_acc_y - accel_offset_y) / 14.3f;
	accel_z = ((float)imu660rb_acc_z - accel_offset_z) / 14.3f;
	
	velocity_x += (gyro_x + accel_last_x) * 0.001f;		//0.5 * 0.002
	velocity_y += (gyro_y + accel_last_y) * 0.001f;
	velocity_z += (gyro_z + accel_last_z) * 0.001f;
	
	accel_last_x = accel_x;
	accel_last_y = accel_y;
	accel_last_z = accel_z;
	
}


void read_gyro_angle(void)
{
	static float gyro_last_x = 0;
	static float gyro_last_y = 0;
	static float gyro_last_z = 0;
	
	imu660rb_get_gyro();
	
	gyro_x = ((float)imu660rb_gyro_x - gyro_offset_x) / 14.3f;
	gyro_y = ((float)imu660rb_gyro_y - gyro_offset_y) / 14.3f;
	gyro_z = ((float)imu660rb_gyro_z - gyro_offset_z) / 14.3f;
	
	angle_x += (gyro_x + gyro_last_x) * 0.001f;		//0.5 * 0.002
	angle_y += (gyro_y + gyro_last_y) * 0.001f;
	angle_z += (gyro_z + gyro_last_z) * 0.001f;
	
	gyro_last_x = gyro_x;
	gyro_last_y = gyro_y;
	gyro_last_z = gyro_z;
	
}


void accel_calibrate(void)
{
	const int8 samples = 100;
	int16 temp[3]={0}; 		// 临时计算变量
	int8 i=0; 				// 循环计数器
	
	for(i=0; i<samples; i++) {
		imu660rb_get_acc();
		temp[0] += imu660rb_acc_x;
        temp[1] += imu660rb_acc_y;
        temp[2] += imu660rb_acc_z;
    }
	
    accel_offset_x = (float)temp[0] / (float)samples;
    accel_offset_y = (float)temp[1] / (float)samples;
    accel_offset_z = (float)temp[2] / (float)samples;
	
	velocity_x=0,	velocity_y=0,	velocity_z=0;
}


void gyro_calibrate(void)
{
	const int8 samples = 100;
	int16 temp[3]={0}; 		// 临时计算变量
	int8 i=0; 				// 循环计数器
	
	for(i=0; i<samples; i++) {
		imu660rb_get_gyro();
		temp[0] += imu660rb_gyro_x;
        temp[1] += imu660rb_gyro_y;
        temp[2] += imu660rb_gyro_z;
    }
	
    gyro_offset_x = (float)temp[0] / (float)samples;
    gyro_offset_y = (float)temp[1] / (float)samples;
    gyro_offset_z = (float)temp[2] / (float)samples;
	
	angle_x=0,	angle_y=0,	angle_z=0;
}



