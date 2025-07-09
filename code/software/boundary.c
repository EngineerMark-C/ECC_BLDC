#include "init.h"

// 改文件逻辑有待优化

float SAFETY_X_MAX;                                                             // X轴最大安全范围
float SAFETY_X_MIN;                                                             // X轴最小安全范围
float SAFETY_Y_MAX;                                                             // Y轴最大安全范围
float SAFETY_Y_MIN;                                                             // Y轴最小安全范围
float SAFETY_MARGIN;                                                             // 安全边界外扩距离（米）

// 自动计算安全边界结构体
typedef struct {
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    uint8_t is_initialized;
} SafetyBoundary;

SafetyBoundary current_boundary = {0.0f, 0.0f, 0.0f, 0.0f, 0};

// 计算GPS点的安全边界
void Calculate_GPS_Safety_Boundary(void)
{
    if (End_GPS_Point < Start_GPS_Point) return;
    
    // 初始化边界值
    float x_min = GPS_ENU[Start_GPS_Point][0];
    float x_max = GPS_ENU[Start_GPS_Point][0];
    float y_min = GPS_ENU[Start_GPS_Point][1];
    float y_max = GPS_ENU[Start_GPS_Point][1];
    
    // 计算所有GPS点的边界
    for (uint8_t i = Start_GPS_Point; i <= End_GPS_Point; i++) {
        if (GPS_ENU[i][0] < x_min) x_min = GPS_ENU[i][0];
        if (GPS_ENU[i][0] > x_max) x_max = GPS_ENU[i][0];
        if (GPS_ENU[i][1] < y_min) y_min = GPS_ENU[i][1];
        if (GPS_ENU[i][1] > y_max) y_max = GPS_ENU[i][1];
    }
    
    // 应用安全边界外扩
    current_boundary.x_min = x_min - SAFETY_MARGIN;
    current_boundary.x_max = x_max + SAFETY_MARGIN;
    current_boundary.y_min = y_min - SAFETY_MARGIN;
    current_boundary.y_max = y_max + SAFETY_MARGIN;
    current_boundary.is_initialized = 1;
}

// 计算INS点的安全边界
void Calculate_INS_Safety_Boundary(void)
{
    if (End_INS_Point < Start_INS_Point) return;
    
    // 初始化边界值
    float x_min = INS_Point[Start_INS_Point][0];
    float x_max = INS_Point[Start_INS_Point][0];
    float y_min = INS_Point[Start_INS_Point][1];
    float y_max = INS_Point[Start_INS_Point][1];
    
    // 计算所有INS点的边界
    for (uint8_t i = Start_INS_Point; i <= End_INS_Point; i++) {
        if (INS_Point[i][0] < x_min) x_min = INS_Point[i][0];
        if (INS_Point[i][0] > x_max) x_max = INS_Point[i][0];
        if (INS_Point[i][1] < y_min) y_min = INS_Point[i][1];
        if (INS_Point[i][1] > y_max) y_max = INS_Point[i][1];
    }
    
    // 应用安全边界外扩
    current_boundary.x_min = x_min - SAFETY_MARGIN;
    current_boundary.x_max = x_max + SAFETY_MARGIN;
    current_boundary.y_min = y_min - SAFETY_MARGIN;
    current_boundary.y_max = y_max + SAFETY_MARGIN;
    current_boundary.is_initialized = 1;
}

// 计算S型走位的安全边界
void Calculate_S_Safety_Boundary(void)
{
    if (End_S_Point < Start_S_Point) return;
    
    // 初始化边界值
    float x_min = S_Point[Start_S_Point][0];
    float x_max = S_Point[Start_S_Point][0];
    float y_min = S_Point[Start_S_Point][1];
    float y_max = S_Point[Start_S_Point][1];
    
    // 计算所有S型走位点的边界
    for (uint8_t i = Start_S_Point; i <= End_S_Point; i++) {
        if (S_Point[i][0] < x_min) x_min = S_Point[i][0];
        if (S_Point[i][0] > x_max) x_max = S_Point[i][0];
        if (S_Point[i][1] < y_min) y_min = S_Point[i][1];
        if (S_Point[i][1] > y_max) y_max = S_Point[i][1];
    }
    
    // 应用安全边界外扩
    current_boundary.x_min = x_min - SAFETY_MARGIN;
    current_boundary.x_max = x_max + SAFETY_MARGIN;
    current_boundary.y_min = y_min - SAFETY_MARGIN;
    current_boundary.y_max = y_max + SAFETY_MARGIN;
    current_boundary.is_initialized = 1;
}

// 合并多个边界
void Merge_Safety_Boundaries(SafetyBoundary* dest, const SafetyBoundary* src) 
{
    if (!src->is_initialized) return;
    
    if (!dest->is_initialized) {
        *dest = *src;
        return;
    }
    
    // 取并集
    dest->x_min = fminf(dest->x_min, src->x_min);
    dest->x_max = fmaxf(dest->x_max, src->x_max);
    dest->y_min = fminf(dest->y_min, src->y_min);
    dest->y_max = fmaxf(dest->y_max, src->y_max);
}

// 综合计算GPS-INS混合导航的安全边界
void Calculate_GPS_INS_Safety_Boundary(void)
{
    SafetyBoundary gps_boundary = {0.0f, 0.0f, 0.0f, 0.0f, 0};
    SafetyBoundary ins_boundary = {0.0f, 0.0f, 0.0f, 0.0f, 0};
    
    // 计算GPS部分边界（从起点到切换点）
    if (End_GPS_Point >= Start_GPS_Point) {
        float x_min = GPS_ENU[Start_GPS_Point][0];
        float x_max = GPS_ENU[Start_GPS_Point][0];
        float y_min = GPS_ENU[Start_GPS_Point][1];
        float y_max = GPS_ENU[Start_GPS_Point][1];
        
        for (uint8_t i = Start_GPS_Point; i <= End_GPS_Point; i++) {
            if (GPS_ENU[i][0] < x_min) x_min = GPS_ENU[i][0];
            if (GPS_ENU[i][0] > x_max) x_max = GPS_ENU[i][0];
            if (GPS_ENU[i][1] < y_min) y_min = GPS_ENU[i][1];
            if (GPS_ENU[i][1] > y_max) y_max = GPS_ENU[i][1];
        }
        
        gps_boundary.x_min = x_min - SAFETY_MARGIN;
        gps_boundary.x_max = x_max + SAFETY_MARGIN;
        gps_boundary.y_min = y_min - SAFETY_MARGIN;
        gps_boundary.y_max = y_max + SAFETY_MARGIN;
        gps_boundary.is_initialized = 1;
    }
    
    // 计算INS部分边界
    if (End_INS_Point >= Start_INS_Point) {
        float x_min = INS_Point[Start_INS_Point][0];
        float x_max = INS_Point[Start_INS_Point][0];
        float y_min = INS_Point[Start_INS_Point][1];
        float y_max = INS_Point[Start_INS_Point][1];
        
        for (uint8_t i = Start_INS_Point; i <= End_INS_Point; i++) {
            if (INS_Point[i][0] < x_min) x_min = INS_Point[i][0];
            if (INS_Point[i][0] > x_max) x_max = INS_Point[i][0];
            if (INS_Point[i][1] < y_min) y_min = INS_Point[i][1];
            if (INS_Point[i][1] > y_max) y_max = INS_Point[i][1];
        }
        
        ins_boundary.x_min = x_min - SAFETY_MARGIN;
        ins_boundary.x_max = x_max + SAFETY_MARGIN;
        ins_boundary.y_min = y_min - SAFETY_MARGIN;
        ins_boundary.y_max = y_max + SAFETY_MARGIN;
        ins_boundary.is_initialized = 1;
    }
    
    // 计算S型走位的边界并合并
    if (End_S_Point >= Start_S_Point) {
        SafetyBoundary s_boundary = {0.0f, 0.0f, 0.0f, 0.0f, 0};
        float x_min = S_Point[Start_S_Point][0];
        float x_max = S_Point[Start_S_Point][0];
        float y_min = S_Point[Start_S_Point][1];
        float y_max = S_Point[Start_S_Point][1];
        
        for (uint8_t i = Start_S_Point; i <= End_S_Point; i++) {
            if (S_Point[i][0] < x_min) x_min = S_Point[i][0];
            if (S_Point[i][0] > x_max) x_max = S_Point[i][0];
            if (S_Point[i][1] < y_min) y_min = S_Point[i][1];
            if (S_Point[i][1] > y_max) y_max = S_Point[i][1];
        }
        
        s_boundary.x_min = x_min - SAFETY_MARGIN;
        s_boundary.x_max = x_max + SAFETY_MARGIN;
        s_boundary.y_min = y_min - SAFETY_MARGIN;
        s_boundary.y_max = y_max + SAFETY_MARGIN;
        s_boundary.is_initialized = 1;
        
        Merge_Safety_Boundaries(&ins_boundary, &s_boundary);
    }
    
    // 合并所有边界
    current_boundary.is_initialized = 0;
    Merge_Safety_Boundaries(&current_boundary, &gps_boundary);
    Merge_Safety_Boundaries(&current_boundary, &ins_boundary);
    current_boundary.is_initialized = 1;
}

// 根据导航模式选择安全边界计算方法
void Calculate_Safety_Boundary(NavigationFlag flag)
{
    // 重置当前边界状态
    current_boundary.is_initialized = 0;
    
    switch(flag) {
        case GPS_NAV_Flag:
        case GPS_ENU_NAV_Flag:
            Calculate_GPS_Safety_Boundary();
            break;
            
        case INS_NAV_Flag:
            Calculate_INS_Safety_Boundary();
            // 如果有S型走位，也要考虑进去
            if (End_S_Point >= Start_S_Point) {
                SafetyBoundary s_boundary = {0.0f, 0.0f, 0.0f, 0.0f, 0};
                float x_min = S_Point[Start_S_Point][0];
                float x_max = S_Point[Start_S_Point][0];
                float y_min = S_Point[Start_S_Point][1];
                float y_max = S_Point[Start_S_Point][1];
                
                for (uint8_t i = Start_S_Point; i <= End_S_Point; i++) {
                    if (S_Point[i][0] < x_min) x_min = S_Point[i][0];
                    if (S_Point[i][0] > x_max) x_max = S_Point[i][0];
                    if (S_Point[i][1] < y_min) y_min = S_Point[i][1];
                    if (S_Point[i][1] > y_max) y_max = S_Point[i][1];
                }
                
                s_boundary.x_min = x_min - SAFETY_MARGIN;
                s_boundary.x_max = x_max + SAFETY_MARGIN;
                s_boundary.y_min = y_min - SAFETY_MARGIN;
                s_boundary.y_max = y_max + SAFETY_MARGIN;
                s_boundary.is_initialized = 1;
                
                Merge_Safety_Boundaries(&current_boundary, &s_boundary);
            }
            break;
            
        case GPS_INS_NAV_Flag:
        case GPS_ENU_INS_NAV_Flag:
            Calculate_GPS_INS_Safety_Boundary();
            break;
            
        default:
            current_boundary.x_min = -0.0f;
            current_boundary.x_max = 0.0f;
            current_boundary.y_min = -0.0f;
            current_boundary.y_max = 0.0f;
            current_boundary.is_initialized = 1;
            break;
    }
    
    // 更新全局安全边界变量
    SAFETY_X_MIN = current_boundary.x_min;
    SAFETY_X_MAX = current_boundary.x_max;
    SAFETY_Y_MIN = current_boundary.y_min;
    SAFETY_Y_MAX = current_boundary.y_max;
}

// 边界检查函数（更新版）
void Safety_Boundary_Check(void)
{
    // 确保安全边界已初始化
    if (!current_boundary.is_initialized) {
        Calculate_Safety_Boundary(Navigation_Flag);
    }
    
    // 检查东向坐标
    if(position[0] < SAFETY_X_MIN || position[0] > SAFETY_X_MAX) 
    {
        Fire_Flag = 0;
        Brake();
    }
    // 检查北向坐标
    if(position[1] < SAFETY_Y_MIN || position[1] > SAFETY_Y_MAX) 
    {
        Fire_Flag = 0;
        Brake();
    }
}