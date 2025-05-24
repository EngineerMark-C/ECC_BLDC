#include "init.h"

uint8_t Camera_Threshold;                                                  // 默认二值化阈值为64

void Camera_Init(void)
{
    // 初始化摄像头
    mt9v03x_init();
}

