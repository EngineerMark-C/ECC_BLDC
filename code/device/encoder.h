#ifndef _encoder_h_
#define _encoder_h_

#define PIT0                            (CCU60_CH0 )                            // 使用的周期中断编号

#define ENCODER_DIR                     (TIM5_ENCODER)                          // 带方向编码器对应使用的编码器接口
#define ENCODER_DIR_PULSE               (TIM5_ENCODER_CH1_P10_3)                // PULSE 对应的引脚
#define ENCODER_DIR_DIR                 (TIM5_ENCODER_CH2_P10_1)                // DIR 对应的引脚

#define WHEEL_PERIMETER                 22.61947f                               // 轮子周长，单位cm
#define GEAR_RATIO                      0.245455f                               // 齿轮传动比  27/110
#define ENCODER_SAMPLE_TIME             0.01f                                   // 采样时间

extern int16_t encoder_data_dir;
extern float speed;

void Encoder_Init(void);
void Encoder_get_speed(void);

#endif
