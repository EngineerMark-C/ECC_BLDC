#ifndef _camera_h_
#define _camera_h_

extern uint8_t Camera_Threshold;                                                // 默认二值化阈值为64

void Camera_Init(void);

// void Camera_BinarizeImage(uint8_t threshold);
// void Camera_ProcessImage(void);
// void Control_SetDirection(int16_t direction_error, float target_angle);


#endif