#include "init.h"

void Init(void)
{
    Encoder_Init();                                                             // 初始化编码器
    BLDC_Init();                                                                // 初始化 BLDC 驱动
    Camera_Init();                                                              // 初始化摄像头
    Imu_Init();                                                                 // 初始化 IMU
    UartReceiver_Init();                                                        // 初始化 UART 接收器
}
