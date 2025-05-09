#include "init.h"

// 初始化UART接收机
void UartReceiver_Init(void)
{
    uart_receiver_init();
}

// 遥控数据与速度映射
void Speed_map(void)
{
    // uart_receiver.channel[1] 的值在 224 到 1824 之间
    int16_t channel_value = uart_receiver.channel[1];

    target_speed = (20.0 / (RECEIVER_CHANGE)) * (channel_value - RECEIVER_MIDDLE);

    // printf("channel_value: %d\r\n", channel_value);
    // printf("target_speed: %f\r\n", target_speed);
}

void Angle_map(void)
{
    // 假设 uart_receiver.channel[0] 的值在 224 到 1824 之间
    int16_t channel_value = uart_receiver.channel[0];

    target_angle = (MAX_ANGLE_R / (RECEIVER_CHANGE)) * (channel_value - RECEIVER_MIDDLE);

    // printf("channel_value: %d\r\n", channel_value);
    // printf("target_angle: %f\r\n", target_angle);
}

// 遥控逻辑
void Remote_Control(void)
{
    if(1 == uart_receiver.finsh_flag)                                           // 帧完成标志判断
    {
        if(1 == uart_receiver.state)                                            // 遥控器失控状态判断
        {
            Speed_map();                                                        // 遥控数据与速度映射
            Angle_map();                                                        // 遥控数据与舵机角度映射
        }
        uart_receiver.finsh_flag = 0;                                           // 帧完成标志复位
    }

}
