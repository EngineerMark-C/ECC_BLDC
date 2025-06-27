#include "init.h"

float target_speed;
float target_angle;

float MAX_SPEED;                                                                // 最大速度
float MIN_SPEED;                                                                // 最小速度
float APPROACH_SPEED;                                                           // 靠近速度
float BRAKING_DISTANCE;                                                         // 开始减速距离

float GPS_SWITCH_DISTANCE;                                                      // GPS 切换距离
float INS_SWITCH_DISTANCE;                                                      // INS 切换距离

float GPS_ENU[MAX_GPS_POINTS][2];                                               // GPS ENU 坐标

float S_Distance;                                                               // S 型走位偏移距离

uint8_t GPS_Point_Index = 0;                                                    // GPS 数据索引
double GPS_Point[MAX_GPS_POINTS][2];                                            // GPS 数据
uint8_t Start_GPS_Point;                                                        // 第一个 GPS 数据索引
uint8_t End_GPS_Point;                                                          // 最后一个 GPS 数据索引
uint8_t NOW_GPS_Point;                                                          // 当前 GPS 数据索引

uint8_t INS_Point_Index = 0;                                                    // INS 数据索引
float INS_Point[MAX_INS_POINTS][2];                                             // INS 点位
uint8_t Start_INS_Point;                                                        // 第一个 INS 数据索引
uint8_t Back_INS_Point;                                                         // 掉头 INS 数据索引
uint8_t End_INS_Point;                                                          // 最后一个 INS 数据索引
uint8_t NOW_INS_Point;                                                          // 当前 INS 数据索引

uint8_t S_Point_Index = 0;                                                      // S 型走位数据索引
uint8_t Start_S_Point;                                                          // S 型走位开始索引
float S_Point[MAX_INS_POINTS][2];                                               // S 型走位点
uint8_t End_S_Point;                                                            // S 型走位结束索引
uint8_t NOW_S_Point;                                                            // 当前 S 型走位索引

uint8_t GPS_TO_INS_Point = 0;                                                   // GPS点位转换到INS点位

typedef struct {
    double origin_lat;    // 原点纬度（弧度）
    double origin_lon;    // 原点经度（弧度）
    float easting;        // 东向坐标（米）
    float northing;       // 北向坐标（米）
} CoordinateSystem;

CoordinateSystem local_frame;  // 本地坐标系

// 到达标志位
uint8_t reach_flag = 0;

// 速度管理函数
void Speed_Management(float distance)
{
    float current_target_speed;

    // 动态速度曲线：距离越近速度越慢
    if(distance > BRAKING_DISTANCE) {
        current_target_speed = MAX_SPEED;
    } 
    else 
    {
        // 线性减速区间
        // 当前目标速度 = 靠近速度 + (最大速度 - 靠近速度) * (当前距离 / 减速距离)
        current_target_speed = APPROACH_SPEED + (MAX_SPEED - APPROACH_SPEED) * (distance / BRAKING_DISTANCE) * 0.6f;

        // 确保不低于最小速度
        current_target_speed = fmaxf(current_target_speed, MIN_SPEED);
    }
    target_speed = current_target_speed;
}

// 初始化零点坐标
void Local_Frame_Init(double lat0, double lon0) 
{
    local_frame.origin_lat = ANGLE_TO_RAD(lat0);
    local_frame.origin_lon = ANGLE_TO_RAD(lon0);
}

// WGS84坐标转ENU坐标
void WGS84_to_ENU(double lat, double lon, float* east, float* north) 
{
    const double a = 6378137.0;       // WGS84长半轴
    const double f = 1.0/298.257223563; // 扁率
    
    double sin_lat0 = sin(local_frame.origin_lat);
    double cos_lat0 = cos(local_frame.origin_lat);
    
    double dLon = ANGLE_TO_RAD(lon) - local_frame.origin_lon;
    double dLat = ANGLE_TO_RAD(lat) - local_frame.origin_lat;
    
    // 卯酉圈曲率半径
    double N = a / sqrt(1 - (2*f - f*f)*sin_lat0*sin_lat0);
    
    // 泰勒展开近似（适用于10km范围内）
    // *east  = (float)(N * cos_lat0 * dLon);
    // *north = (float)(N * dLat - 0.5 * N * (dLat*dLat)*sin_lat0*cos_lat0);
    *east  = - (float)(N * dLat - 0.5 * N * (dLat*dLat)*sin_lat0*cos_lat0);
    *north = (float)(N * cos_lat0 * dLon);
}

// 将路径点预转换为ENU坐标（启动时初始化）
void WGS84_to_ENU_Init(void)
{
    Local_Frame_Init(GPS_Point[0][0], GPS_Point[0][1]);
    for(int i=0; i <= End_GPS_Point; i++){
        WGS84_to_ENU(GPS_Point[i][0], GPS_Point[i][1], 
                    &GPS_ENU[i][0], &GPS_ENU[i][1]);
    }
}

void Mirror_INS_Point_Generate(void)
{
    // 镜像 INS 点位
    for(uint8_t i=1; i <= Back_INS_Point; i++)
    {
        INS_Point[Back_INS_Point + i][0] = INS_Point[Back_INS_Point - i][0];
        INS_Point[Back_INS_Point + i][1] = INS_Point[Back_INS_Point - i][1];
    }
}

// S 型走位点位生成
void S_Point_Generate(uint8_t i)
{
    S_Point[i][0] = INS_Point[i][0];
    S_Point[i][1] = INS_Point[i][1] + ((i & 1) ? 1 : -1) * S_Distance;
}

// 生成所有 S 型走位点
void S_Point_Generate_All(void)
{
    for(uint8_t i=0; i < End_S_Point; i++)
    {
        S_Point_Generate(i);
    }
}

void GPS_Point_to_Point(uint8_t i)
{
    // char str[20];
    // sprintf(str, "go to %d", i);
    // ips114_show_string(0, 112, str);
    double angle = get_two_points_azimuth(NOW_location.latitude, NOW_location.longitude, GPS_Point[i][0], GPS_Point[i][1]);
    double distance = get_two_points_distance(NOW_location.latitude, NOW_location.longitude, GPS_Point[i][0], GPS_Point[i][1]);

    // target_speed = 0.0f;
    target_angle = (float)angle;
    Speed_Management((float)distance);
    // ips114_show_float(0, 96, target_angle, 5, 1);
    // ips114_show_float(90, 96, (float)distance, 5, 1);
    if (distance < GPS_SWITCH_DISTANCE)
    {
        reach_flag = 1;
    }
}

void GPS_ENU_Point_to_Point(uint8_t i)
{
    // 使用平面坐标系计算
    float dx = GPS_ENU[i][0] - position[0];  // 东向差值
    float dy = GPS_ENU[i][1] - position[1];  // 北向差值
    
    float angle = RAD_TO_ANGLE(atan2f(dy, dx));
    angle = angle < 0 ? angle + 360 : angle;
    float distance = sqrtf(dx*dx + dy*dy);
    
    target_angle = angle;
    Speed_Management(distance);
    
    // 修改到达判断条件
    if (distance < GPS_SWITCH_DISTANCE)
    {
        reach_flag = 1;
    }
}

void GPS_Navigation(void)
{
    if (NOW_GPS_Point > End_GPS_Point) {
        target_speed = 0.0f;
        return;
    }
    if (Start_GPS_Point < End_GPS_Point)
    {
        GPS_Point_to_Point(NOW_GPS_Point);
        if (reach_flag)  // 改为检查标志位
        {
            NOW_GPS_Point++;
            reach_flag = 0;  // 重置标志位
        }
    }
}

void GPS_ENU_Navigation(void)
{
    if (NOW_GPS_Point > End_GPS_Point) {
        target_speed = 0.0f;
        return;
    }
    if (Start_GPS_Point < End_GPS_Point)
    {
        GPS_ENU_Point_to_Point(NOW_GPS_Point);
        if (reach_flag)  // 改为检查标志位
        {
            NOW_GPS_Point++;
            reach_flag = 0;  // 重置标志位
        }
    }
}

// S 型走位导航
void S_Point_to_Point(uint8_t i)
{
    // 使用平面坐标系计算（单位：米）
    float dx = S_Point[NOW_S_Point][0] - position[0];
    float dy = S_Point[NOW_S_Point][1] - position[1];
    
    // 计算平面方位角（0-360度）
    float angle = RAD_TO_ANGLE(atan2f(dy, dx));
    angle = angle < 0 ? angle + 360 : angle;
    
    // 计算欧几里得距离
    float distance = sqrtf(dx*dx + dy*dy);

    target_angle = angle;
    Speed_Management(distance);
    
    if (distance < INS_SWITCH_DISTANCE)
    {
        reach_flag = 1;
    }
}

// S 型走位导航
void S_Point_Navigation(void)
{
    if (NOW_S_Point > End_S_Point) {
        target_speed = 0.0f;
        return;
    }
    if (Start_S_Point < End_S_Point)
    {
        S_Point_to_Point(NOW_S_Point);
        if (reach_flag)
        {
            NOW_S_Point++;
            reach_flag = 0;  // 重置标志位
        }
    }
}

void INS_Point_to_Point(uint8_t i)
{
    // 使用平面坐标系计算（单位：米）
    float dx = INS_Point[i][0] - position[0];
    float dy = INS_Point[i][1] - position[1];
    
    // 计算平面方位角（0-360度）
    float angle = RAD_TO_ANGLE(atan2f(dy, dx));
    angle = angle < 0 ? angle + 360 : angle;
    
    // 计算欧几里得距离
    float distance = sqrtf(dx*dx + dy*dy);

    target_angle = angle;
    Speed_Management(distance);

    if (distance < INS_SWITCH_DISTANCE)
    {
        reach_flag = 1;
    }
}

void INS_Navigation(void)
{
    if (NOW_INS_Point > End_INS_Point) {
        target_speed = 0.0f;
        return;
    }
    if (Start_INS_Point < End_INS_Point)
    {
        if (NOW_S_Point == NOW_INS_Point && NOW_S_Point < End_S_Point) 
        {
            S_Point_to_Point(NOW_S_Point);
            if (reach_flag)
            {
                NOW_S_Point++;
                NOW_INS_Point++;
                reach_flag = 0;  // 重置标志位
            }
        } 
        else 
        {
            INS_Point_to_Point(NOW_INS_Point);
            if (reach_flag)
            {
                NOW_INS_Point++;
                reach_flag = 0;  // 重置标志位
            }
        }
    }
}

void GPS_INS_Navigation(void)
{
    static uint8_t navigation_phase = 0;  // 0:GPS导航阶段  1:INS导航阶段  2:返回GPS导航阶段
    
    // 添加全局停车判断
    if (NOW_GPS_Point > End_GPS_Point && NOW_INS_Point > End_INS_Point) {
        target_speed = 0.0f;
        return;
    }
    
    switch(navigation_phase) {
        case 0:
            if (NOW_GPS_Point <= GPS_TO_INS_Point) {
                // 继续使用GPS导航到切换点
                GPS_Point_to_Point(NOW_GPS_Point);
                if (reach_flag)  // 改为检查标志位
                {
                    if (NOW_GPS_Point == GPS_TO_INS_Point)
                    {
                        // 到达切换点，准备切换到INS导航
                        navigation_phase = 1;
                        NOW_INS_Point = Start_INS_Point;  // 初始化INS起始点
                    }
                    NOW_GPS_Point++;
                    reach_flag = 0;  // 重置标志位
                }
            } else {
                target_speed = 0.0f;  // GPS阶段超出范围时停车
            }
            break;
            
        case 1:
            if (NOW_INS_Point <= End_INS_Point) {
                INS_Point_to_Point(NOW_INS_Point);
                if (reach_flag)
                {
                    NOW_INS_Point++;
                    if (NOW_INS_Point > End_INS_Point)
                    {
                        // INS导航结束，切回GPS导航
                        navigation_phase = 2;
                        NOW_GPS_Point = GPS_TO_INS_Point + 1;  // 从切换点后的GPS点继续导航
                    }
                    reach_flag = 0;  // 重置标志位
                }
            } else {
                target_speed = 0.0f;  // INS阶段超出范围时停车
            }
            break;
            
        case 2:
            if (NOW_GPS_Point <= End_GPS_Point) {
                GPS_Point_to_Point(NOW_GPS_Point);
                if (reach_flag)  // 改为检查标志位
                {
                    NOW_GPS_Point = NOW_GPS_Point + 1;
                }
                reach_flag = 0;  // 重置标志位
            } else {
                target_speed = 0.0f;  // 最终GPS阶段超出范围时停车
            }
            break;
    }
}

void GPS_ENU_INS_Navigation(void)
{
    static uint8_t navigation_phase = 0;  // 0:GPS导航阶段  1:INS导航阶段  2:返回GPS导航阶段
    
    // 全局停车条件
    if (NOW_GPS_Point > End_GPS_Point && NOW_INS_Point > End_INS_Point) {
        target_speed = 0.0f;
        return;
    }
    
    switch(navigation_phase) {
        case 0:
            if (NOW_GPS_Point <= GPS_TO_INS_Point) {
                // 继续使用GPS导航到切换点
                GPS_ENU_Point_to_Point(NOW_GPS_Point);
                if (reach_flag)  // 改为检查标志位
                {
                    if (NOW_GPS_Point == GPS_TO_INS_Point)
                    {
                        // 到达切换点，准备切换到INS导航
                        navigation_phase = 1;
                        NOW_INS_Point = Start_INS_Point;  // 初始化INS起始点
                    }
                    NOW_GPS_Point++;
                    reach_flag = 0;  // 重置标志位
                }
            } else {
                target_speed = 0.0f;
            }
            break;
            
        case 1:
            if (NOW_INS_Point <= End_INS_Point) {
                INS_Point_to_Point(NOW_INS_Point);
                if (reach_flag)
                {
                    NOW_INS_Point++;
                    if (NOW_INS_Point > End_INS_Point)
                    {
                        // INS导航结束，切回GPS导航
                        navigation_phase = 2;
                        NOW_GPS_Point = GPS_TO_INS_Point + 1;  // 从切换点后的GPS点继续导航
                    }
                    reach_flag = 0;  // 重置标志位
                }
            } else {
                target_speed = 0.0f;
            }
            break;
            
        case 2:
            if (NOW_GPS_Point <= End_GPS_Point) {
                GPS_ENU_Point_to_Point(NOW_GPS_Point);
                if (reach_flag)  // 改为检查标志位
                {
                    NOW_GPS_Point = NOW_GPS_Point + 1;
                }
                reach_flag = 0;  // 重置标志位
            } else {
                target_speed = 0.0f;
            }
            break;
    }
}
