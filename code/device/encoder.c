#include "init.h" 

int16_t encoder_data_dir = 0;
float speed = 0.0f;

void Encoder_Init (void)
{
    encoder_dir_init(ENCODER_DIR, ENCODER_DIR_PULSE, ENCODER_DIR_DIR);
    pit_ms_init(PIT0, 10);                                                      // 初始化 PIT0 为周期中断 10ms
}

void Encoder_get_speed(void)
{
    // 霍尔编码器速度计算
    // encoder_data_dir 是电机每分钟的转速 (RPM)，需要将其转换为每秒的转速 (RPS)
    // 速度 = (电机转速 / 60) * 齿轮比 * 轮子周长
    speed = (float)encoder_data_dir /60 * GEAR_RATIO * WHEEL_PERIMETER;
}
