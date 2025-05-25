#include "init.h"
#include "wifi.h"
#include "steer.h"
#include "bldc.h"
#include <string.h>
#include <stdlib.h>
// 添加WiFi UART设备头文件
#include "zf_device_wifi_uart.h"

#define WIFI_SSID_TEST "4D421-422"
#define WIFI_PASSWORD_TEST "NO.0STUDIO"

#define CMD_BUFFER_SIZE 256
#define MAX_VALUE 800 // 最大值限制

uint8_t buffer[CMD_BUFFER_SIZE]; // 缓冲区，用于存储读取到的原始数据
char command[CMD_BUFFER_SIZE];   // 缓冲区，用于存储读取到的字符串数据

// 当前车辆状态
static int16_t current_speed = 0;
static float current_angle = 0.0f;

void Wifiudp_Init()
{
    while (wifi_uart_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST, WIFI_UART_STATION))
    {
        system_delay_ms(500);
        printf("\r\n Wi-Fi UART init failed, retrying...\r\n");
    }

    if (wifi_uart_connect_udp_client(
            WIFI_UART_TARGET_IP,
            WIFI_UART_TARGET_PORT,
            WIFI_UART_LOCAL_PORT,
            WIFI_UART_COMMAND))
    {
        printf("\r\n Connect UDP Client error, try again.");
        system_delay_ms(500);
    }

    printf("\r\n Module IP: %s", wifi_uart_information.wifi_uart_local_ip);
}

// 解析接收到的命令
wifi_command_struct wifi_parse_command(const char *cmd_str)
{
    wifi_command_struct result = {CMD_UNKNOWN, 0};

    // 解析角度命令: "angle:XX"
    if (strncmp(cmd_str, "angle:", 6) == 0)
    {
        int16_t angle_value = (int16_t)atof(cmd_str + 6);
        result.cmd = CMD_ANGLE;
        result.value = angle_value;
    }
    // 解析速度命令: "speed:XX"
    else if (strncmp(cmd_str, "speed:", 6) == 0)
    {
        int16_t speed_value = (int16_t)atoi(cmd_str + 6);
        // 限制速度范围
        if (speed_value > MAX_VALUE)
            speed_value = MAX_VALUE;
        if (speed_value < -MAX_VALUE)
            speed_value = -MAX_VALUE;

        result.cmd = CMD_SPEED;
        result.value = speed_value;
    }
    // 按钮按下命令
    else if (strcmp(cmd_str, "forward") == 0)
    {
        result.cmd = CMD_FORWARD;
    }
    else if (strcmp(cmd_str, "backward") == 0)
    {
        result.cmd = CMD_BACKWARD;
    }
    else if (strcmp(cmd_str, "left") == 0)
    {
        result.cmd = CMD_LEFT;
    }
    else if (strcmp(cmd_str, "right") == 0)
    {
        result.cmd = CMD_RIGHT;
    }
    else if (strcmp(cmd_str, "stop") == 0)
    {
        result.cmd = CMD_STOP;
    }

    return result;
}

// 执行命令
void wifi_execute_command(wifi_command_struct command)
{
    switch (command.cmd)
    {
    case CMD_FORWARD:
        BLDC_Set_Duty(MAX_VALUE);  // 半速前进
        current_speed = MAX_VALUE; // 更新当前速度
        break;

    case CMD_BACKWARD:
        BLDC_Set_Duty(-MAX_VALUE);  // 半速后退
        current_speed = -MAX_VALUE; // 更新当前速度
        break;

    case CMD_LEFT:
        Steer_set_duty(PWM_STEER_MAX_R); // 左转
        current_angle = PWM_STEER_MAX_R; // 更新当前角度
        break;

    case CMD_RIGHT:
        Steer_set_duty(PWM_STEER_MIN_R); // 右转
        current_angle = PWM_STEER_MIN_R; // 更新当前角度
        break;

    case CMD_STOP:
        BLDC_Set_Duty(0); // 停止
        current_speed = 0;
        Steer_set_duty(PWM_STEER_MIDDLE_R); // 回中
        current_angle = PWM_STEER_MIDDLE_R;
        break;

    case CMD_SPEED:
            BLDC_Set_Duty(command.value);
            current_speed = command.value;
        break;

    case CMD_ANGLE:
    {
            // 反转映射关系：当command.value从MIN到MAX时，实际duty从MAX到MIN
        float duty = PWM_STEER_MAX_R + PWM_STEER_MIN_R - command.value;
            Steer_set_duty(duty);
        current_angle = command.value; // 保存命令值而非实际duty值
    }
        break;

    default:
        // 未知命令不处理
        break;
    }

    // 添加调试输出
    printf("\r\nExecuted command: %d, Value: %d\r\n", command.cmd, command.value);
    printf("\r\nCurrent state: Speed=%d, Angle=%.1f\r\n", current_speed, current_angle);

    // 发送状态回应
    wifi_send_status(current_speed, current_angle);
}

void wifi_process(void)
{
    uint32_t receive_len = 0;

    // 清空缓冲区
    memset(buffer, 0, CMD_BUFFER_SIZE);

    // 尝试接收数据
    if (wifi_uart_read_buffer(buffer, CMD_BUFFER_SIZE))
    {
        receive_len = strlen((char *)buffer); // 获取接收到的数据长度
        if (receive_len > 0 && receive_len < CMD_BUFFER_SIZE)
        {
            // 确保以null结尾
            buffer[receive_len] = '\0';

            // 添加调试信息 - 显示原始接收数据
            printf("\r\nRaw received data: [%s]\r\n", buffer);

            // 处理ESP8266的"+IPD,x:"前缀
            char *actual_cmd = (char *)buffer;

            // 首先去除开头的空白字符和换行符
            while (*actual_cmd && (*actual_cmd == '\r' || *actual_cmd == '\n' || *actual_cmd == ' ' || *actual_cmd == '\t'))
            {
                actual_cmd++;
            }

            // 检查并提取"+IPD,x:"格式的命令
            if (strncmp(actual_cmd, "+IPD,", 5) == 0)
            {
                // 查找实际命令的起始位置（冒号后面）
                char *colon_pos = strchr(actual_cmd, ':');
                if (colon_pos != NULL)
                {
                    actual_cmd = colon_pos + 1;
                }
            }

            // 添加调试信息 - 显示提取的命令
            printf("\r\nExtracted command before cleaning: [%s]\r\n", actual_cmd);

            // 复制实际命令到命令缓冲区
            strncpy(command, actual_cmd, CMD_BUFFER_SIZE - 1);
            command[CMD_BUFFER_SIZE - 1] = '\0';

            // 去除前后的空白字符和不可见字符
            // 先处理开头
            char *start = command;
            while (*start && (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n' || *start < 32))
            {
                start++;
            }

            // 如果有移动，则调整字符串
            if (start > command)
            {
                memmove(command, start, strlen(start) + 1);
            }

            // 再处理结尾
            char *p = command + strlen(command) - 1;
            while (p >= command && (*p == '\n' || *p == '\r' || *p == ' ' || *p == '\t' || *p < 32))
            {
                *p = '\0';
                p--;
            }

            // 添加更多调试信息 - 显示清理后的命令
            printf("\r\nCleaned command: [%s], length: %d\r\n", command, (int)strlen(command));

            // 只处理非空命令
            if (strlen(command) > 0)
            {
                // 添加字节级调试 - 显示每个字符的ASCII值
                printf("\r\nCommand bytes after cleaning: ");
                for (int i = 0; i < strlen(command); i++)
                {
                    printf("%02X ", (unsigned char)command[i]);
                }
                printf("\r\n");

                // 解析并执行命令
                wifi_command_struct cmd = wifi_parse_command(command);

                // 添加命令解析结果调试
                printf("\r\nParsed command type: %d, value: %d\r\n", cmd.cmd, cmd.value);

                // 只有当命令有效时才执行
                if (cmd.cmd != CMD_UNKNOWN)
                {
                    printf("\r\nExecuting valid command: [%s]\r\n", command);
                    wifi_execute_command(cmd);
                }
                else
                {
                    printf("\r\nUnknown command ignored: [%s]\r\n", command);
                }
            }
            else
            {
                printf("\r\nReceived empty command, ignored\r\n");
            }
        }
    }
}

// 发送车辆状态数据到上位机
void wifi_send_status(int16_t speed, float angle)
{
    char status_msg[64];
    sprintf(status_msg, "status:speed=%d,angle=%.1f", speed, angle);

    // 使用UDP发送状态
    wifi_uart_send_buffer((uint8_t *)status_msg, strlen(status_msg));
}
