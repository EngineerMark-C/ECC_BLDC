#ifndef _imu_h_
#define _imu_h_

// 选择使用的IMU传感器
// 0: IMU963RA, 1: ICM42688, 2: imu660rb
#define USE_IMU_TYPE                    0

// 选择使用的姿态解算算法
// 0: Mahony, 1: Madgwick
#define USE_AHRS_ALGORITHM              0

#define PIT1                            (CCU60_CH1 )                            // 使用的周期中断编号

#define sampleFreq                      (200.0f)                                // sample frequency in Hz

#if USE_AHRS_ALGORITHM == 0
#define twoKpDef                        (2.0f * 0.5f)                           // 2 * proportional gain
#define twoKiDef                        (2.0f * 0.0005f)                        // 2 * integral gain
#else
#define betaDef                         (0.1f)                                  // Madgwick algorithm gain
#endif

extern float pitch, roll, yaw;

extern float gyro_bias[3];                                                      // 陀螺仪偏置
extern float acc_bias[3];                                                       // 加速度计偏置

void Imu_Init(void);
void Calibrate_Gyro(void);
void Calibrate_Acc(void);
void Imu_get_data(void);
void Imu_Update(void);

#endif
