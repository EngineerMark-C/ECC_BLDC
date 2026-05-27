#include "init.h"

volatile uint8_t Fire_Flag  = 0;                               //车辆启动标志位

NavigationFlag Navigation_Flag;                       //导航标志位初始化

void Navigation_Mode_Switch(void)
{
    if (Fire_Flag == 1)
    {
        switch(Navigation_Flag)
        {
            case GPS_NAV_Flag:
                GPS_Navigation();
                break;
            case GPS_ENU_NAV_Flag:
                GPS_ENU_Navigation();
                break;
            case INS_NAV_Flag:
                INS_Navigation();
                break;
            case GPS_INS_NAV_Flag:
                GPS_INS_Navigation();
                break;  
            case GPS_ENU_INS_NAV_Flag:
                GPS_ENU_INS_Navigation();
                break;
        }
    }
    if (Fire_Flag == 0)
    {
        // Remote_Control();                               // 遥控逻辑
        Brake();
    }
}
