#include "init.h"

void Init(void)
{
    IPS_Init();                                                                 // 初始化 IPS
    Boot_Animation();                                                           // 启动画面
    Switch_Init();                                                              // 初始化拨码开关
    Button_Init();                                                              // 初始化按键
    Flash_Init();                                                               // 初始化 Flash 存储
    Calculate_Safety_Boundary(Navigation_Flag);                                 // 计算安全边界
    Encoder_Init();                                                             // 初始化编码器
    Steer_Init();                                                               // 初始化舵机
    BLDC_Init();                                                                // 初始化 BLDC 驱动
    Imu_Init();                                                                 // 初始化 IMU
    GPS_Init();                                                                 // 初始化 GPS
    INS_Init();                                                                 // 初始化惯导系统
    PID_init(&pid_speed, 550.0f, 2800.0f, 0.0f, 1, 10000);                      // 初始化电机 PID 控制器
    PID_init(&pid_steer, 0.5f, 0.0f, 0.0f, 0, 100);                             // 初始化舵机 PID 控制器
    ips114_clear();                                                             // 清除 IPS 显示屏
    //Camera_Init();                                                              // 初始化摄像头
    // UartReceiver_Init();                                                       // 初始化 UART 接收器
    // Wifiudp_Init();                                                            // 初始化 WiFi 模块
    // Save_All_Flash_Data();
}

// MaxSpeed 5.5  p 550 i 2800 d 0
// MaxSpeed 12.0 p 520 i 3200 d 0
// MaxSpeed 16.0 p 560 i 4300 d 0
