#ifndef _gps_h_
#define _gps_h_

#define PIT2                            (CCU61_CH0 )                            // 使用的周期中断编号

struct Now_gnss
{
    double latitude;
    double longitude;
};

extern struct Now_gnss NOW_location;

void GPS_Init(void);                                                            // GPS 初始化
void Get_Now_Location(void);                                                    // 获取当前位置
void Print_Gps_Info(void);                                                      // 打印 GPS 信息

#endif