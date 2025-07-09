#include "init.h"

uint8_t Camera_Threshold;                                          // 默认二值化阈值为128
// uint8_t BinarizedImage[MT9V03X_H][MT9V03X_W];                           // 二值化后的图像数组
// int16_t WhiteRegionCenter = 0;                                           // 白色区域中心位置
// int16_t LastValidCenter = MT9V03X_W / 2;                                // 上一次有效的中心位置
// int16_t MotorSpeed = 1000;                                              // 电机速度，可调整

// // 长方形检测的结构体
// typedef struct {
//     int16_t left;       // 左边界
//     int16_t right;      // 右边界
//     int16_t top;        // 上边界
//     int16_t bottom;     // 下边界
//     int16_t center_x;   // 中心x坐标
//     int16_t center_y;   // 中心y坐标
//     float angle;        // 长边方向角度
// } Rectangle_t; 

// Rectangle_t WhiteRectangle = {0};

void Camera_Init(void)
{
    // 初始化摄像头
    mt9v03x_init();
}

// /**
//  * @brief 对图像进行二值化处理
//  * @param threshold 二值化阈值
//  */
// void Camera_BinarizeImage(uint8_t threshold)
// {
//     uint8_t i, j;
    
//     // 遍历整个图像并进行二值化
//     for(i = 0; i < MT9V03X_H; i++)
//     {
//         for(j = 0; j < MT9V03X_W; j++)
//         {
//             if(mt9v03x_image[i][j] > threshold)
//                 BinarizedImage[i][j] = 255;  // 白色
//             else
//                 BinarizedImage[i][j] = 0;    // 黑色
//         }
//     }
// }

// /**
//  * @brief 检测白色长方形区域
//  * @return 是否成功检测到长方形
//  */
// uint8_t Camera_DetectWhiteRectangle(void)
// {
//     int16_t i, j;
//     int16_t left = MT9V03X_W, right = 0, top = MT9V03X_H, bottom = 0;
//     uint8_t found = 0;
    
//     // 扫描整个图像找到白色区域的边界
//     for(i = 0; i < MT9V03X_H; i++)
//     {
//         for(j = 0; j < MT9V03X_W; j++)
//         {
//             if(BinarizedImage[i][j] == 255)
//             {
//                 found = 1;
                
//                 // 更新边界
//                 if(j < left) left = j;
//                 if(j > right) right = j;
//                 if(i < top) top = i;
//                 if(i > bottom) bottom = i;
//             }
//         }
//     }
    
//     if(!found) return 0;
    
//     // 保存长方形信息
//     WhiteRectangle.left = left;
//     WhiteRectangle.right = right;
//     WhiteRectangle.top = top;
//     WhiteRectangle.bottom = bottom;
    
//     // 计算中心点
//     WhiteRectangle.center_x = (left + right) / 2;
//     WhiteRectangle.center_y = (top + bottom) / 2;
    
//     // 计算长边方向角度（相对于垂直方向）
//     int16_t width = right - left;
//     int16_t height = bottom - top;
    
//     // 由于长方形高大于宽，角度将基于长边计算
//     if(height > width)
//     {
//         // 长边垂直，车应沿垂直方向行驶
//         WhiteRectangle.angle = 0;
//     }
//     else
//     {
//         // 长边水平，车应沿水平方向行驶
//         WhiteRectangle.angle = 90;
//     }
    
//     return 1;
// }

// /**
//  * @brief 分析图像并控制车辆方向
//  */
// void Camera_ProcessImage(void)
// {
//     // 对图像进行二值化处理
//     Camera_BinarizeImage(Camera_Threshold);
    
//     // 检测白色长方形
//     if(Camera_DetectWhiteRectangle())
//     {
//         // 计算车辆中心与长方形中心的横向偏差
//         int16_t center_error = WhiteRectangle.center_x - (MT9V03X_W / 2);
        
//         // 控制转向，使车辆对准长方形中心
//         Control_SetDirection(center_error, WhiteRectangle.angle);
        
//         // 更新最后有效的中心位置
//         LastValidCenter = WhiteRectangle.center_x;
//     }
//     else
//     {
//         // 未检测到长方形，使用上次有效的中心位置继续前进
//         int16_t center_error = LastValidCenter - (MT9V03X_W / 2);
//         Control_SetDirection(center_error, 0);
//     }
    
//     // 控制车辆前进
//     BLDC_Set_Duty(MotorSpeed);
// }

// /**
//  * @brief 控制车辆方向
//  * @param direction_error 方向偏差，正值向右转，负值向左转
//  * @param target_angle 目标角度，长方形长边的方向
//  */
// void Control_SetDirection(int16_t direction_error, float target_angle)
// {
//     // 比例系数，需要调试
//     float kp = 0.1;
    
//     // 根据长方形的方向和中心偏差计算转向角度
//     float steering_angle = kp * direction_error;
    
//     // 考虑长方形方向的影响
//     if(target_angle == 90) // 长边水平
//     {
//         // 如果长边水平，调整方向使车辆走直线
//         steering_angle = 0;
//     }
    
//     // 限制转向角度
//     if(steering_angle > MAX_ANGLE_R) steering_angle = MAX_ANGLE_R;
//     if(steering_angle < -MAX_ANGLE_R) steering_angle = -MAX_ANGLE_R;
    
//     // 设置舵机角度
//     Steer_set_angle(steering_angle);
// }

// /**
//  * @brief 图像处理主函数，在主循环中调用
//  */
// void Camera_Main(void)
// {
//     // 等待摄像头采集完成
//     if(mt9v03x_finish_flag)
//     {
//         // 处理图像
//         Camera_ProcessImage();
        
//         // 重置标志位，准备下一帧图像采集
//         mt9v03x_finish_flag = 0;
//     }
// }

