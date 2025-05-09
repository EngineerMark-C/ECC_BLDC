#include "init.h"

int16_t BLDC_duty = 0;

void BLDC_Init(void)
{
    // 初始化 PWM 输出引脚
    // 初始化 PWM 通道 频率 20KHz 占空比 0
    pwm_init(PWMOUT_PIN, 20000, 0);
    // 初始化方向输出引脚
    gpio_init(DIROUT_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}

void BLDC_Set_Duty(int16_t duty)
{
    // 设置方向引脚
    if (duty >= 0)
    {
        gpio_set_level(DIROUT_PIN, GPIO_HIGH); // 正转
    }
    else
    {
        gpio_set_level(DIROUT_PIN, GPIO_LOW); // 反转
        duty = -duty; // 取相反数
    }

    if (duty > DUTY_MAX) duty = DUTY_MAX; // 限幅

    BLDC_duty = duty;
    pwm_set_duty(PWMOUT_PIN, BLDC_duty);
}
