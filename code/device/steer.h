#ifndef _steer_h_
#define _steer_h_

#define STEER_PIN                       (ATOM3_CH3_P00_12)                       // 舵机PWM输出引脚

// 老 R 车舵机参数 
// R 车 光荣退役
// #define PWM_STEER_MIN_R                 655                                     // R 车舵机PWM右极限
// #define PWM_STEER_MIDDLE_R              765                                     // R 车舵机PWM中值
// #define PWM_STEER_MAX_R                 885                                     // R 车舵机PWM左极限
// #define MAX_ANGLE_R                     21.3f                                   // R 车舵机最大转角

#define PWM_STEER_MIN_R                 580                                     // R 车舵机PWM右极限
#define PWM_STEER_MIDDLE_R              765                                     // R 车舵机PWM中值
#define PWM_STEER_MAX_R                 940                                     // R 车舵机PWM左极限
#define MAX_ANGLE_R                     30.0f                                   // R 车舵机最大转角

extern int16_t STEER_duty;

void Steer_Init(void);
void Steer_set_duty(int16_t duty);
void Steer_set_angle(float angle);
void Steer_angle(float angle);

#endif