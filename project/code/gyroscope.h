#ifndef _GYROSCOPE_H_
#define _GYROSCOPE_H_

extern float accel_x;       // X轴加速度值
extern float accel_y;       // Y轴加速度值
extern float accel_z;       // Z轴加速度值
extern float gyro_x;        // X轴陀螺仪角速度数据
extern float gyro_y;        // Y轴陀螺仪角速度数据
extern float gyro_z;        // Z轴陀螺仪角速度数据

extern float velocity_x;
extern float velocity_y;
extern float velocity_z;
extern float angle_x;
extern float angle_y;
extern float angle_z;

void read_accel_velocity(void);
void read_gyro_angle(void);
void accel_calibrate(void);
void gyro_calibrate(void);


#endif
