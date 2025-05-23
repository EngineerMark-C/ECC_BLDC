#include "init.h"

void Switch_Init(void)
{
    // 将所有拨码开关引脚初始化为上拉输入模式
    // 上拉输入模式下，悬空时为高电平，接地时为低电平
    gpio_init(SWITCH_PIN_1, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(SWITCH_PIN_2, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(SWITCH_PIN_3, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(SWITCH_PIN_4, GPI, GPIO_HIGH, GPI_PULL_UP);
}