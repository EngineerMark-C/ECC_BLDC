#include "init.h"

void Init(void)
{
    IPS_Init();                                                                 // 初始化 IPS
    Button_Init();                                                              // 初始化按键
    Encoder_Init();                                                             // 初始化编码器
    Steer_Init();                                                               // 初始化舵机
    BLDC_Init();                                                                // 初始化 BLDC 驱动
    // Camera_Init();                                                              // 初始化摄像头
    Imu_Init();                                                                 // 初始化 IMU
    UartReceiver_Init();                                                        // 初始化 UART 接收器
    PID_init(&pid_speed, 0.0f, 200.0f, 0.0f, 0, 10000);                          // 初始化电机 PID 控制器
    PID_init(&pid_steer, 1.0f, 0.0f, 0.0f, 0, 100);                            // 初始化舵机 PID 控制器
}
