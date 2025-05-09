#ifndef _imu_h_
#define _imu_h_

#define PIT1                            (CCU60_CH1 )                            // 使用的周期中断编号

#define sampleFreq                      200.0f                                  // sample frequency in Hz
#define twoKpDef                        (2.0f * 0.5f)                           // 2 * proportional gain
#define twoKiDef                        (2.0f * 0.0005f)                        // 2 * integral gain

extern float pitch, roll, yaw;

extern float gyro_bias[3];                                         // 陀螺仪偏置
extern float acc_bias[3];                                          // 加速度计偏置

void Imu_Init(void);
void Calibrate_Gyro(void);
void Calibrate_Acc(void);
void Imu_get_data(void);
void Imu_Update(void);

#endif
