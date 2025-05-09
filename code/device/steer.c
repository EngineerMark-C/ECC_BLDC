#include "init.h"

int16_t STEER_duty = 0;

void Steer_Init(void)
{
    pwm_init(STEER_PIN, 50, PWM_STEER_MIDDLE_R);
}

void Steer_set_duty(int16_t duty)
{
    // 限制舵机PWM范围
    if (duty < PWM_STEER_MIN_R) duty = PWM_STEER_MIN_R;
    if (duty > PWM_STEER_MAX_R) duty = PWM_STEER_MAX_R;
    STEER_duty = duty;
    pwm_set_duty(STEER_PIN, duty);
}

void Steer_set_angle(float angle)
{
    //  duty = 中值 + (角度 / 最大角度) * (左极限 - 中值)
    if (angle > MAX_ANGLE_R) angle = MAX_ANGLE_R;
    if (angle < -MAX_ANGLE_R) angle = -MAX_ANGLE_R;

    int16_t duty = (int16_t)(PWM_STEER_MIDDLE_R + (angle / MAX_ANGLE_R) * (PWM_STEER_MAX_R - PWM_STEER_MIDDLE_R));

    Steer_set_duty(duty);
}

// 改为使用 PID 控制
void Steer_angle(float angle)
{
    float error;
    float current_yaw = yaw;
    
    // 计算角度差
    error = current_yaw - angle;
    
    // 将角度差规范化到 -180 到 180 度
    while(error > 180.0f) error -= 360.0f;
    while(error < -180.0f) error += 360.0f;

    // 设置舵机角度
    Steer_set_angle(-error);
}
