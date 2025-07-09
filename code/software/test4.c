#include "init.h"

//************************************全局变量定义****************************************//
const char temp_string[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}";


// 命令缓冲区
uint8_t command_buffer[MAX_COMMAND_BUFFER];
uint8_t command_buffer_index = 0;

// 定时器和状态标志
uint8_t timer_10ms_flag = 0;
uint8_t command_complete_flag = 1;
uint8_t voice_recognition_flag = 0;

// static float start_x = 0.0f;          // 记录开始时的X坐标
// static float start_yaw = 0.0f;        // 记录开始时的偏航角
// static float total_rotated = 0.0f;    // 记录总的旋转角度
// static float last_yaw = 0.0f;         // 上次的偏航角
// static uint8_t rotation_state = 0;    // 旋转状态：0=未开始，1=旋转中，2=完成

//************************************延时函数****************************************//
/**
 * @brief 基于定时器标志的延时函数
 * @param ms 延时时间（毫秒）
 */
void delay_ms(uint16_t ms)
{
    uint16_t count = (ms + 9) / 10;
    while (count > 0)
    {
        if (timer_10ms_flag)
        {
            timer_10ms_flag = 0;
            count--;
        }
    }
}

//************************************语音命令定义****************************************//

typedef enum
{
    CMD_NONE = 0,
    CMD_OPEN_HAZARD,                    // 打开双闪灯
    CMD_OPEN_LEFT_TURN,                 // 打开左转灯
    CMD_OPEN_RIGHT_TURN,                // 打开右转灯
    CMD_OPEN_LOW_BEAM,                  // 打开近光灯
    CMD_OPEN_HIGH_BEAM,                 // 打开远光灯
    CMD_OPEN_FOG_LIGHT,                 // 打开雾灯
    CMD_MOVE_FORWARD_10M,               // 向前直行10米
    CMD_MOVE_BACKWARD_10M,              // 后退直行10米
    CMD_SNAKE_FORWARD_10M,              // 蛇形前进10米
    CMD_SNAKE_BACKWARD_10M,             // 蛇形后退10米
    CMD_TURN_COUNTERCLOCK,              // 逆时针转一圈
    CMD_TURN_CLOCKWISE,                 // 顺时针转一圈
    CMD_PARKING_1,                      // 停进停车区1
    CMD_PARKING_2,                      // 停进停车区2
    CMD_PARKING_3,                      // 停进停车区3
    CMD_MAX
} Voice_Command_Type;

// 函数指针类型定义
typedef void (*Command_Handler)(void);

//************************************命令映射表****************************************//
/**
 * @brief 命令映射结构体
 */
typedef struct
{
    const char *command_text;           // 命令文本
    Voice_Command_Type command_id;      // 命令ID
    Command_Handler handler;            // 处理函数
} Command_Map_Type;


const Command_Map_Type command_map[] = {
    {"打开双闪灯", CMD_OPEN_HAZARD, Handle_Open_Hazard},
    {"打开左转灯", CMD_OPEN_LEFT_TURN, Handle_Open_Left_Turn},
    {"打开右转灯", CMD_OPEN_RIGHT_TURN, Handle_Open_Right_Turn},
    {"打开近光灯", CMD_OPEN_LOW_BEAM, Handle_Open_Low_Beam},
    {"打开远光灯", CMD_OPEN_HIGH_BEAM, Handle_Open_High_Beam},
    {"打开雾灯", CMD_OPEN_FOG_LIGHT, Handle_Open_Fog_Light},
    {"向前直行十米", CMD_MOVE_FORWARD_10M, Handle_Move_Forward_10M},
    {"向前直行10米", CMD_MOVE_FORWARD_10M, Handle_Move_Forward_10M},
    {"后退直行十米", CMD_MOVE_BACKWARD_10M, Handle_Move_Backward_10M},
    {"后退直行10米", CMD_MOVE_BACKWARD_10M, Handle_Move_Backward_10M},
    {"蛇形前进十米", CMD_SNAKE_FORWARD_10M, Handle_Snake_Forward_10M},
    {"蛇形前进10米", CMD_SNAKE_FORWARD_10M, Handle_Snake_Forward_10M},
    {"蛇形后退十米", CMD_SNAKE_BACKWARD_10M, Handle_Snake_Backward_10M},
    {"蛇形后退10米", CMD_SNAKE_BACKWARD_10M, Handle_Snake_Backward_10M},
    {"逆时针转一圈", CMD_TURN_COUNTERCLOCK, Handle_Turn_Counterclock},
    {"逆时针转1圈", CMD_TURN_COUNTERCLOCK, Handle_Turn_Counterclock},
    {"顺时针转一圈", CMD_TURN_CLOCKWISE, Handle_Turn_Clockwise},
    {"顺时针转1圈", CMD_TURN_CLOCKWISE, Handle_Turn_Clockwise},
    {"停进停车区一", CMD_PARKING_1, Handle_Parking_1},
    {"停进停车区1", CMD_PARKING_1, Handle_Parking_1},
    {"停进停车区二", CMD_PARKING_2, Handle_Parking_2},
    {"停进停车区2", CMD_PARKING_2, Handle_Parking_2},
    {"停进停车区三", CMD_PARKING_3, Handle_Parking_3},
    {"停进停车区3", CMD_PARKING_3, Handle_Parking_3},
    {NULL, CMD_NONE, NULL}
};

//************************************语音识别处理****************************************//
/**
 * @brief 语音命令识别函数
 * @param voice_input 语音输入字符串
 */
void Recognize_Voice_Command(const char *voice_input)
{
    char cleaned_input[MAX_WORD_LENGTH];
    
    // 清理输入字符串
    snprintf(cleaned_input, sizeof(cleaned_input), "%s", voice_input);
    
    // 遍历命令映射表进行匹配
    uint8_t i = 0;
    while (command_map[i].command_text != NULL)
    {
        if (strstr(cleaned_input, command_map[i].command_text) != NULL)
        {
            if (command_buffer_index < MAX_COMMAND_BUFFER)
            {
                command_buffer[command_buffer_index++] = i + 1;
            }
        }
        i++;
    }
    
    // 打印匹配结果
    if (command_buffer_index == 0)
    {
        printf("未识别的命令: %s\n", voice_input);
    }
    else
    {
        for (uint8_t j = 0; j < command_buffer_index; j++)
        {
            printf("已匹配命令索引: %d\n", command_buffer[j]);
        }
    }
}

/**
 * @brief 执行命令缓冲区中的命令
 */
void Execute_Command_Buffer(void)
{
    uint8_t command_index = 0;
    
    while (command_index < 7)
    {
        if (command_complete_flag == 1 && voice_recognition_flag == 1)
        {
            command_complete_flag = 0;
            uint8_t voice_cmd_id = command_buffer[command_index];
            
            // 检查命令ID有效性
            if (voice_cmd_id > 0 && voice_cmd_id <= (sizeof(command_map) / sizeof(command_map[0]) - 1))
            {
                Command_Handler handler = command_map[voice_cmd_id - 1].handler;
                if (handler != NULL)
                {
                    handler();  // 执行命令处理函数
                }
            }
            command_index++;
            printf("完成第%d条命令\n", command_index);
        }
        else
        {
            break;  // 如果条件不满足，跳出循环
        }
    }
}

//************************************灯光控制命令实现****************************************//
/**
 * @brief 打开双闪灯
 */
void Handle_Open_Hazard(void)
{
    printf("执行命令: 打开双闪灯\n");
    dot_matrix_screen_show_string("123");
    dot_matrix_screen_set_brightness(2500);
    command_complete_flag = 1;
    delay_ms(3000);
}

/**
 * @brief 打开左转灯
 */
void Handle_Open_Left_Turn(void)
{
    printf("执行命令: 打开左转灯\n");
    dot_matrix_screen_show_string("789");
    dot_matrix_screen_set_brightness(2500);
    command_complete_flag = 1;
    delay_ms(3000);
}

/**
 * @brief 打开右转灯
 */
void Handle_Open_Right_Turn(void)
{
    printf("执行命令: 打开右转灯\n");
    dot_matrix_screen_show_string("456");
    dot_matrix_screen_set_brightness(2500);
    command_complete_flag = 1;
    delay_ms(3000);
}

/**
 * @brief 打开近光灯
 */
void Handle_Open_Low_Beam(void)
{
    printf("执行命令: 打开近光灯\n");
    dot_matrix_screen_show_string(":;<");
    dot_matrix_screen_set_brightness(2500);
    command_complete_flag = 1;
    delay_ms(3000);
}

/**
 * @brief 打开远光灯
 */
void Handle_Open_High_Beam(void)
{
    printf("执行命令: 打开远光灯\n");
    dot_matrix_screen_show_string("=>?");
    dot_matrix_screen_set_brightness(2500);
    command_complete_flag = 1;
    delay_ms(3000);
}

/**
 * @brief 打开雾灯
 */
void Handle_Open_Fog_Light(void)
{
    printf("执行命令: 打开雾灯\n");
    dot_matrix_screen_show_string("@AB");
    dot_matrix_screen_set_brightness(2500);
    command_complete_flag = 1;
    delay_ms(3000);
}

//************************************运动控制命令实现****************************************//

/**
 * @brief 向前直行10米
 */
void Handle_Move_Forward_10M(void)
{
    target_speed = 2.0f;  // 设置目标速度为2.0 m/s
    delay_ms(5000);
    target_speed = 0.0f;  // 停止运动
    command_complete_flag = 1;
    delay_ms(1000);
}

/**
 * @brief 后退直行10米
 */
void Handle_Move_Backward_10M(void)
{
    target_speed = -2.0f;
    delay_ms(5000);
    target_speed = 0.0f;
    command_complete_flag = 1;
    delay_ms(1000);
}

/**
 * @brief 蛇形前进10米
 */
void Handle_Snake_Forward_10M(void)
{
    target_speed = 2.0f;

    target_angle = 30.0f;
    delay_ms(1000);
    target_angle = -30.0f;
    delay_ms(1000);
    target_angle = 30.0f;
    delay_ms(1000);
    target_angle = -30.0f;
    delay_ms(1000);
    target_angle = 30.0f;
    delay_ms(1000);

    target_speed = 0.0f;
    command_complete_flag = 1;
    delay_ms(1000);
}

/**
 * @brief 蛇形后退10米
 */
void Handle_Snake_Backward_10M(void)
{
    target_speed = -2.0f;

    target_angle = 30.0f;
    delay_ms(1000);
    target_angle = -30.0f;
    delay_ms(1000);
    target_angle = 30.0f;
    delay_ms(1000);
    target_angle = -30.0f;
    delay_ms(1000);
    target_angle = 30.0f;
    delay_ms(1000);

    target_speed = 0.0f;
    command_complete_flag = 1;
    delay_ms(1000);
}

/**
 * @brief 逆时针转一圈
 */
void Handle_Turn_Counterclock(void)
{
    printf("执行命令: 逆时针转一圈\n");
    target_speed = 2.0f;

    target_angle = 270.0f;
    delay_ms(1000);
    target_angle = 180.0f;
    delay_ms(1000);
    target_angle = 90.0f;
    delay_ms(1000);
    target_angle = 0.0f;
    delay_ms(1000);

    target_speed = 0.0f;
    command_complete_flag = 1;

    delay_ms(1000);
}

/**
 * @brief 顺时针转一圈
 */
void Handle_Turn_Clockwise(void)
{
    printf("执行命令: 顺时针转一圈\n");
    target_speed = 2.0f;

    target_angle = 90.0f;
    delay_ms(1000);
    target_angle = 180.0f;
    delay_ms(1000);
    target_angle = 270.0f;
    delay_ms(1000);
    target_angle = 360.0f;
    delay_ms(1000);

    target_speed = 0.0f;
    command_complete_flag = 1;

    delay_ms(1000);
}

//************************************停车控制命令实现****************************************//
/**
 * @brief 停进停车区1
 */
void Handle_Parking_1(void)
{
    printf("执行命令: 停进停车区1\n");
    // TODO: 添加停进停车区1的具体实现
    command_complete_flag = 1;
    delay_ms(3000);
}

/**
 * @brief 停进停车区2
 */
void Handle_Parking_2(void)
{
    printf("执行命令: 停进停车区2\n");
    // TODO: 添加停进停车区2的具体实现
    command_complete_flag = 1;
    delay_ms(3000);
}

/**
 * @brief 停进停车区3
 */
void Handle_Parking_3(void)
{
    printf("执行命令: 停进停车区3\n");
    // TODO: 添加停进停车区3的具体实现
    command_complete_flag = 1;
    delay_ms(3000);
}

//************************************系统控制函数****************************************//
/**
 * @brief 完整的命令执行流程
 */
void Execute_Complete_Command(void)
{
    if (voice_recognition_flag == 1)
    {
        // 停止音频接收定时器
        pit_disable(ASR_PIT);
        printf("音频接收定时器已关闭。\r\n");

        // 断开WiFi连接
        wifi_uart_disconnected_wifi();
        printf("WiFi连接已断开。\r\n");
        delay_ms(100);

        // 初始化点阵屏
        printf("按钮20.9按下，初始化点阵屏...\r\n");
        dot_matrix_screen_init();
        printf("点阵屏初始化完成，开始执行命令序列\r\n");
        
        // 执行命令缓冲区中的所有命令
        Execute_Command_Buffer();
        
        // 重置语音识别标志位
        voice_recognition_flag = 0;
        
        // 清空命令缓冲区
        command_buffer_index = 0;
        
        printf("所有命令执行完成\r\n");
    }
}
