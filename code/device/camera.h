#ifndef _camera_h_
#define _camera_h_

#include "init.h"

// 白线边界存储(用于显示)
typedef struct {
    int left_edge;
    int right_edge;
    int center;
    uint8_t valid;
} line_info_t;

extern uint8_t Camera_Threshold;                                                // 二值化阈值
extern uint8_t Camera_Exposure;                                                 // 曝光时间
extern float angle_adjustment;

extern uint8_t Camera_Image[MT9V03X_H][MT9V03X_W];
extern line_info_t Line_Info[MT9V03X_H];                                        // 线条信息

void Camera_Init(void);
void Camera_Process(void);
void Camera_Steer_Control(void);

int8_t Camera_Get_Line_Direction(void);                                         // 获取白线检测结果
int16_t Camera_Get_Line_Position(void);                                         // 获取白线中心位置
uint8_t Camera_Is_Line_Detected(void);                                          // 检查是否检测到白线
int16_t Camera_Get_Line_Offset(void);                                           // 获取白线偏移量

#endif