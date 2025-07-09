#include "init.h"

TestFlag test_flag = NO_TEST;

void Speed_Management_For_Test3(float distance)
{
    float current_target_speed;
    float angle_diff;
    float speed_factor = 1.0f;
    
    // 计算角度差值（考虑0-360度的循环特性）
    angle_diff = fabs(next_target_angle - yaw);
    if (angle_diff > 180.0f) {
        angle_diff = 360.0f - angle_diff;  // 取较小的角度差
    }
    
    // 根据角度差值确定速度因子
    if (angle_diff <= 10.0f) {
        // 10度以内：无需减速
        speed_factor = 1.0f;
    }
    else if (angle_diff <= 30.0f) {
        // 10-30度：轻微减速
        // 线性插值：10度时factor=1.0，30度时factor=0.8
        speed_factor = 1.0f - (angle_diff - 10.0f) / 20.0f * 0.2f;
    }
    else if (angle_diff <= 60.0f) {
        // 30-60度：中等减速
        // 线性插值：30度时factor=0.8，60度时factor=0.5
        speed_factor = 0.8f - (angle_diff - 30.0f) / 30.0f * 0.3f;
    }
    else {
        // 60度以上：大幅减速
        // 线性插值：60度时factor=0.5，90度及以上时factor=0.3
        float max_angle = 90.0f;
        if (angle_diff > max_angle) angle_diff = max_angle;
        speed_factor = 0.5f - (angle_diff - 60.0f) / 30.0f * 0.2f;
    }
    
    // 基础速度计算（原有的距离控制逻辑）
    if(distance > BRAKING_DISTANCE) {
        current_target_speed = MAX_SPEED;
    } 
    else 
    {
        // 线性减速区间
        current_target_speed = MIN_SPEED + (MAX_SPEED - MIN_SPEED) * (distance / BRAKING_DISTANCE) * 0.6f;
        // 确保不低于最小速度
        current_target_speed = fmaxf(current_target_speed, MIN_SPEED);
    }
    
    // 应用角度控制的速度因子
    current_target_speed *= speed_factor;
    
    // 确保最终速度不低于最小速度
    current_target_speed = fmaxf(current_target_speed, MIN_SPEED);
    
    // 更新目标速度
    target_speed = current_target_speed;
}