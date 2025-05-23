#ifndef _switch_h_
#define _switch_h_

#include "init.h"

// 拨码开关引脚定义
#define SWITCH_PIN_1                    (P22_3)                                 // 拨码开关1
#define SWITCH_PIN_2                    (P22_2)                                 // 拨码开关2
#define SWITCH_PIN_3                    (P22_1)                                 // 拨码开关3
#define SWITCH_PIN_4                    (P22_0)                                 // 拨码开关4

// 拨码开关状态
#define SWITCH_LEFT                     (0)                                     // 拨码开关拨向左侧
#define SWITCH_RIGHT                    (1)                                     // 拨码开关拨向右侧

// 获取单个拨码开关状态
#define SWITCH_1_STATUS                 (gpio_get_level(SWITCH_PIN_1))
#define SWITCH_2_STATUS                 (gpio_get_level(SWITCH_PIN_2))
#define SWITCH_3_STATUS                 (gpio_get_level(SWITCH_PIN_3))
#define SWITCH_4_STATUS                 (gpio_get_level(SWITCH_PIN_4))

// 判断开关位置
#define SWITCH_1_IS_LEFT                (SWITCH_1_STATUS == SWITCH_LEFT)
#define SWITCH_1_IS_RIGHT               (SWITCH_1_STATUS == SWITCH_RIGHT)
#define SWITCH_2_IS_LEFT                (SWITCH_2_STATUS == SWITCH_LEFT)
#define SWITCH_2_IS_RIGHT               (SWITCH_2_STATUS == SWITCH_RIGHT)
#define SWITCH_3_IS_LEFT                (SWITCH_3_STATUS == SWITCH_LEFT)
#define SWITCH_3_IS_RIGHT               (SWITCH_3_STATUS == SWITCH_RIGHT)
#define SWITCH_4_IS_LEFT                (SWITCH_4_STATUS == SWITCH_LEFT)
#define SWITCH_4_IS_RIGHT               (SWITCH_4_STATUS == SWITCH_RIGHT)

// 获取所有开关组合状态
#define SWITCH_ALL_STATUS               ((SWITCH_4_STATUS) | (SWITCH_3_STATUS << 1) | \
                                        (SWITCH_2_STATUS << 2) | (SWITCH_1_STATUS << 3))

// 拨码开关初始化
void Switch_Init(void);

#endif