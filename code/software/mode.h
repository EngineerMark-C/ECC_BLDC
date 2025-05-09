#ifndef _mode_h_
#define _mode_h_

extern uint8_t Fire_Flag;

typedef enum {
    GPS_NAV_Flag,                                                    //GPS导航标志位
    GPS_ENU_NAV_Flag,                                                //GPS_ENU导航标志位
    INS_NAV_Flag,                                                    //INS导航标志位
    GPS_INS_NAV_Flag,                                                //GPS_INS导航标志位
    GPS_ENU_INS_NAV_Flag                                             //GPS_ENU_INS导航标志位
} NavigationFlag;

extern NavigationFlag Navigation_Flag;

void Navigation_Mode_Switch(void);

#endif