#include "init.h"

float target_speed = 0.0f;                                                      // 目标速度
float target_angle = 0.0f;                                                      // 目标角度

float MAX_SPEED;                                                                // 最大速度
float MIN_SPEED;                                                                // 最小速度
float BRAKING_DISTANCE;                                                         // 开始减速距离
float Brake_Threshold = 6.0f;                                                   // 刹车阈值

float S_MAX_SPEED;                                                              // S 型走位最大速度
float S_MIN_SPEED;                                                              // S 型走位最小速度
float S_BRAKING_DISTANCE;                                                       // S 型走位制动距离

float GPS_SWITCH_DISTANCE;                                                      // GPS 切换距离
float INS_SWITCH_DISTANCE;                                                      // INS 切换距离
float S_SWITCH_DISTANCE = 0.4f;                                                // S   切换距离

float GPS_ENU[MAX_GPS_POINTS][2];                                               // GPS ENU 坐标

float S_Distance;                                                               // S 型走位偏移距离

uint8_t GPS_Point_Index = 0;                                                    // GPS 数据索引
double GPS_Point[MAX_GPS_POINTS][2];                                            // GPS 数据
uint8_t Start_GPS_Point;                                                        // 第一个 GPS 数据索引
uint8_t End_GPS_Point;                                                          // 最后一个 GPS 数据索引
uint8_t NOW_GPS_Point;                                                          // 当前 GPS 数据索引

double Direction_Point[2][2];                                                   // 两个发车方向GPS点
float Start_Direction;                                                          // 发车方向角度
uint8_t Direction_Point_Index = 0;                                              // 发车方向点索引

uint8_t INS_Point_Index = 0;                                                    // INS 数据索引
float INS_Point[MAX_INS_POINTS][2];                                             // INS 点位
float INS_Point_Navigation_Frame[MAX_INS_POINTS][2];                            // INS 点位导航坐标系
uint8_t Start_INS_Point;                                                        // 第一个 INS 数据索引
uint8_t Back_INS_Point;                                                         // 掉头 INS 数据索引
uint8_t End_INS_Point;                                                          // 最后一个 INS 数据索引
uint8_t NOW_INS_Point;                                                          // 当前 INS 数据索引

uint8_t S_Point_Index = 0;                                                      // S 型走位数据索引
uint8_t Start_S_Point;                                                          // S 型走位开始索引
float S_Point[MAX_INS_POINTS][2];                                               // S 型走位点
float S_Point_Navigation_Frame[MAX_INS_POINTS][2];                              // S 型走位点导航坐标系
uint8_t End_S_Point;                                                            // S 型走位结束索引
uint8_t NOW_S_Point;                                                            // 当前 S 型走位索引

float next_target_angle = 0.0f;                                                 // 下一个目标角度

float now_s_distance = 0.0f;                                                    // 当前S型走位距离

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
uint8_t angle_flag = 0;

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
        current_target_speed = MIN_SPEED + (MAX_SPEED - MIN_SPEED) * (distance / BRAKING_DISTANCE) * 0.7f;
        // current_target_speed = MIN_SPEED + (MAX_SPEED - MIN_SPEED) * (distance / BRAKING_DISTANCE) * 0.6f;
        // 确保不低于最小速度
        current_target_speed = fmaxf(current_target_speed, MIN_SPEED);
    }
    target_speed = current_target_speed;
}

// S 型走位速度管理函数
void Speed_Management_For_S(float distance)
{
    float current_target_speed;

    // 动态速度曲线：距离越近速度越慢
    if(distance > S_BRAKING_DISTANCE)
    {
        current_target_speed = S_MAX_SPEED;
    } 
    else 
    {
        // 线性减速区间
        // 当前目标速度 = 靠近速度 + (最大速度 - 靠近速度) * (当前距离 / 减速距离)
        current_target_speed = S_MIN_SPEED + (S_MAX_SPEED - S_MIN_SPEED) * (distance / S_BRAKING_DISTANCE) * 0.7f;
        // current_target_speed = S_MIN_SPEED + (S_MAX_SPEED - S_MIN_SPEED) * (distance / (S_BRAKING_DISTANCE - INS_SWITCH_DISTANCE)) * 0.6f;
        // 确保不低于最小速度
        current_target_speed = fmaxf(current_target_speed, S_MIN_SPEED);
    }
    target_speed = current_target_speed;
}

void Brake(void)
{
    if (target_speed < MIN_SPEED)
    {
        target_speed = 0.0f;
    }
    else
    {
        target_speed -= 0.05f;
    }
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
    *east  = (float)(N * dLat - 0.5 * N * (dLat*dLat)*sin_lat0*cos_lat0);
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

// 将INS点位转换为导航坐标系
void Vehicle_To_Navigation(float Rotation_Angle, float origin_x, float origin_y, float* process_x, float* process_y)
{
    // 计算旋转后的坐标
    float cos_dir = cosf(Rotation_Angle);
    float sin_dir = sinf(Rotation_Angle);

    *process_x = origin_x * cos_dir - origin_y * sin_dir;
    *process_y = origin_x * sin_dir + origin_y * cos_dir;
}

void Vehicle_To_Navigation_INS(void)
{
    // 将所有INS点位转换为导航坐标系
    for(uint8_t i=0; i <= End_INS_Point; i++)
    {
        Vehicle_To_Navigation(ANGLE_TO_RAD(Start_Direction), 
                                INS_Point[i][0], INS_Point[i][1],
                                &INS_Point_Navigation_Frame[i][0], 
                                &INS_Point_Navigation_Frame[i][1]);
    }
}

void Vehicle_To_Navigation_S(void)
{
    // 将所有S型走位点转换为导航坐标系
    for(uint8_t i=0; i <= End_S_Point; i++)
    {
        Vehicle_To_Navigation(ANGLE_TO_RAD(Start_Direction), 
                                S_Point[i][0], S_Point[i][1],
                                &S_Point_Navigation_Frame[i][0],
                                &S_Point_Navigation_Frame[i][1]);
    }
}

// 获取发车方向
void Get_Start_Direction(void)
{
    //Direction_Point[0]发车方向起点
    //Direction_Point[1]发车方向终点
    Start_Direction = (float)get_two_points_azimuth(Direction_Point[0][0], Direction_Point[0][1],
                                            Direction_Point[1][0], Direction_Point[1][1]);
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
// void S_Point_Generate(uint8_t i)
// {
//     S_Point[i][0] = INS_Point[i][0];
//     S_Point[i][1] = INS_Point[i][1] + ((i & 1) ? 1 : -1) * S_Distance;
// }

// 生成所有 S 型走位点
void S_Point_Generate_All(void)
{
    for(uint8_t i=0; i < End_S_Point; i++)
    {
        if (i < Back_INS_Point) 
        {
        S_Point[i][0] = INS_Point[i][0];
        S_Point[i][1] = INS_Point[i][1] + ((i & 1) ? 1 : -1) * S_Distance;
        }
        else if (i == Back_INS_Point) 
        {
            S_Point[i][0] = INS_Point[i][0];
        }
        else
        {
        S_Point[i][0] = INS_Point[i][0];
        S_Point[i][1] = INS_Point[i][1] + ((i & 1) ? -1 : 1) * S_Distance;
        } 
    }
    // 将所有S型走位点转换为导航坐标系
    Vehicle_To_Navigation_S();
}

void Caculate_Next_GPS_Angle(uint8_t i)
{
    // 计算下一个目标角度
    if (i < End_GPS_Point)
    {
        double angle = get_two_points_azimuth(GPS_Point[i][0], GPS_Point[i][1], GPS_Point[i+1][0], GPS_Point[i+1][1]);
        next_target_angle = (float)angle;
    }
    else
    {
        next_target_angle = target_angle;
    }
}

void Caculate_Next_ENU_Angle(uint8_t i)
{
    // 计算下一个目标角度
    if (i < End_GPS_Point)
    {
        float dx = GPS_ENU[i+1][0] - GPS_ENU[i][0];
        float dy = GPS_ENU[i+1][1] - GPS_ENU[i][1];
        float next_angle = RAD_TO_ANGLE(atan2f(dy, dx));
        next_angle = next_angle < 0 ? next_angle + 360 : next_angle;
        next_target_angle = next_angle;
    }
    else
    {
        next_target_angle = target_angle;
    }
}

// 将ENU点位传递给INS点位
void ENU_To_INS_Points(void)
{
    for (uint8_t i = 0; i <= End_GPS_Point; i++)
    {
        // 将ENU点位传递给INS点位
        INS_Point[i][0] = GPS_ENU[i][0];
        INS_Point[i][1] = GPS_ENU[i][1];
    }
    Save_INS_Point_Memory();
    ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "ENU Points to INS.");
    system_delay_ms(500);
}

// 有待优化
void GPS_Point_to_Point(uint8_t i)
{
    double angle = get_two_points_azimuth(NOW_location.latitude, NOW_location.longitude, GPS_Point[i][0], GPS_Point[i][1]);
    double distance = get_two_points_distance(NOW_location.latitude, NOW_location.longitude, GPS_Point[i][0], GPS_Point[i][1]);

    if (NOW_GPS_Point < End_GPS_Point) 
    {
        // 计算下一个目标角度
        Caculate_Next_GPS_Angle(i);
    }

    if (reach_flag !=1)
    {
        target_angle = (float)angle;
        if (test_flag != TEST__3) Speed_Management((float)distance);
        else Speed_Management_For_Test3((float)distance, i);
    }

    if (distance < GPS_SWITCH_DISTANCE)
    {
        reach_flag = 1;
    }
}

void GPS_Navigation(void)
{
    if (NOW_GPS_Point > End_GPS_Point)
    {
        Brake();
        return;
    }
    if (Start_GPS_Point < End_GPS_Point)
    {
        GPS_Point_to_Point(NOW_GPS_Point);
        if (reach_flag )
        {
            target_angle = next_target_angle;
            target_speed = MIN_SPEED;
            if (angle_flag)  // 检查角度到达标志位
            {
                NOW_GPS_Point++;
                reach_flag = 0;  // 重置标志位
            }
        }
    }
}

void GPS_ENU_Point_to_Point(uint8_t i)
{
    // 使用平面坐标系计算
    float dx = GPS_ENU[i][0] - position[0];  // 东向差值
    float dy = GPS_ENU[i][1] - position[1];  // 北向差值


    if (i < End_GPS_Point) 
    {
        Caculate_Next_ENU_Angle(i);
    }

    float angle = RAD_TO_ANGLE(atan2f(dy, dx));
    angle = angle < 0 ? angle + 360 : angle;
    float distance = sqrtf(dx*dx + dy*dy);

    if (reach_flag !=1)
    {
        // target_angle = (float)angle;
        if (i == Test3Element[3].Point_Index )
        {
            target_angle += angle_adjustment;
        }
        else
        {
            target_angle = (float)angle;
        }
        if (test_flag != TEST__3) Speed_Management((float)distance);
        else Speed_Management_For_Test3((float)distance, i);
    }
    // 修改到达判断条件
    if (distance < GPS_SWITCH_DISTANCE)
    {
        reach_flag = 1;
    }
}

void GPS_ENU_Navigation(void)
{
    if (NOW_GPS_Point > End_GPS_Point) {
        Brake();
        return;
    }
    if (Start_GPS_Point < End_GPS_Point)
    {
        GPS_ENU_Point_to_Point(NOW_GPS_Point);
        if (reach_flag)
        {
            target_angle = next_target_angle;
            target_speed = MIN_SPEED;
            if (angle_flag)  // 检查角度到达标志位
            {
                NOW_GPS_Point++;
                reach_flag = 0;  // 重置标志位
            }
        }
    }
}

void Caculate_Next_S_Point_Angle(uint8_t i)
{
    // 计算下一个 S 型走位点
    if (i < End_S_Point) 
    {
        float dx = S_Point_Navigation_Frame[i+1][0] - S_Point_Navigation_Frame[i][0];
        float dy = S_Point_Navigation_Frame[i+1][1] - S_Point_Navigation_Frame[i][1];

        float next_angle = RAD_TO_ANGLE(atan2f(dy, dx));
        next_angle = next_angle < 0 ? next_angle + 360 : next_angle;

        next_target_angle = next_angle;
    }
    else 
    {
        next_target_angle = target_angle;
    }
}

void Caculate_Now_S_Point_Distance(uint8_t i)
{
    float dx, dy;
    if (i == 0) 
    {
        dx = S_Point_Navigation_Frame[i][0];
        dy = S_Point_Navigation_Frame[i][1];
    }
    else 
    {
        dx = S_Point_Navigation_Frame[i][0] - S_Point_Navigation_Frame[i-1][0];
        dy = S_Point_Navigation_Frame[i][1] - S_Point_Navigation_Frame[i-1][1];
    }
    now_s_distance = sqrtf(dx*dx + dy*dy);
}

void Caculate_Next_INS_Point_Angle(uint8_t i)
{
    // 计算下一个 INS 点位的角度
    if (i < End_INS_Point) 
    {
        float dx = INS_Point_Navigation_Frame[i+1][0] - INS_Point_Navigation_Frame[i][0];
        float dy = INS_Point_Navigation_Frame[i+1][1] - INS_Point_Navigation_Frame[i][1];

        float next_angle = RAD_TO_ANGLE(atan2f(dy, dx));
        next_angle = next_angle < 0 ? next_angle + 360 : next_angle;

        next_target_angle = next_angle;
    }
    else 
    {
        next_target_angle = target_angle;
    }
}

void INS_Point_Y_Zero(void)
{
    // 将 INS 点位的 Y 坐标归零
    for (uint8_t i = 0; i <= End_INS_Point; i++)
    {
        INS_Point[i][1] = 0.0f;
    }
}

// void S_Point_to_Point(uint8_t i)
// {
//     // 使用平面坐标系计算（单位：米）
//     float dx = S_Point_Navigation_Frame[NOW_S_Point][0] - position[0];
//     float dy = S_Point_Navigation_Frame[NOW_S_Point][1] - position[1];

//     // 计算到当前目标点的角度
//     float current_angle = RAD_TO_ANGLE(atan2f(dy, dx));
//     current_angle = current_angle < 0 ? current_angle + 360 : current_angle;
    
//     // 计算欧几里得距离
//     float distance = sqrtf(dx*dx + dy*dy);
    
//     // 如果不是最后一个点，则进行角度混合
//     if (i < End_S_Point) 
//     {
//         Caculate_Next_S_Point_Angle(i);
//         Caculate_Now_S_Point_Distance(i);
//         // 根据距离计算权重，距离越近，下一个点的角度权重越大
//         float weight = 1.0f - (distance / now_s_distance);
//         weight = fmaxf(0.0f, fminf(1.0f, weight));  // 限制在 0-1 范围内
        
//         float angle_diff = next_target_angle - current_angle;
        
//         // 混合角度
//         current_angle = current_angle + angle_diff * weight;
        
//         // 确保角度在 0-360 范围内
//         if (current_angle < 0) current_angle += 360.0f;
//         if (current_angle >= 360.0f) current_angle -= 360.0f;
//     }
//     else 
//     {
//         // 最后一个点，直接使用当前目标点角度
//         current_angle = current_angle;
//     }

//     if (reach_flag != 1)
//     {
//         target_angle = current_angle;
//         Speed_Management(distance);
//     }
//     if (distance < INS_SWITCH_DISTANCE)
//     {
//         reach_flag = 1;
//     }
// }

void S_Point_to_Point(uint8_t i)
{
    // 使用平面坐标系计算（单位：米）
    float dx = S_Point_Navigation_Frame[NOW_S_Point][0] - position[0];
    float dy = S_Point_Navigation_Frame[NOW_S_Point][1] - position[1];
    // 计算到当前目标点的角度
    float current_angle = RAD_TO_ANGLE(atan2f(dy, dx));

    current_angle = current_angle < 0 ? current_angle + 360 : current_angle;
    // 计算欧几里得距离
    float distance = sqrtf(dx*dx + dy*dy);
    if (i < End_S_Point) 
    {
        Caculate_Next_S_Point_Angle(i);
    }
    if (reach_flag != 1)
    {
        target_angle = current_angle;
        if ( i == Start_S_Point || i == Back_INS_Point || i == Back_INS_Point+1 || i == End_S_Point)   // Back_INS_Point- 1 ? End_S_Point
        {
            Speed_Management(distance);
        }
        else
        {
            Speed_Management_For_S(distance);
        }
    }
    if (distance < S_SWITCH_DISTANCE)
    {
        reach_flag = 1;
    }
}

void INS_Point_to_Point(uint8_t i)
{
    // 使用平面坐标系计算（单位：米）
    float dx = INS_Point_Navigation_Frame[i][0] - position[0];
    float dy = INS_Point_Navigation_Frame[i][1] - position[1];

    // 计算平面方位角（0-360度）
    float angle = RAD_TO_ANGLE(atan2f(dy, dx));
    angle = angle < 0 ? angle + 360 : angle;
    
    if (i < End_INS_Point) 
    {
        Caculate_Next_INS_Point_Angle(i);
    }
    // 计算欧几里得距离
    float distance = sqrtf(dx*dx + dy*dy);
    if (reach_flag != 1)
    {
        target_angle = angle;
        if (test_flag != TEST__3) Speed_Management((float)distance);
        else Speed_Management_For_Test3((float)distance, i);
    }
    if (distance < INS_SWITCH_DISTANCE)
    {
        reach_flag = 1;
    }
}

void INS_Navigation(void)
{
    if (NOW_INS_Point > End_INS_Point)
    {
        Brake();
        return;
    }
    if (Start_INS_Point < End_INS_Point)
    {
        if (NOW_S_Point == NOW_INS_Point && NOW_S_Point < End_S_Point && NOW_S_Point != Back_INS_Point) 
        {
            S_Point_to_Point(NOW_S_Point);
            if (reach_flag)
            {
                target_speed = MIN_SPEED;
                target_angle = next_target_angle;
                if (angle_flag)  // 检查角度到达标志位
                {
                    NOW_S_Point++;
                    NOW_INS_Point++;
                    reach_flag = 0;  // 重置标志位
                }
            }
        } 
        else if (NOW_S_Point == Back_INS_Point) 
        {
            INS_Point_to_Point(NOW_INS_Point);
            if (reach_flag)
            {
                target_speed = MIN_SPEED;
                target_angle = next_target_angle;
                if (angle_flag)  // 检查角度到达标志位
                {
                    NOW_S_Point++;
                    NOW_INS_Point++;
                    reach_flag = 0;  // 重置标志位
                }
            }
        }
        else
        {
            INS_Point_to_Point(NOW_INS_Point);
            if (reach_flag)
            {
                target_speed = MIN_SPEED;
                target_angle = next_target_angle;
                if (angle_flag)  // 检查角度到达标志位
                {
                    NOW_INS_Point++;
                    reach_flag = 0;  // 重置标志位
                }
            }
        }
    }
}

void GPS_INS_Navigation(void)
{
    static uint8_t navigation_phase = 0;  // 0:GPS导航阶段  1:INS导航阶段  2:返回GPS导航阶段
    
    // 添加全局停车判断
    if (NOW_GPS_Point > End_GPS_Point && NOW_INS_Point > End_INS_Point)
    {
        Brake();
        return;
    }
    
    switch(navigation_phase)
    {
        case 0:
            if (NOW_GPS_Point <= GPS_TO_INS_Point)
            {
                // 继续使用GPS导航到切换点
                GPS_Point_to_Point(NOW_GPS_Point);
                if (reach_flag)
                {
                    target_speed = MIN_SPEED;
                    if (NOW_GPS_Point == GPS_TO_INS_Point)
                    {
                        // 到达切换点，准备切换到INS导航
                        navigation_phase = 1;
                        NOW_INS_Point = Start_INS_Point;  // 初始化INS起始点
                    }
                    if (angle_flag)
                    {
                        NOW_GPS_Point++;
                        reach_flag = 0;  // 重置标志位
                    }
                }
            }
            else
            {
                Brake();
            }
            break;
            
        case 1:
            if (NOW_INS_Point <= End_INS_Point)
            {
                INS_Point_to_Point(NOW_INS_Point);
                if (reach_flag)
                {
                    target_speed = MIN_SPEED;
                    if (angle_flag)
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
                }
            } else
            {
                Brake();
            }
            break;
            
        case 2:
            if (NOW_GPS_Point <= End_GPS_Point)
            {
                GPS_Point_to_Point(NOW_GPS_Point);
                if (reach_flag)
                {
                    target_speed = MIN_SPEED;
                    if (angle_flag)
                    {
                        // 到达GPS点，继续下一个点
                        NOW_GPS_Point++;
                        reach_flag = 0;  // 重置标志位
                    }
                }
            } else
            {
                Brake();
            }
            break;
    }
}

void GPS_ENU_INS_Navigation(void)
{
    static uint8_t navigation_phase = 0;  // 0:GPS导航阶段  1:INS导航阶段  2:返回GPS导航阶段
    
    // 全局停车条件
    if (NOW_GPS_Point > End_GPS_Point && NOW_INS_Point > End_INS_Point)
    {
        Brake();
        return;
    }
    
    switch(navigation_phase)
    {
        case 0:
            if (NOW_GPS_Point <= GPS_TO_INS_Point)
            {
                // 继续使用GPS导航到切换点
                GPS_ENU_Point_to_Point(NOW_GPS_Point);
                if (reach_flag)
                {
                    target_speed = MIN_SPEED;
                    if (angle_flag)
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
                }
            }
            else
            {
                Brake();
            }
            break;
            
        case 1:
            if (NOW_INS_Point <= End_INS_Point)
            {
                INS_Point_to_Point(NOW_INS_Point);
                if (reach_flag)
                {
                    target_speed = MIN_SPEED;
                    if (angle_flag)
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
                }
            }
            else
            {
                Brake();
            }
            break;
            
        case 2:
            if (NOW_GPS_Point <= End_GPS_Point)
            {
                GPS_ENU_Point_to_Point(NOW_GPS_Point);
                if (reach_flag)
                {
                    target_speed = MIN_SPEED;
                    if (angle_flag)
                    {
                        NOW_GPS_Point++;
                        reach_flag = 0;  // 重置标志位
                        target_speed = MIN_SPEED;
                    }
                }
            }
            else
            {
                Brake();
            }
            break;
    }
}
