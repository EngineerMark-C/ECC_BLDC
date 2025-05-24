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


void receive_command()
{
    for (uint32 i = 0; i < 256; i++)
    {
        buffer[i] = 0;
    }
    uint32 read_len = wifi_uart_read_buffer(buffer, sizeof(buffer));
    if (read_len > 0)
    {
        for (uint32 i = 0; i < read_len; i++)
        {
            printf("buffer[%d] = %d\n", i, buffer[i]);
        }

            for (uint32 i = 0; i < 256; i++)
    {
        buffer[i] = 0;
    }

    uint32 read_len = wifi_uart_read_buffer(command, sizeof(command));
    if (read_len > 0)
    {
        for (uint32 i = 0; i < read_len; i++)
        {
            printf("command[%d] = %c\n", i, command[i]);
        }
        // for (uint32 i = 0; i < read_len; i++)
        // {
        //     uint8 Remotecontrol_Flag = buffer[i];
        //     printf("Remotecontrol_Flag = %d, i = %d\n", Remotecontrol_Flag, i);

        //     switch(Remotecontrol_Flag)
        //     {
        //         case 1: // 后退
        //             printf("1: Go backward\n");
        //             GPS_ENU_Navigation();
        //             break;
        //         case 2: // 左转
        //             printf("2: Turn left\n");
        //             INS_Navigation();
        //             break;
        //         case 3: // 右转
        //             printf("3: Turn right\n");
        //             GPS_INS_Navigation();
        //             break;
        //         case 4: // 停止
        //             printf("4: Stop\n");
        //             GPS_ENU_INS_Navigation();
        //             break;
        //         default:
        //             printf("Unknown command: %d\n", Remotecontrol_Flag);
        //             break;
        //     }
        // }
    }
}
}




