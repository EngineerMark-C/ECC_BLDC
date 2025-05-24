#include "init.h"

void Init(void)
{
    IPS_Init();                                                                 // 初始化 IPS
    Switch_Init();                                                              // 初始化拨码开关
    Button_Init();                                                              // 初始化按键
    Basic_Data_Init();                                                          // 初始化基本数据
    GPS_Points_Init();                                                          // 初始化 GPS 点位
    INS_Points_Init();                                                          // 初始化 INS 点位
    WGS84_to_ENU_Init();                                                        // 初始化 WGS84 坐标转 ENU 坐标
    S_Point_Init();                                                             // 初始化 S 型走位点
    Encoder_Init();                                                             // 初始化编码器
    Steer_Init();                                                               // 初始化舵机
    BLDC_Init();                                                                // 初始化 BLDC 驱动
    //Camera_Init();                                                              // 初始化摄像头
    Imu_Init();                                                                 // 初始化 IMU
    GPS_Init();                                                                 // 初始化 GPS
    INS_Init();                                                                  // 初始化惯导系统
    // UartReceiver_Init();                                                        // 初始化 UART 接收器
    PID_init(&pid_speed, 0.0f, 200.0f, 0.0f, 0, 10000);                          // 初始化电机 PID 控制器
    PID_init(&pid_steer, 1.0f, 0.0f, 0.0f, 0, 100);                            // 初始化舵机 PID 控制器
}
