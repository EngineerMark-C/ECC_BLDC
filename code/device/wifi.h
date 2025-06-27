#ifndef _wifi_h
#define _wifi_h

#include "init.h"

// 控制命令定义
typedef enum {
    CMD_UNKNOWN = 0,
    CMD_FORWARD,
    CMD_BACKWARD,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_STOP,
    CMD_SPEED,
    CMD_ANGLE,
    CMD_PING     // 添加新的连接测试命令类型
} wifi_command_type;

// 命令结构体
typedef struct {
    wifi_command_type cmd;
    int16_t value;
} wifi_command_struct;

// 函数声明
void Wifiudp_Init(void);
void wifi_process(void);
void wifi_send_status(int16_t speed, float angle);
wifi_command_struct wifi_parse_command(const char* cmd_str);
void wifi_execute_command(wifi_command_struct command);
void check_command_timeout(void);

#endif