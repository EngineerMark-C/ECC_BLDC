#ifndef _pid_h_
#define _pid_h_

struct PID {
    float kp;
    float ki;
    float kd;
    float target;
    float current;
    float error;
    float error_last;
    float integral;
    float derivative;
    float output;
};

extern int16_t output_speed;
extern struct PID pid_speed;
extern struct PID pid_steer;

void PID_init(struct PID *pid, float kp, float ki, float kd);
void Motor_PID_Control(float target);
void Steer_PID_Control(float target);

#endif
