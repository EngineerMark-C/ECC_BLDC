#ifndef _pid_h_
#define _pid_h_

struct PID {
    // PID 参数
    float kp;                        // 比例系数
    float ki;                        // 积分系数
    float kd;                        // 微分系数
    
    // 控制目标与状态
    float target;                    // 目标值
    float current;                   // 当前值（反馈值）
    
    // 误差相关
    float error;                     // 当前误差
    float error_last;                // 上次误差
    float error_prev;                // 上上次误差（用于增量式PID）
    
    // 积分相关
    float integral;                  // 积分项
    float integral_max;              // 积分上限
    float integral_min;              // 积分下限
    bool integral_separation_enable; // 积分分离使能
    float integral_separation_threshold; // 积分分离阈值
    
    // 微分相关
    float derivative;                // 微分项
    float filter_coefficient;        // 微分滤波系数(0-1)，值越小滤波越强
    
    // 输出相关
    float output;                    // PID输出
    float output_min;                // 输出下限
    float output_max;                // 输出上限
    
    // 控制周期
    float dt;                        // 采样时间/控制周期(s)
    
    // 控制模式
    uint8_t mode;                    // 控制模式: 0-位置式PID, 1-增量式PID
    bool enabled;                    // 使能状态
};

// PID参数表结构
typedef struct {
    float max_speed;    // 最大速度阈值
    float kp;           // 比例系数
    float ki;           // 积分系数
    float kd;           // 微分系数
} PID_Params_t;

extern int16_t output_speed;
extern struct PID pid_speed;
extern struct PID pid_steer;
extern const PID_Params_t pid_params_table[];

// PID 初始化与计算函数
void PID_init(struct PID *pid, float kp, float ki, float kd, uint8_t mode, float integral_limit);
void PID_reset(struct PID *pid);
void PID_enable(struct PID *pid, bool enable);
void PID_calc(struct PID *pid, float current);
void Update_PID_Params(float target_speed);

// 应用控制函数
void Motor_PID_Control(float target);
void Steer_PID_Control(float target);

#endif
