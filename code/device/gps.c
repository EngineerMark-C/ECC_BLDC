#include "init.h"

struct Now_gnss NOW_location;

void GPS_Init(void)
{
    gnss_init(TAU1201);
    pit_ms_init(PIT2, 10);                                                     // 初始化PIT2为周期中断20ms周期   实际上是导航更新的周期
}

void Get_Now_Location(void)
{
    if(gnss_flag)
    {
        // 解析GPS数据
        if(0 == gnss_data_parse())
        {
            if(gnss.state)
            {
                NOW_location.latitude = gnss.latitude;
                NOW_location.longitude = gnss.longitude;
            }
        }
        gnss_flag = 0;
    }
}

// 打印GPS信息到串口
void Print_Gps_Info(void)
{
    // 解析GPS数据
    if(0 == gnss_data_parse())
    {
        printf("----------------------------------------\n");
        printf("GPS状态: %s\n", gnss.state ? "有效" : "无效");
        
        if(gnss.state)  // 如果GPS定位有效
        {
            // 打印位置信息
            printf("经度: %.15g°%c\n", gnss.longitude, gnss.ew);
            printf("纬度: %.15g°%c\n", gnss.latitude, gnss.ns);
            printf("海拔: %.6fm\n", gnss.height);
            
            // 打印运动信息
            printf("速度: %.6fkm/h\n", gnss.speed);
            printf("航向: %.6f°\n", gnss.direction);
            
            // 打印卫星信息
            printf("使用卫星数: %d\n", gnss.satellite_used);
            
            // 打印时间信息
            printf("时间: %d-%02d-%02d %02d:%02d:%02d\n", 
                gnss.time.year, gnss.time.month, gnss.time.day,
                gnss.time.hour, gnss.time.minute, gnss.time.second);
        }
        printf("----------------------------------------\n");
    }
}
