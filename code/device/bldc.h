#ifndef _bldc_h
#define _bldc_h

#define DIROUT_PIN                      (P02_5)                                 // 方向输出引脚
#define PWMOUT_PIN                      (ATOM0_CH4_P02_4)                       // PWM输出引脚
#define DUTY_MAX                        (4000)                                  // 占空比限幅

void BLDC_Init(void);
void BLDC_Set_Duty(int16_t BLDC_duty);

#endif