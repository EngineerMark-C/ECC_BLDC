#include "init.h"

volatile float twoKp = twoKpDef;                                                // 2 * proportional gain (Kp)
volatile float twoKi = twoKiDef;                                                // 2 * integral gain (Ki)
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;                      // quaternion of sensor frame relative to auxiliary frame
volatile float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;     // integral error terms scaled by Ki

float pitch = 0.0f, roll = 0.0f, yaw = 0.0f;                                    // 欧拉角
float ax_raw, ay_raw, az_raw;                                                   // 加速度计数据
float gx_raw, gy_raw, gz_raw;                                                   // 原始陀螺仪数据

float gyro_bias[3] = {0.0f, 0.0f, 0.0f};                                        // 陀螺仪偏置
float acc_bias[3] = {0.0f, 0.0f, 0.0f};                                         // 加速度计偏置

float invSqrt(float x)
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

// 初始化 IMU963RA
void Imu_Init(void)
{
    imu963ra_init();
    pit_ms_init(PIT1, 5);                                                       // 初始化PIT1为周期中断5ms周期
}

// 获取 IMU963RA 数据
void Imu_get_data(void)
{
    imu963ra_get_gyro();                                                        // 获取 IMU963RA 陀螺仪数据
    imu963ra_get_acc();                                                         // 获取 IMU963RA 加速度计数据
    // imu963ra_get_mag();                                                      // 获取 IMU963RA 磁力计数据

    // 获取加速度计和陀螺仪数据并转换为实际物理值
    ax_raw = imu963ra_acc_transition(imu963ra_acc_x);
    ay_raw = imu963ra_acc_transition(imu963ra_acc_y);
    az_raw = imu963ra_acc_transition(imu963ra_acc_z);
    gx_raw = imu963ra_gyro_transition(imu963ra_gyro_x);
    gy_raw = imu963ra_gyro_transition(imu963ra_gyro_y);
    gz_raw = imu963ra_gyro_transition(imu963ra_gyro_z);
}

// 陀螺仪偏置校准函数
void Calibrate_Gyro(void)
{
    ips114_show_string(60, 0, "Gyro Calibrating...");
    ips114_show_string(60, 16, "Keep IMU Still");
    
    int samples = 1000;  // 采样次数
    float sum_x = 0.0f, sum_y = 0.0f, sum_z = 0.0f;
    char progress_str[20];
    pit_disable(PIT1);
    system_delay_ms(1000);  // 延时1秒

    // 采集静止状态下的陀螺仪数据
    for(int i = 0; i < samples; i++)
    {
        imu963ra_get_gyro();
        sum_x += imu963ra_gyro_transition(imu963ra_gyro_x);
        sum_y += imu963ra_gyro_transition(imu963ra_gyro_y);
        sum_z += imu963ra_gyro_transition(imu963ra_gyro_z);
        system_delay_ms(5);  // 5ms,与实际采样周期一致
        
        if(i % 100 == 0)  // 每100次采样更新一次进度
        {
            sprintf(progress_str, "Progress:%d%%", i / 10);
            ips114_show_string(60, 32, progress_str);
        }
    }
    
    // 计算平均值作为偏置
    gyro_bias[0] = sum_x / samples;
    gyro_bias[1] = sum_y / samples;
    gyro_bias[2] = sum_z / samples;
    
    // 显示校准结果
    ips114_show_string(60, 48, "Gyro Bias:");
    ips114_show_float(60, 64, gyro_bias[0], 6, 2);  // X
    ips114_show_float(60, 80, gyro_bias[1], 6, 2);  // Y
    ips114_show_float(60, 96, gyro_bias[2], 6, 2);  // Z
    ips114_show_string(60, 112, "Calibration Done!");
    pit_enable(PIT1);
    system_delay_ms(1000);
}

// 加速度计校准函数
void Calibrate_Acc(void)
{
    ips114_show_string(60, 0, "Acc Calibrating...");
    ips114_show_string(60, 16, "Keep IMU Still & Level");
    
    int samples = 1000;  // 采样次数
    float sum_x = 0.0f, sum_y = 0.0f, sum_z = 0.0f;
    char progress_str[20];
    
    system_delay_ms(1000);  // 延时1秒等待稳定

    // 采集静止状态下的加速度计数据
    for(int i = 0; i < samples; i++)
    {
        imu963ra_get_acc();
        sum_x += imu963ra_acc_transition(imu963ra_acc_x);
        sum_y += imu963ra_acc_transition(imu963ra_acc_y);
        sum_z += imu963ra_acc_transition(imu963ra_acc_z);
        system_delay_ms(5);  // 5ms采样间隔
        
        if(i % 100 == 0)  // 更新进度
        {
            sprintf(progress_str, "Progress:%d%%", i / 10);
            ips114_show_string(60, 32, progress_str);
        }
    }
    
    // 计算静态偏置（注意：z轴需减去1g重力加速度）
    acc_bias[0] = sum_x / samples;
    acc_bias[1] = sum_y / samples;
    acc_bias[2] = sum_z / samples - 1.0f;  // 减去重力加速度
    
    // 显示校准结果
    ips114_show_string(60, 48, "Acc Bias:");
    ips114_show_float(60, 64, acc_bias[0], 6, 2);  // X
    ips114_show_float(60, 80, acc_bias[1], 6, 2);  // Y
    ips114_show_float(60, 96, acc_bias[2], 6, 2);  // Z
    ips114_show_string(60, 112, "Acc Calibration Done!");
    system_delay_ms(1000);
}

void MahonyAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az)
{
	float recipNorm;
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;
	float qa, qb, qc;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;        

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		halfvx = q1 * q3 - q0 * q2;
		halfvy = q0 * q1 + q2 * q3;
		halfvz = q0 * q0 - 0.5f + q3 * q3;
	
		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		// Compute and apply integral feedback if enabled
		if(twoKi > 0.0f) {
			integralFBx += twoKi * halfex * (1.0f / sampleFreq);	// integral error scaled by Ki
			integralFBy += twoKi * halfey * (1.0f / sampleFreq);
			integralFBz += twoKi * halfez * (1.0f / sampleFreq);
			gx += integralFBx;	// apply integral feedback
			gy += integralFBy;
			gz += integralFBz;
		}
		else {
			integralFBx = 0.0f;	// prevent integral windup
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gx += twoKp * halfex;
		gy += twoKp * halfey;
		gz += twoKp * halfez;
	}
	
	// Integrate rate of change of quaternion
	gx *= (0.5f * (1.0f / sampleFreq));		// pre-multiply common factors
	gy *= (0.5f * (1.0f / sampleFreq));
	gz *= (0.5f * (1.0f / sampleFreq));
	qa = q0;
	qb = q1;
	qc = q2;
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx); 
	
	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}

//四元素解算
void Imu_Update(void)
{    
    float gx, gy, gz;

    gx = ANGLE_TO_RAD((gx_raw - gyro_bias[0]));
    gy = ANGLE_TO_RAD((gy_raw - gyro_bias[1]));
    gz = ANGLE_TO_RAD((gz_raw - gyro_bias[2]));
    
    // printf("%.5f, %.5f, %.5f\n", gx, gy, gz);

    MahonyAHRSupdateIMU(gx, gy, gz, ax_raw, ay_raw, az_raw);

    // 计算欧拉角（弧度）
    pitch = asin(2 * (q0 * q2 - q1 * q3));
    roll = atan2(2 * (q0 * q1 + q2 * q3), 1 - 2 * (q1 * q1 + q2 * q2));
    yaw = -atan2(2 * (q0 * q3 + q1 * q2), 1 - 2 * (q2 * q2 + q3 * q3));
    
    // 转换为角度
    pitch = RAD_TO_ANGLE(pitch);
    roll = RAD_TO_ANGLE(roll);
    yaw = RAD_TO_ANGLE(yaw);
    
    // 将yaw从-180~180转换为0~360
    yaw = yaw < 0 ? yaw + 360 : yaw;
}
