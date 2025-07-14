#include "init.h"

TestFlag test_flag = NO_TEST;

Test3Data Test3Element[4];
//  Point_Index  |  Through_Speed  |
// -------------|----------------- |
//       0      |      3.0f        |        坡道
//       1      |      3.0f        |        草地
//       2      |      3.0f        |        颠簸
//       3      |      3.0f        |        狭路

// float Max_Speed_For_Test1(float max_speed)
// {
//     float angle_diff;
//     float speed_factor = 1.0f;

//     // 计算角度差值（考虑0-360度的循环特性）
//     angle_diff = fabs(target_angle - yaw);
//     if (angle_diff > 30.0f)
//     {
//         speed_factor = 0.5f;
//     }
//     else if (angle_diff > 20.0f && angle_diff <= 30.0f)
//     {
//         speed_factor = 0.7f;
//     }
//     else if (angle_diff > 10.0f && angle_diff <= 20.0f)
//     {
//         speed_factor = 0.9f;
//     }
//     else if (angle_diff > 0.0f && angle_diff <= 10.0f)
//     {
//         speed_factor = 1.0f;
//     }
//     return max_speed * speed_factor;
// }

void Speed_Management_For_Test3(float distance ,uint8_t i)
{
    float current_target_speed;

    // 检查是否为特殊点位
    int8_t special_index = -1;
    for (uint8_t j = 0; j < 4; j++)
    {
        if (i == Test3Element[j].Point_Index)
        {
            special_index = j;
            break;
        }
    }
    
    if (special_index >= 0)
    {
        current_target_speed = Test3Element[special_index].Through_Speed;
    }
    else
    {
        // 检查是否为特殊点位的前一个点
        float minimum_speed = MIN_SPEED;  // 默认最小速度

        for (uint8_t j = 0; j < 4; j++)
        {
            if (i == Test3Element[j].Point_Index - 1)
            {
                minimum_speed = Test3Element[j].Through_Speed;
                break;
            }
        }
        
        if(distance > BRAKING_DISTANCE)
        {
            current_target_speed = MAX_SPEED;
        } 
        else {
            current_target_speed = minimum_speed + (MAX_SPEED - minimum_speed) * (distance / BRAKING_DISTANCE) * 0.7f;
            current_target_speed = fmaxf(current_target_speed, minimum_speed);
        }
    }
    target_speed = current_target_speed;
}