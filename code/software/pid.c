#include "init.h"

struct PID pid_speed;
struct PID pid_steer;
int16_t output_speed;

void PID_init(struct PID *pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->target = 0;
    pid->current = 0;
    pid->error = 0;
    pid->error_last = 0;
    pid->integral = 0;
    pid->derivative = 0;
    pid->output = 0;
}

//位置式 PID
void PID_Speed_Calc(struct PID *pid, float current)
{
    pid->current = current;
    pid->error = pid->target - pid->current;                    // 计算当前误差
    
    pid->integral += pid->error;                                // 累积误差积分
    pid->derivative = pid->error - pid->error_last;             // 计算误差微分
    
    // 积分限幅，防止积分饱和
    if(pid->integral > 10000) pid->integral = 10000;
    if(pid->integral < -10000) pid->integral = -10000;
    
    // 计算PID输出
    pid->output = pid->kp * pid->error + 
                 pid->ki * pid->integral + 
                 pid->kd * pid->derivative;
    
    // 输出限幅
    if(pid->output > DUTY_MAX) pid->output = DUTY_MAX;
    if(pid->output < -DUTY_MAX) pid->output = -DUTY_MAX;
    
    pid->error_last = pid->error;                               // 保存上次误差
}

// 增量式 PID
// void PID_Speed_Calc(struct PID *pid, float current)
// {
//     pid->current = current;
//     pid->error = pid->target - pid->current;

//     pid->output += pid->kp * (pid->error - pid->error_last) + 
//                   pid->ki * pid->error + 
//                   pid->kd * (pid->error - 2 * pid->error_last + pid->derivative);
    
//     // 输出限幅
//     if(pid->output > DUTY_MAX) pid->output = DUTY_MAX;
//     if(pid->output < -DUTY_MAX) pid->output = -DUTY_MAX;

//     pid->derivative = pid->error_last;
//     pid->error_last = pid->error;
// }

// 电机 PID 控制
void Motor_PID_Control(float target)
{
    pid_speed.target = target;                                      // 设置目标值
    PID_Speed_Calc(&pid_speed, speed);                              // 使用当前速度作为反馈值
    output_speed += (int16_t)pid_speed.output;                      // 将PID输出转换为占空比
    
    // 输出限幅
    if(output_speed > DUTY_MAX) output_speed = DUTY_MAX;
    if(output_speed < -DUTY_MAX) output_speed = -DUTY_MAX;

    // 电机控制
    BLDC_Set_Duty(output_speed);
}

// 角度 PID
void PID_Angle_Calc(struct PID *pid, float current)
{
    pid->current = current;
    
    // 计算误差
    pid->error = pid->current - pid->target;
    
    // 将误差规范化到 -180 到 180 度范围
    while(pid->error > 180.0f) pid->error -= 360.0f;
    while(pid->error < -180.0f) pid->error += 360.0f;
    
    // 计算积分项
    pid->integral += pid->error;
    
    // 积分限幅,防止积分饱和(根据实际调试修改限幅值)
    if(pid->integral > 100.0f) pid->integral = 100.0f;
    if(pid->integral < -100.0f) pid->integral = -100.0f;
    
    // 计算微分项
    pid->derivative = pid->error - pid->error_last;
    
    // 计算PID输出
    pid->output = (pid->kp * pid->error + 
                 pid->ki * pid->integral + 
                 pid->kd * pid->derivative);
    
    // 输出限幅,防止舵机打角过大(根据实际舵机限位调整)
    if(pid->output > MAX_ANGLE_R) pid->output = MAX_ANGLE_R;
    if(pid->output < -MAX_ANGLE_R) pid->output = -MAX_ANGLE_R;

    // 保存上次误差
    pid->error_last = pid->error;
}

// 舵机 PID 控制
void Steer_PID_Control(float target_angle)
{
    pid_steer.target = target_angle;                                // 设置目标角度
    PID_Angle_Calc(&pid_steer, yaw);                                // 使用当前偏航角作为反馈
    Steer_set_angle(pid_steer.output);                              // 设置舵机角度
}
