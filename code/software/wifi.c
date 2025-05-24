#include "init.h"

#define WIFI_SSID_TEST          "bless"
#define WIFI_PASSWORD_TEST      "88888888"

int8_t Remotecontrol_Flag = 9;                       // 遥控标志位初始化（修正数据类型）
uint8_t buffer[256];                                 // 缓冲区，用于存储读取到的原始数据
char command[256];                                   // 缓冲区，用于存储读取到的字符串数据

void wifiudp_init() {
    while (wifi_uart_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST, WIFI_UART_STATION)) {
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

void receive_command1() {
    uint32_t i;
//    char command[256] = {0};  // 命令缓冲区
    uint8_t data_buffer[256] = {0};  // 数据缓冲区
    uint32_t data_length = 0;
    
    // 读取串口数据
    uint32_t recv_len = wifi_uart_read_buffer(buffer, sizeof(buffer));
    
    if (recv_len > 0) {
        // 打印原始数据（调试用）
        printf("\r\n原始数据：");
        for (i = 0; i < recv_len; i++) {
            printf(" [%d]=%d", i, buffer[i]);
        }
        
        // 解析ESP8266数据包格式 "+IPD,n:data"
        // 简化版解析，实际应用中可能需要更复杂的解析逻辑
        if (recv_len > 10 && 
            buffer[0] == '\r' && buffer[1] == '\n' && 
            buffer[2] == '+' && buffer[3] == 'I' && buffer[4] == 'P' && buffer[5] == 'D') {
            
            // 查找数据起始位置（冒号后）
            uint32_t data_start = 0;
            for (i = 0; i < recv_len; i++) {
                if (buffer[i] == ':') {
                    data_start = i + 1;
                    break;
                }
            }
            
            // 提取数据长度（冒号前的数字）
            uint32_t len = 0;
            for (i = 6; i < data_start - 1; i++) {
                if (buffer[i] >= '0' && buffer[i] <= '9') {
                    len = len * 10 + (buffer[i] - '0');
                }
            }
            
            // 提取实际数据
            if (data_start > 0 && data_start + len <= recv_len) {
                memcpy(data_buffer, &buffer[data_start], len);
                data_length = len;
                
                // 打印提取的数据
                printf("\r\n提取的数据：");
                for (i = 0; i < data_length; i++) {
                    printf(" [%d]='%c'", i, data_buffer[i]);
                }
                
                // 处理命令
                if (data_length > 0) {
                    switch (data_buffer[0]) {
                        case '1':
                            printf("\r\n命令：前进");
                            BLDC_Set_Duty(800);
                            break;
                        case '2':
                            printf("\r\n命令：左转");
                            Steer_set_duty(PWM_STEER_MAX_R);
                            break;
                        case '3':
                            printf("\r\n命令：右转");
                            Steer_set_duty(PWM_STEER_MIN_R);
                            break;
                        case '4':
                            printf("\r\n命令：停止");
                            BLDC_Set_Duty(0);
                            break;
                        case '5':
                            printf("\r\n命令：后退");
                            BLDC_Set_Duty(-800);
                            break;
                        default:
                            printf("\r\n未知命令：%c", data_buffer[0]);
                            break;
                    }
                }
            }
        }
    }
}





