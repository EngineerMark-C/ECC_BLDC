#include "init.h"

const char temp_string[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}";
uint8_t arr[20];
uint8_t arr_index = 0;

// 定义定时器标志
uint8_t timer_10ms_flag = 0;

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

// 定义支持的指令列表
typedef enum
{
    CMD_NONE,
    CMD_OPEN_HAZARD,                                                            // 打开双闪灯
    CMD_CLOSE_HAZARD,                                                           // 关闭双闪灯
    CMD_OPEN_LEFT_TURN,                                                         // 打开左转灯
    CMD_CLOSE_LEFT_TURN,                                                        // 关闭左转灯
    CMD_OPEN_RIGHT_TURN,                                                        // 打开右转灯
    CMD_CLOSE_RIGHT_TURN,                                                       // 关闭右转灯
    CMD_OPEN_LOW_BEAM,                                                          // 打开近光灯
    CMD_CLOSE_LOW_BEAM,                                                         // 关闭近光灯
    CMD_OPEN_HIGH_BEAM,                                                         // 打开远光灯
    CMD_CLOSE_HIGH_BEAM,                                                        // 关闭远光灯
    CMD_OPEN_FOG_LIGHT,                                                         // 打开雾灯
    CMD_CLOSE_FOG_LIGHT,                                                        // 关闭雾灯
    CMD_MOVE_FORWARD_10M,                                                       // 向前直行10米
    CMD_MOVE_BACKWARD_10M,                                                      // 后退直行10米
    CMD_SNAKE_FORWARD_10M,                                                      // 蛇形前进10米
    CMD_SNAKE_BACKWARD_10M,                                                     // 蛇形后退10米
    CMD_TURN_COUNTERCLOCK,                                                      // 逆时针转一圈
    CMD_TURN_CLOCKWISE,                                                         // 顺时针转一圈
    CMD_PARKING_1,                                                              // 停进停车区1
    CMD_PARKING_2,                                                              // 停进停车区2
    CMD_PARKING_3,                                                              // 停进停车区3
} VoiceCommand;

// 函数指针类型定义
typedef void (*CommandHandler)(void);

// 命令处理函数声明
void handle_open_hazard(void);
void handle_close_hazard(void);
void handle_open_left_turn(void);
void handle_close_left_turn(void);
void handle_open_right_turn(void);
void handle_close_right_turn(void);
void handle_open_low_beam(void);
void handle_close_low_beam(void);
void handle_open_high_beam(void);
void handle_close_high_beam(void);
void handle_open_fog_light(void);
void handle_close_fog_light(void);
void handle_move_forward_10m(void);
void handle_move_backward_10m(void);
void handle_snake_forward_10m(void);
void handle_snake_backward_10m(void);
void handle_turn_counterclock(void);
void handle_turn_clockwise(void);
void handle_parking_1(void);
void handle_parking_2(void);
void handle_parking_3(void);

// 命令-处理函数映射表
const struct
{
    const char *command;
    VoiceCommand id;
    CommandHandler handler;
} command_map[] = {
    {"打开双闪灯", CMD_OPEN_HAZARD, handle_open_hazard},
    {"关闭双闪灯", CMD_CLOSE_HAZARD, handle_close_hazard},
    {"打开左转灯", CMD_OPEN_LEFT_TURN, handle_open_left_turn},
    {"关闭左转灯", CMD_CLOSE_LEFT_TURN, handle_close_left_turn},
    {"打开右转灯", CMD_OPEN_RIGHT_TURN, handle_open_right_turn},
    {"关闭右转灯", CMD_CLOSE_RIGHT_TURN, handle_close_right_turn},
    {"打开近光灯", CMD_OPEN_LOW_BEAM, handle_open_low_beam},
    {"关闭近光灯", CMD_CLOSE_LOW_BEAM, handle_close_low_beam},
    {"打开远光灯", CMD_OPEN_HIGH_BEAM, handle_open_high_beam},
    {"关闭远光灯", CMD_CLOSE_HIGH_BEAM, handle_close_high_beam},
    {"打开雾灯", CMD_OPEN_FOG_LIGHT, handle_open_fog_light},
    {"关闭雾灯", CMD_CLOSE_FOG_LIGHT, handle_close_fog_light},
    {"向前直行十米", CMD_MOVE_FORWARD_10M, handle_move_forward_10m},
    {"后退直行十米", CMD_MOVE_BACKWARD_10M, handle_move_backward_10m},
    {"蛇形前进十米", CMD_SNAKE_FORWARD_10M, handle_snake_forward_10m},
    {"蛇形后退十米", CMD_SNAKE_BACKWARD_10M, handle_snake_backward_10m},
    {"逆时针转一圈", CMD_TURN_COUNTERCLOCK, handle_turn_counterclock},
    {"顺时针转一圈", CMD_TURN_CLOCKWISE, handle_turn_clockwise},
    {"停进停车区一", CMD_PARKING_1, handle_parking_1},
    {"停进停车区1", CMD_PARKING_1, handle_parking_1},
    {"停进停车区二", CMD_PARKING_2, handle_parking_2},
    {"停进停车区2", CMD_PARKING_2, handle_parking_2},
    {"停进停车区三", CMD_PARKING_3, handle_parking_3},
    {"停进停车区3", CMD_PARKING_3, handle_parking_3},
    {NULL, CMD_NONE, NULL}};

// 全局变量，存储当前识别的命令
VoiceCommand current_command = CMD_NONE;

void recognize_command(const char *voice_input)
{
    char cleaned_input[MAX_WORD_LENGTH];
    // 去除可能的标点和空格
    snprintf(cleaned_input, sizeof(cleaned_input), "%s", voice_input);
    int8 i = 0;
    while (command_map[i].command != NULL)
    {
        if (strstr(cleaned_input, command_map[i].command) != NULL)
        {
            if (arr_index < 10)
            {
                arr[arr_index++] = i + 1;
            }
        }
        i++;
    }
    // 打印匹配结果
    if (arr_index == 0)
    {
        printf("未识别的命令: %s\n", voice_input);
    }
    else
    {
        for (int g = 0; g < arr_index; g++)
        {
            printf("已匹配命令索引: %d\n", arr[g]);
        }
    }
}
int8 completecommand_flag = 1;
int8 voice_flag = 0;

// kemu44.c 文件中的 complete_command 函数修改
void complete_command()
{
    int8 g = 0;
    while (g < 7)
    {
        if (completecommand_flag == 1 && voice_flag == 1)
        {
            completecommand_flag = 0;
            int i = arr[g];
            if (i > 0 && i <= sizeof(command_map) / sizeof(command_map[0]) - 1)
            {
                CommandHandler handler = command_map[i - 1].handler;
                if (handler != NULL)
                {
                    handler(); // 调用命令处理函数
                }
            }
            g++;
            printf("完成第%d条命令\n", g + 1);
        }
        else
        {
            break; // 如果条件不满足，跳出循环
        }
    }
}

void handle_open_hazard(void)
{
    printf("执行命令: 打开双闪灯\n");
    dot_matrix_screen_show_string("123");
    dot_matrix_screen_set_brightness(2500);
    completecommand_flag = 1;
    delay_ms(3000);
}

void handle_close_hazard(void)
{
    printf("执行命令: 关闭双闪灯\n");
    // 关闭双闪灯的函数调用
}

void handle_open_left_turn(void)
{
    printf("执行命令: 打开左转灯\n");
    dot_matrix_screen_show_string("789");
    dot_matrix_screen_set_brightness(2500);
    completecommand_flag = 1;
    delay_ms(3000);
}

void handle_close_left_turn(void)
{
    printf("执行命令: 关闭左转灯\n");
    // 关闭左转灯的函数调用
}

void handle_open_right_turn(void)
{
    printf("执行命令: 打开右转灯\n");
    dot_matrix_screen_show_string("456");
    dot_matrix_screen_set_brightness(2500);
    completecommand_flag = 1;
    delay_ms(3000);
}

void handle_close_right_turn(void)
{
    printf("执行命令: 关闭右转灯\n");
    // 关闭右转灯的函数调用
}

void handle_open_low_beam(void)
{
    printf("执行命令: 打开近光灯\n");
    dot_matrix_screen_show_string(":;<");
    dot_matrix_screen_set_brightness(2500);
    completecommand_flag = 1;
    delay_ms(3000);
}

void handle_close_low_beam(void)
{
    printf("执行命令: 关闭近光灯\n");
    // 关闭近光灯的函数调用
}

void handle_open_high_beam(void)
{
    printf("执行命令: 打开远光灯\n");
    dot_matrix_screen_show_string("=>?");
    dot_matrix_screen_set_brightness(2500);
    completecommand_flag = 1;
    delay_ms(3000);
}

void handle_close_high_beam(void)
{
    printf("执行命令: 关闭远光灯\n");
    // 关闭远光灯的函数调用
}

void handle_open_fog_light(void)
{
    printf("执行命令: 打开雾灯\n");
    dot_matrix_screen_show_string("@AB");
    dot_matrix_screen_set_brightness(2500);
    completecommand_flag = 1;
    delay_ms(3000);
}

void handle_close_fog_light(void)
{
    printf("执行命令: 关闭雾灯\n");
    // 关闭雾灯的函数调用
}

void handle_move_forward_10m(void)
{
    printf("执行命令: 向前直行10米\n");
    //    encoder_clear_count(ENCODER_DIR);
    //    distance = 0.0f;//置零！
    //    target_speed = 2; //
    //    while (distance < TARGET_DISTANCE)
    //    {
    //        Encoder_get_speed();
    //        Motor_PID_Control(target_speed);
    //        distance += speed * SAMPLE_TIME;
    //        // 延时10ms
    // delay_ms(10);
    //    }
    //    BLDC_Set_Duty(0);
    //    completecommand_flag = 1;
    //    delay_ms(1000);
}

void handle_move_backward_10m(void)
{
    printf("执行命令: 后退直行10米\n");
    //    encoder_clear_count(ENCODER_DIR);
    //    distance = 0.0f;//置零！
    //    target_speed = -2; //
    //    while (distance < TARGET_DISTANCE)
    //    {
    //        Encoder_get_speed();
    //        Motor_PID_Control(target_speed);
    //        distance += speed * SAMPLE_TIME;
    //        // 延时10ms
    //        delay_ms(10);
    //    }
    //    BLDC_Set_Duty(0);
    //    completecommand_flag = 1;
    //    delay_ms(1000);
}
#define SNAKE_ANGLE 10.0f // 蛇形摆动的固定角度，单位：度
#define SNAKE_PERIOD 2000 // 蛇形摆动的周期，单位：ms
void handle_snake_forward_10m(void)
{
    printf("执行命令: 蛇形前进10米\n");
    //    distance = 0.0f;  // 初始化行驶距离
    //    last_time = get_current_time();  // 假设存在获取当前时间的函数
    //
    //    while (distance < TARGET_DISTANCE) {
    //        uint32_t current_time = get_current_time();
    //        uint32_t elapsed_time = current_time - last_time;
    //        // 更新速度和距离
    //        distance += speed * (elapsed_time / 1000.0f);  //单位确定一下：速度单位：m/s，时间单位：ms
    //        target_angle;
    //        if ((elapsed_time % SNAKE_PERIOD) < (SNAKE_PERIOD / 2)) {
    //            target_angle = SNAKE_ANGLE;
    //        } else {
    //            target_angle = -SNAKE_ANGLE;
    //        }
    //        Steer_PID_Control(target_angle);
    //        target_speed = 3;  // 目标速度，单位：m/s
    //        Motor_PID_Control(target_speed);
    //        last_time = current_time;
    //        completecommand_flag = 1;
    //        delay_ms(1000);
}

void handle_snake_backward_10m(void)
{
    printf("执行命令: 蛇形后退10米\n");
    //    distance = 0.0f;  // 初始化行驶距离
    //    last_time = get_current_time();  // 假设存在获取当前时间的函数
    //
    //    while (distance < TARGET_DISTANCE) {
    //        uint32_t current_time = get_current_time();
    //        uint32_t elapsed_time = current_time - last_time;
    //        // 更新速度和距离
    //        distance += speed * (elapsed_time / 1000.0f);  //单位确定一下：速度单位：m/s，时间单位：ms
    //        target_angle;
    //        if ((elapsed_time % SNAKE_PERIOD) < (SNAKE_PERIOD / 2)) {
    //            target_angle = SNAKE_ANGLE;
    //        } else {
    //            target_angle = -SNAKE_ANGLE;
    //        }
    //        Steer_PID_Control(target_angle);
    //        target_speed = -3;  // 目标速度，单位：m/s
    //        Motor_PID_Control(target_speed);
    //        last_time = current_time;
    //        completecommand_flag = 1;
    //        delay_ms(1000);
}
#define ROTATION_TIME 5000
void handle_turn_counterclock(void)
{
    printf("执行命令: 逆时针转一圈\n");
    //    // 记录开始时间
    //    last_time = get_current_time();
    //    while (get_current_time() - last_time < ROTATION_TIME) {
    //        Steer_PID_Control(-20);
    //        target_speed = 2;
    //        Motor_PID_Control(target_speed);
    //    }
    //    // 停止电机和舵机
    //    BLDC_Set_Duty(0);
    //    Steer_set_angle(0);
    //    completecommand_flag = 1;
    //    delay_ms(1000);
}

void handle_turn_clockwise(void)
{
    printf("执行命令: 顺时针转一圈\n");
    //    last_time = get_current_time();
    //    while (get_current_time() - last_time < ROTATION_TIME) {
    //        Steer_PID_Control(20);
    //        target_speed = 2;
    //        Motor_PID_Control(target_speed);
    //    }
    //    // 停止电机和舵机
    //    BLDC_Set_Duty(0);
    //    Steer_set_angle(0);
    //    completecommand_flag = 1;
    //    delay_ms(1000);
}

// 停车控制命令处理函数
void handle_parking_1(void)
{
    printf("执行命令: 停进停车区1\n");
    // 停进停车区1的函数调用
}

void handle_parking_2(void)
{
    printf("执行命令: 停进停车区2\n");
    // 停进停车区2的函数调用
}

void handle_parking_3(void)
{
    printf("执行命令: 停进停车区3\n");
    // 停进停车区3的函数调用
}

void True_complete_command()
{
    if (voice_flag == 1)
    {
        // 中断音频接收定时器
        pit_disable(ASR_PIT);
        printf("音频接收定时器已关闭。\r\n");

        wifi_uart_disconnected_wifi(); // 断开WIFI连接
        printf("WiFi连接已断开。\r\n");
        delay_ms(100); // 等待100ms

        printf("按钮20.9按下，初始化点阵屏...\r\n");
        dot_matrix_screen_init(); // 初始化点阵屏
        printf("点阵屏初始化完成，执行handle_open_high_beam()\r\n");
        complete_command();
        // handle_open_high_beam();           // 执行高亮灯操作
        voice_flag = 0; // 重置语音标志位
    }
}
