#include "init.h"

struct PID pid_speed;
struct PID pid_steer;
int16_t output_speed;

// PID参数表
const PID_Params_t speed_pid_params_table[] = 
{
    {5.5f,  550.0f, 2800.0f, 0.0f},    // 低速参数
    {12.0f, 520.0f, 3200.0f, 0.0f},    // 中速参数
    {16.0f, 560.0f, 4300.0f, 0.0f},    // 高速参数
};

const PID_Params_t steer_pid_params_table[] = 
{
    {5.0f, 0.7f, 0.0f, 0.0f},           // 舵机PID参数
    {10.0f, 0.3f, 0.0f, 0.0f},          // 舵机PID参数
    {15.0f, 0.3f, 0.0f, 0.0f},          // 舵机PID参数
};

void PID_init(struct PID *pid, float kp, float ki, float kd, uint8_t mode, float integral_limit)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    // 初始化状态量
    pid->target = 0.0f;
    pid->current = 0.0f;

    // 初始化误差量
    pid->error = 0.0f;
    pid->error_last = 0.0f;
    pid->error_prev = 0.0f;

    // 初始化积分相关
    pid->integral = 0.0f;
    pid->integral_max = integral_limit;
    pid->integral_min = -integral_limit;
    pid->integral_separation_enable = false;
    pid->integral_separation_threshold = 2000.0f;

    // 初始化微分相关
    pid->derivative = 0.0f;
    pid->filter_coefficient = 0.8f; // 默认滤波系数

    // 初始化输出相关
    pid->output = 0.0f;
    pid->output_min = -DUTY_MAX;
    pid->output_max = DUTY_MAX;

    // 初始化控制周期
    pid->dt = 0.01f; // 默认时间间隔为 10ms

    // 初始化控制模式
    pid->mode = mode; // 默认位置式PID
    pid->enabled = true;
}

void PID_set_params(struct PID *pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

void Update_Speed_PID_Params(float target_speed)
{
    // 根据目标速度选择 PID 参数
    if (target_speed <= speed_pid_params_table[0].max_speed)
    {
        PID_set_params(&pid_speed, speed_pid_params_table[0].kp, speed_pid_params_table[0].ki, speed_pid_params_table[0].kd);
    }
    else if (target_speed > speed_pid_params_table[0].max_speed && target_speed <= speed_pid_params_table[1].max_speed)
    {
        PID_set_params(&pid_speed, speed_pid_params_table[1].kp, speed_pid_params_table[1].ki, speed_pid_params_table[1].kd);
    }
    else
    {
        PID_set_params(&pid_speed, speed_pid_params_table[2].kp, speed_pid_params_table[2].ki, speed_pid_params_table[2].kd);
    }
}

void Update_Steer_PID_Params(float now_speed)
{
    // 根据当前速度选择舵机 PID 参数
    if (now_speed <= steer_pid_params_table[0].max_speed)
    {
        PID_set_params(&pid_steer, steer_pid_params_table[0].kp, steer_pid_params_table[0].ki, steer_pid_params_table[0].kd);
    }
    else if (now_speed > steer_pid_params_table[0].max_speed && now_speed <= steer_pid_params_table[1].max_speed)
    {
        PID_set_params(&pid_steer, steer_pid_params_table[1].kp, steer_pid_params_table[1].ki, steer_pid_params_table[1].kd);
    }
    else
    {
        PID_set_params(&pid_steer, steer_pid_params_table[2].kp, steer_pid_params_table[2].ki, steer_pid_params_table[2].kd);
    }
}

// 重置 PID 控制器状态
void PID_reset(struct PID *pid)
{
    pid->error = 0.0f;
    pid->error_last = 0.0f;
    pid->error_prev = 0.0f;
    pid->integral = 0.0f;
    pid->derivative = 0.0f;
    pid->output = 0.0f;
}

void PID_enable(struct PID *pid, bool enable)
{
    pid->enabled = enable; // 启用或禁用 PID 控制器
}

// 统一的 PID 计算函数
void PID_calc(struct PID *pid, float current)
{
    if (!pid->enabled)
    {
        pid->output = 0.0f; // 如果控制器未启用，输出为0
        PID_reset(pid); // 重置状态
        return; // 如果控制器未启用，直接返回
    }

    // 更新当前值
    pid->current = current;

    // 计算误差
    pid->error = pid->target - pid->current;

    // 选择使用位置式PID还是增量式PID
    if (pid->mode == 0)
    {
        // 位置式PID

        // 积分项计算（带积分分离）
        if (!pid->integral_separation_enable || fabsf(pid->error) < pid->integral_separation_threshold)
        {
            pid->integral += pid->error * pid->dt;
        }

        // 积分限幅
        if (pid->integral > pid->integral_max)
        {
            pid->integral = pid->integral_max;
        }
        else if (pid->integral < pid->integral_min)
        {
            pid->integral = pid->integral_min;
        }

        // 微分项计算（带低通滤波）
        float derivative_raw = (pid->error - pid->error_last) / pid->dt;
        pid->derivative = pid->filter_coefficient * derivative_raw +
                          (1.0f - pid->filter_coefficient) * pid->derivative;

        // 计算PID输出
        pid->output = pid->kp * pid->error +
                      pid->ki * pid->integral +
                      pid->kd * pid->derivative;
    }
    else
    {
        // 增量式PID
        float dp = pid->kp * (pid->error - pid->error_last);
        float di = pid->ki * pid->error * pid->dt;
        float dd = pid->kd * (pid->error - 2 * pid->error_last + pid->error_prev) / pid->dt;

        // 计算输出增量
        float delta_output = dp + di + dd;

        // 更新输出
        pid->output += delta_output;
    }

    // 输出限幅
    if (pid->output > pid->output_max)
    {
        pid->output = pid->output_max;
    }
    else if (pid->output < pid->output_min)
    {
        pid->output = pid->output_min;
    }

    // 保存历史误差
    pid->error_prev = pid->error_last;
    pid->error_last = pid->error;
}

// 电机 PID 控制
void Motor_PID_Control(float target)
{
    pid_speed.target = target;                // 设置目标值
    PID_calc(&pid_speed, speed);              // 使用当前速度作为反馈值
    output_speed = (int16_t)pid_speed.output;

    // 输出限幅
    if (output_speed > DUTY_MAX)
        output_speed = DUTY_MAX;
    if (output_speed < -DUTY_MAX)
        output_speed = -DUTY_MAX;

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
