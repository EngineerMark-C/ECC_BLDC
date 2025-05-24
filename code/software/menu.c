#include "init.h"

#define MAIN_MENU_ITEMS_COUNT (sizeof(main_menu_items) / sizeof(MainMenuItem))
#define NAV_MODE_COUNT (sizeof(nav_mode_names) / sizeof(nav_mode_names[0]))
#define GPS_INS_PATH_MENU_COUNT (sizeof(gps_ins_path_menu) / sizeof(GPSINSPathMenuItem))

// 定义菜单状态
typedef enum
{
    MENU_MAIN,           // 主菜单状态
    MENU_Calibrate_Gyro, // 陀螺仪校准状态
    MENU_GPS_Point,      // GPS点位显示状态
    MENU_ENU_Point,      // ENU点位显示状态
    MENU_INS_Point,      // INS点位显示状态
    MENU_GPS_PATH,       // 路径设置状态
    MENU_SPEED_MANAGE,   // 速度管理状态
    MENU_GPS_INFO,       // GPS信息显示状态
    MENU_SPEED_IMU,      // 速度和IMU信息显示状态
    MENU_STEER,          // 舵机调节状态
    MENU_NAV_MODE,       // 导航模式选择状态
    MENU_S_Point,        // S型走位显示状态
    MENU_Camera,         // 摄像头显示状态
    MENU_Boundary,       // 边界显示状态
    MENU_PATH,           // 添加路径显示状态
} MenuState;

// 主菜单项定义
typedef struct
{
    const char *name;
} MainMenuItem;

// 舵机菜单项定义
typedef struct
{
    const char *name;
    uint16_t value;
    uint16_t min;
    uint16_t max;
    uint16_t step;
} SteerMenuItem;

// 电机菜单项定义
typedef struct
{
    const char *name;
    float step;
    float *num;
} MotorMenuItem;

// GPS路径设置菜单项定义
typedef struct
{
    const char *name;
    uint8_t *num;
} GPSINSPathMenuItem;

typedef struct
{
    const char *name;
    float *num;
} BoundaryMenuItem;

// 主菜单项
MainMenuItem main_menu_items[] = {
    {"Calibrate Gyro"},
    {"GPS Point"},
    {"ENU Point"},
    {"INS Point"},
    {"GPS Path Setup"},
    {"Speed Manage"},
    {"GPS Info"},
    {"Speed & IMU"},
    {"Steer Control"},
    {"Navigation Mode"},
    {"S Point"},
    {"Camera"},
    {"Boundary"},
    {"Path Display"} // 添加路径显示菜单项
};

// 路径设置菜单项
GPSINSPathMenuItem gps_ins_path_menu[] = {
    {"Start GPS Point", &Start_GPS_Point},
    {"End GPS Point", &End_GPS_Point},
    {"Start INS Point", &Start_INS_Point},
    {"End INS Point", &End_INS_Point},
    {"GPS to INS Point", &GPS_TO_INS_POINT},
    {"Start S Point", &Start_S_Point},
    {"End S Point", &End_S_Point}};

// 边界编辑菜单项
BoundaryMenuItem boundary_menu[] = {
    {"SAFETY_X_MAX", &SAFETY_X_MAX},
    {"SAFETY_X_MIN", &SAFETY_X_MIN},
    {"SAFETY_Y_MAX", &SAFETY_Y_MAX},
    {"SAFETY_Y_MIN", &SAFETY_Y_MIN}};

// 舵机菜单项
SteerMenuItem steer_menu = {
    "Steer PWM",
    PWM_STEER_MIDDLE_R,
    PWM_STEER_MIN_R,
    PWM_STEER_MAX_R,
    5};

// 电机菜单项
MotorMenuItem motor_menu[] = {
    {"MAX_SPEED", 0.1f, &MAX_SPEED},
    {"MIN_SPEED", 0.1f, &MIN_SPEED},
    {"APPROACH_SPEED", 0.1f, &APPROACH_SPEED},
    {"BRAKING_DISTANCE", 0.1f, &BRAKING_DISTANCE},
    {"S_Distance", 0.1f, &S_Distance}};

// 导航模式菜单显示文本数组
const char *nav_mode_names[] = {
    "GPS Navigation",
    "GPS-ENU Navigation",
    "INS Navigation",
    "GPS-INS Navigation",
    "GPS-ENU-INS Navigation"};

// 菜单全局变量
static uint8_t current_item = 0;         // 当前选中的菜单项
static MenuState menu_state = MENU_MAIN; // 当前菜单状态
static bool edit_mode = false;           // 编辑模式标志
static uint8_t start_index = 0;          // 新增：当前显示起始索引
static const uint8_t visible_items = 6;  // 一屏显示6个条目（16px/item）
uint8_t Camera_Choose = 0;               // 摄像头选择
static MenuState last_state = MENU_MAIN; // 记录上次菜单状态
static uint8_t need_clear = 1;           // 清屏标志

static bool edit_coord = false;  // 编辑坐标选择，false=X坐标，true=Y坐标
static float adjust_step = 0.1f; // 默认调整步长

// 添加全局按键状态变量声明
static key_state_enum key1_state;
static key_state_enum key2_state;
static key_state_enum key3_state;
static key_state_enum key4_state;

void Button_Init(void)
{
    key_init(10); // 初始化按键
}

void Update_Adjust_Step(void)
{
    // 读取三个拨码开关的组合状态 (从0到7)
    uint8_t switch_status = (SWITCH_2_STATUS << 1) | (SWITCH_1_STATUS);

    // 根据拨码开关组合选择不同的步长
    switch (switch_status)
    {
    case 0: // 00
        adjust_step = 0.1f;
        break;
    case 1: // 01
        adjust_step = 1.0f;
        break;
    case 2: // 10
        adjust_step = 5.0f;
        break;
    case 3: // 11
        adjust_step = 10.0f;
        break;
    default:
        adjust_step = 0.1f; // 默认步长
        break;
    }
}

void Display_Menu(void)
{
    if (need_clear)
    {
        ips114_clear(); // 仅在需要时清屏
    }
    switch (menu_state)
    {
    case MENU_MAIN:
        Display_Main_Menu();
        break;
    case MENU_STEER:
        Display_Steer_Menu();
        break;
    case MENU_SPEED_IMU:
        Display_Speed_Imu_Info();
        break;
    case MENU_GPS_INFO:
        Display_GPS_Info();
        break;
    case MENU_GPS_Point:
        Display_GPS_Point();
        break;
    case MENU_GPS_PATH:
        Display_GPS_INS_Path();
        break;
    case MENU_SPEED_MANAGE:
        Display_Speed_Manage_Menu();
        break;
    case MENU_Calibrate_Gyro:
        Display_Calibrate_Gyro();
        break;
    case MENU_INS_Point:
        Display_INS_Point();
        break;
    case MENU_ENU_Point:
        Display_ENU_Point();
        break;
    case MENU_NAV_MODE:
        Display_Nav_Mode_Menu();
        break;
    case MENU_S_Point:
        Display_S_Point();
        break;
    case MENU_Camera:
        Display_Camera();
        break;
    case MENU_Boundary:
        Display_Boundary();
        break;
    case MENU_PATH:
        Display_Path();
        break;
    }
}

void Menu(void)
{
    key_scanner(); // 按键扫描

    // 获取所有按键状态
    key1_state = key_get_state(KEY_1); // 上
    key2_state = key_get_state(KEY_2); // 下
    key3_state = key_get_state(KEY_3); // 确认/编辑
    key4_state = key_get_state(KEY_4); // 返回

    if (last_state != menu_state || key1_state || key2_state || key3_state || key4_state)
    {
        ips114_clear(); // 状态变化时清屏
        need_clear = 1;
        last_state = menu_state;
    }
    else
    {
        need_clear = 0;
    }

    switch (menu_state)
    {
    case MENU_MAIN:
        Main_Menu_Key_Process();
        break;
    case MENU_STEER:
        Steer_Menu_Key_Process();
        break;
    case MENU_GPS_Point:
        GPS_Point_Menu_Key_Process();
        break;
    case MENU_GPS_PATH:
        GPS_INS_Path_Menu_Key_Process();
        break;
    case MENU_SPEED_MANAGE:
        Speed_Manage_Menu_Key_Process();
        break;
    case MENU_Calibrate_Gyro:
        Calibrate_Gyro_Menu_Key_Process();
        break;
    case MENU_INS_Point:
        INS_Point_Menu_Key_Process();
        break;
    case MENU_ENU_Point:
        ENU_Point_Menu_Key_Process();
        break;
    case MENU_NAV_MODE:
        Nav_Mode_Key_Process();
        break;
    case MENU_S_Point:
        S_Point_Menu_Key_Process();
        break;
    case MENU_Camera:
        Camera_Menu_Key_Process();
        break;
    case MENU_Boundary:
        Boundary_Menu_Key_Process();
        break;
    case MENU_PATH: // 添加路径显示菜单按键处理
        Path_Menu_Key_Process();
        break;
    case MENU_SPEED_IMU:
    case MENU_GPS_INFO:
        if (key4_state == KEY_SHORT_PRESS)
        {
            menu_state = MENU_MAIN;
            key_clear_state(KEY_4);
        }
        break;
    }
    // 更新显示
    Display_Menu();
}

const char *key_state_to_string(key_state_enum state)
{
    switch (state)
    {
    case KEY_RELEASE:
        return "KEY_RELEASE";
    case KEY_SHORT_PRESS:
        return "KEY_SHORT_PRESS";
    case KEY_LONG_PRESS:
        return "KEY_LONG_PRESS";
    default:
        return "UNKNOWN_STATE";
    }
}

// 显示主菜单
void Display_Main_Menu(void)
{
    // 显示当前可见范围的菜单项（Y轴间隔16像素）
    for (uint8_t i = 0; i < visible_items; i++)
    {
        uint8_t item_num = start_index + i;
        if (item_num >= MAIN_MENU_ITEMS_COUNT)
            break;

        char buffer[32];
        sprintf(buffer, "%s%s",
                (item_num == current_item) ? "> " : "  ",
                main_menu_items[item_num].name);
        ips114_show_string(0, i * 16, buffer);
    }
    // 底部提示信息
    char buffer[32];
    sprintf(buffer, "Page:%02d/%02d", start_index / visible_items + 1,
            (MAIN_MENU_ITEMS_COUNT + visible_items - 1) / visible_items);
    ips114_show_string(0, 112, buffer);
    ips114_show_int(90, 112, Fire_Flag, 2);
}

// 显示舵机调节界面
void Display_Steer_Menu(void)
{
    ips114_show_string(0, 0, "Steer Control");
    char buffer[32];
    sprintf(buffer, "%sPWM: %d",
            edit_mode ? "> " : "  ",
            steer_menu.value);
    ips114_show_string(0, 16, buffer);
    ips114_show_string(0, 48, edit_mode ? "KEY1:+  KEY2:-" : "KEY3:Edit");
    ips114_show_string(0, 64, "Press KEY4 to return");
}

void Display_Speed_Imu_Info(void)
{
    // 速度控制标题与信息
    ips114_show_string(0, 0, "Speed Control");

    // 速度相关数据
    ips114_show_string(0, 16, "Target:");
    ips114_show_float(50, 16, pid_speed.target, 4, 1); // 目标速度
    ips114_show_string(100, 16, "m/s");

    ips114_show_string(0, 32, "Actual:");
    ips114_show_float(50, 32, speed, 4, 1); // 当前速度
    ips114_show_string(100, 32, "m/s");

    ips114_show_string(0, 48, "Error:");
    ips114_show_float(50, 48, pid_speed.error, 4, 1); // 误差
    ips114_show_string(100, 48, "m/s");

    // 输出信息
    ips114_show_string(0, 64, "Out:");
    ips114_show_int(50, 64, output_speed, 5); // 输出值

    // 分割线
    ips114_draw_line(0, 79, 150, 79, RGB565_GRAY);

    // IMU姿态信息
    ips114_show_string(0, 84, "IMU:");

    // 第一行展示pitch和roll
    ips114_show_string(0, 96, "P:");
    ips114_show_float(15, 96, pitch, 5, 1); // pitch

    ips114_show_string(75, 96, "R:");
    ips114_show_float(90, 96, roll, 5, 1); // roll

    // 第二行展示yaw和gyro_x
    ips114_show_string(0, 112, "Y:");
    ips114_show_float(15, 112, yaw, 5, 1); // yaw

    // ips114_show_string(75, 112, "G:");
    // ips114_show_float(90, 112, gx_raw, 5, 0);            // 陀螺仪X轴原始数据
}

// 显示GPS信息
void Display_GPS_Info(void)
{
    // 始终显示标题
    ips114_show_string(0, 0, "GPS:");
    ips114_show_string(40, 0, gnss.state ? "Valid" : "No Fix");

    // 解析GPS数据
    if (0 == gnss_data_parse())
    {
        if (gnss.state)
        {
            // Location data
            ips114_show_string(0, 16, "LON:");
            ips114_show_double(30, 16, gnss.longitude, 4, 8);
            ips114_show_char(130, 16, gnss.ew);

            ips114_show_string(0, 32, "LAT:");
            ips114_show_double(30, 32, gnss.latitude, 4, 8);
            ips114_show_char(130, 32, gnss.ns);

            ips114_show_string(0, 48, "ALT:");
            ips114_show_float(30, 48, gnss.height, 6, 1);
            ips114_show_string(90, 48, "m");

            // Movement data
            ips114_show_string(0, 64, "SPD:");
            ips114_show_float(30, 64, gnss.speed, 5, 1);
            ips114_show_string(90, 64, "km/h");

            ips114_show_string(0, 80, "DIR:");
            ips114_show_float(30, 80, gnss.direction, 5, 1);
            ips114_show_string(90, 80, "deg");

            // System info
            ips114_show_string(0, 96, "SAT:");
            ips114_show_int(30, 96, gnss.satellite_used, 2);

            // Time display
            ips114_show_string(0, 112, "UTC:");
            ips114_show_int(30, 112, gnss.time.hour, 2);
            ips114_show_string(46, 112, ":");
            ips114_show_int(54, 112, gnss.time.minute, 2);
            ips114_show_string(70, 112, ":");
            ips114_show_int(78, 112, gnss.time.second, 2);
        }
        else
        {
            // Show waiting message when GPS is invalid
            ips114_show_string(0, 48, "Waiting for signal...");
        }
        system_delay_ms(50); // Refresh interval
    }
}

// 显示GPS点位管理界面
void Display_GPS_Point(void)
{
    ips114_show_string(0, 0, "GPS Points Management");
    // 显示当前可见范围的点位（Y轴间隔16像素）
    for (uint8_t i = 0; i < visible_items; i++)
    {
        uint8_t point_num = start_index + i;
        if (point_num >= MAX_GPS_POINTS)
            break;

        char point_info[32];
        sprintf(point_info, "%sP%d:%.6f,%.6f",
                (point_num == GPS_Point_Index) ? ">" : " ",
                point_num,
                GPS_Point[point_num][0],
                GPS_Point[point_num][1]);
        ips114_show_string(0, 16 + i * 16, point_info);
    }
    // 底部提示信息
    char buffer[32];
    sprintf(buffer, "Idx:%02d KEY3:Save KEY4:Back", GPS_Point_Index);
    ips114_show_string(0, 112, buffer);
}

// 显示GPS INS路径设置界面
void Display_GPS_INS_Path(void)
{
    ips114_show_string(0, 0, "GPS Path Setting");
    // 显示当前可见范围的菜单项（Y轴间隔16像素）
    for (uint8_t i = 0; i < visible_items; i++)
    {
        uint8_t item_num = start_index + i;
        if (item_num >= GPS_INS_PATH_MENU_COUNT)
            break;

        char buffer[32];
        sprintf(buffer, "%s%s: %d",
                (item_num == current_item) ? "> " : "  ",
                gps_ins_path_menu[item_num].name,
                *gps_ins_path_menu[item_num].num);
        ips114_show_string(0, 16 + i * 16, buffer);
    }
    // 底部提示信息
    ips114_show_string(0, 112, edit_mode ? "KEY1:+  KEY2:-" : "KEY3:Edit KEY4:Back");
}

// 显示速度管理界面
void Display_Speed_Manage_Menu(void)
{
    ips114_show_string(0, 0, "Speed Management");

    for (uint8_t i = 0; i < 5; i++)
    {
        char buffer[32];
        sprintf(buffer, "%s%s: %.1f",
                (i == current_item) ? "> " : "  ",
                motor_menu[i].name,
                *motor_menu[i].num);
        ips114_show_string(0, 16 + i * 16, buffer);
    }
    ips114_show_string(0, 96, edit_mode ? "KEY1:+  KEY2:-" : "KEY3:Edit");
    ips114_show_string(0, 112, "KEY4:Back");
}

// 显示陀螺仪校准界面
void Display_Calibrate_Gyro(void)
{
    ips114_show_string(60, 0, "Gyro Calibrating...");
    ips114_show_string(60, 16, "Keep IMU Still");
}

// 显示INS点位管理界面
void Display_INS_Point(void)
{
    ips114_show_string(0, 0, "INS Points");
    ips114_show_float(90, 0, position[0], 6, 2);
    ips114_show_float(150, 0, position[1], 6, 2);
    ips114_show_float(200, 0, adjust_step, 2, 1);
    // 显示当前可见范围的点位（Y轴间隔16像素）
    for (uint8_t i = 0; i < visible_items; i++)
    {
        uint8_t point_num = start_index + i;
        if (point_num >= MAX_INS_POINTS)
            break;

        char point_info[32];
        sprintf(point_info, "%sP%d:%.3f,%.3f",
                (point_num == INS_Point_Index) ? ">" : " ",
                point_num,
                INS_Point[point_num][0],
                INS_Point[point_num][1]);
        ips114_show_string(0, 16 + i * 16, point_info);
    }

    // 底部提示信息
    if (edit_mode)
    {
        char buffer[32];
        sprintf(buffer, "Edit P%d: %s K1:+ K2:- K3:Switch",
                INS_Point_Index,
                edit_coord ? "Y" : "X");
        ips114_show_string(0, 96, buffer);
        ips114_show_string(0, 112, "K4:Exit Edit");
    }
    else
    {
        char buffer[32];

        // 根据拨码开关状态显示不同的KEY3功能提示
        if (SWITCH_4_STATUS == SWITCH_LEFT)
        {
            sprintf(buffer, "Idx:%02d K3:Edit K4:Back", INS_Point_Index);
        }
        else if (SWITCH_4_STATUS == SWITCH_RIGHT)
        {
            sprintf(buffer, "Idx:%02d K3:Save K4:Back", INS_Point_Index);
        }
        ips114_show_string(0, 112, buffer);
    }
}

// 显示 S型走位 点位管理界面
void Display_S_Point(void)
{
    ips114_show_string(0, 0, "S Points");
    ips114_show_float(200, 0, adjust_step, 2, 1);
    // 显示当前可见范围的点位（Y轴间隔16像素）
    for (uint8_t i = 0; i < visible_items; i++)
    {
        uint8_t point_num = start_index + i;
        if (point_num >= MAX_INS_POINTS)
            break;

        char point_info[32];
        sprintf(point_info, "%sP%d:%.3f,%.3f",
                (point_num == S_Point_Index) ? ">" : " ",
                point_num,
                S_Point[point_num][0],
                S_Point[point_num][1]);
        ips114_show_string(0, 16 + i * 16, point_info);
    }

    // 底部提示信息
    if (edit_mode)
    {
        char buffer[32];
        sprintf(buffer, "Edit P%d: %s K1:+ K2:- K3:Switch",
                S_Point_Index,
                edit_coord ? "Y" : "X");
        ips114_show_string(0, 96, buffer);
        ips114_show_string(0, 112, "K4:Exit Edit");
    }
    else
    {
        char buffer[32];

        // 根据拨码开关状态显示不同的KEY3功能提示
        if (SWITCH_4_STATUS == SWITCH_LEFT)
        {
            sprintf(buffer, "Idx:%02d K3:Edit K4:Back", S_Point_Index);
        }
        else if (SWITCH_4_STATUS == SWITCH_RIGHT)
        {
            sprintf(buffer, "Idx:%02d K3:Generate K4:Back", S_Point_Index);
        }

        ips114_show_string(0, 112, buffer);
    }
}
// 显示ENU点位管理界面
void Display_ENU_Point(void)
{
    ips114_show_string(0, 0, "ENU Points");
    // 显示当前可见范围的点位（Y轴间隔16像素）
    for (uint8_t i = 0; i < visible_items; i++)
    {
        uint8_t point_num = start_index + i;
        if (point_num >= MAX_GPS_POINTS)
            break;

        char point_info[32];
        sprintf(point_info, "%sP%d:%.6f,%.6f",
                (point_num == GPS_Point_Index) ? ">" : " ",
                point_num,
                GPS_ENU[point_num][0],
                GPS_ENU[point_num][1]);
        ips114_show_string(0, 16 + i * 16, point_info);
    }
    // 底部提示信息
    char buffer[32];
    sprintf(buffer, "Idx:%02d KEY3:Save KEY4:Back", GPS_Point_Index);
    ips114_show_string(0, 112, buffer);
}

// 导航模式菜单显示函数
void Display_Nav_Mode_Menu(void)
{
    ips114_show_string(0, 0, "Select Nav Mode");
    // 添加调试输出确认Navigation_Flag的值
    char debug_buf[32];
    sprintf(debug_buf, "Flag:%d ", (uint8_t)Navigation_Flag);
    ips114_show_string(80, 0, debug_buf);

    for (uint8_t i = 0; i < NAV_MODE_COUNT; i++)
    {
        char buffer[32];
        sprintf(buffer, "%s%s",
                (i == (uint8_t)Navigation_Flag) ? "> " : "  ",
                nav_mode_names[i]);
        ips114_show_string(0, 16 + i * 16, buffer);
    }
    // 底部提示信息
    ips114_show_string(0, 112, "KEY3:Select  KEY4:Back");
}

// 摄像头显示函数
void Display_Camera(void)
{
    ips114_show_string(0, 0, "Camera");
    if (mt9v03x_finish_flag)
    {
        if (Camera_Choose == 0)
        {
            ips114_displayimage03x((const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H); // 显示原始图像
        }
        else if (Camera_Choose == 1)
        {
            ips114_show_gray_image(0, 0, (const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H, 240, 135, 64); // 显示灰度图像
        }
        mt9v03x_finish_flag = 0;
    }
    ips114_show_string(0, 112, "Press KEY4:Back");
}

// 边界编辑界面
void Display_Boundary(void)
{
    ips114_show_string(0, 0, "Boundary");
    ips114_show_float(200, 0, adjust_step, 2, 1);
    for (uint8_t i = 0; i < 4; i++)
    {
        char buffer[32];
        sprintf(buffer, "%s%s: %.2f",
                (i == current_item) ? "> " : "  ",
                boundary_menu[i].name,
                *boundary_menu[i].num);
        ips114_show_string(0, 16 + i * 16, buffer);
    }
    // 底部提示信息
    ips114_show_string(0, 96, edit_mode ? "KEY1:+  KEY2:-" : "KEY3:Edit");
    ips114_show_string(0, 112, "KEY4:Back");
}

// 主路径显示函数
void Display_Path(void)
{
    // 获取路径边界
    float min_x, max_x, min_y, max_y;
    Find_Path_Bounds(&min_x, &max_x, &min_y, &max_y);

    // 计算自动缩放(如果需要)
    Calculate_Auto_Zoom(min_x, max_x, min_y, max_y);

    // 绘制坐标系
    Draw_Coordinate_System();

    // 绘制路径线条
    Draw_Path_Lines();

    // 绘制点位标记
    Draw_Points();

    // 绘制界面信息
    Draw_UI_Info();
}

// 主菜单按键处理
void Main_Menu_Key_Process(void)
{
    if (key1_state == KEY_SHORT_PRESS)
    {
        // 修改后的上键循环逻辑
        current_item = (current_item == 0) ? (MAIN_MENU_ITEMS_COUNT - 1) : (current_item - 1);
        // 更新显示起始索引
        if (current_item < start_index)
            start_index = (current_item / visible_items) * visible_items;
        key_clear_state(KEY_1);
    }
    if (key2_state == KEY_SHORT_PRESS)
    {
        // 修改后的下键循环逻辑
        current_item = (current_item == MAIN_MENU_ITEMS_COUNT - 1) ? 0 : (current_item + 1);
        // 更新显示起始索引
        if (current_item >= start_index + visible_items || current_item < start_index)
            start_index = (current_item / visible_items) * visible_items;
        key_clear_state(KEY_2);
    }
    if (key3_state == KEY_SHORT_PRESS)
    {
        // 进入选中的子菜单
        switch (current_item)
        {
        case 0:
            menu_state = MENU_Calibrate_Gyro;
            break;
        case 1:
            menu_state = MENU_GPS_Point;
            start_index = 0;
            break;
        case 2:
            menu_state = MENU_ENU_Point;
            start_index = 0;
            break;
        case 3:
            menu_state = MENU_INS_Point;
            start_index = 0;
            break;
        case 4:
            menu_state = MENU_GPS_PATH;
            break;
        case 5:
            menu_state = MENU_SPEED_MANAGE;
            current_item = 0;
            break;
        case 6:
            menu_state = MENU_GPS_INFO;
            break;
        case 7:
            menu_state = MENU_SPEED_IMU;
            break;
        case 8:
            menu_state = MENU_STEER;
            break;
        case 9:
            menu_state = MENU_NAV_MODE;
            ;
            break;
        case 10:
            menu_state = MENU_S_Point;
            start_index = 0;
            break;
        case 11:
            menu_state = MENU_Camera;
            break;
        case 12:
            menu_state = MENU_Boundary;
            current_item = 0;
            break;
        case 13:
            menu_state = MENU_PATH;
            break;
        }
        key_clear_state(KEY_3);
    }
    if (key4_state == KEY_SHORT_PRESS)
    {
        Fire_Flag = Fire_Flag ? 0 : 1;
        key_clear_state(KEY_4);
    }
}

// 舵机菜单按键处理
void Steer_Menu_Key_Process(void)
{
    if (edit_mode)
    {
        if (key1_state == KEY_SHORT_PRESS)
        {
            if (steer_menu.value < steer_menu.max)
                steer_menu.value += steer_menu.step;
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            if (steer_menu.value > steer_menu.min)
                steer_menu.value -= steer_menu.step;
            key_clear_state(KEY_2);
        }
        if (key3_state == KEY_SHORT_PRESS)
        {
            // 保存舵机PWM值
            Steer_set_duty(steer_menu.value);
            key_clear_state(KEY_3);
        }
        if (key4_state == KEY_SHORT_PRESS)
        {
            menu_state = MENU_MAIN;
            key_clear_state(KEY_4);
        }
    }
    else
    {
        if (key3_state == KEY_SHORT_PRESS)
        {
            edit_mode = true;
            key_clear_state(KEY_3);
        }
        if (key4_state == KEY_SHORT_PRESS)
        {
            menu_state = MENU_MAIN;
            key_clear_state(KEY_4);
        }
    }
}

// GPS点位管理按键处理
void GPS_Point_Menu_Key_Process(void)
{
    if (key1_state == KEY_SHORT_PRESS)
    {
        if (GPS_Point_Index > 0)
        {
            GPS_Point_Index--;
            // 滚动逻辑：当当前索引小于起始索引时调整显示范围
            if (GPS_Point_Index < start_index)
                start_index = GPS_Point_Index;
        }
        key_clear_state(KEY_1);
    }
    if (key2_state == KEY_SHORT_PRESS)
    {
        if (GPS_Point_Index < MAX_GPS_POINTS - 1)
        {
            GPS_Point_Index++;
            // 滚动逻辑：当当前索引超过显示范围时调整显示范围
            if (GPS_Point_Index >= start_index + visible_items)
                start_index = GPS_Point_Index - visible_items + 1;
        }
        key_clear_state(KEY_2);
    }
    if (key3_state == KEY_SHORT_PRESS)
    {
        Save_GPS_Point();
        // 保存后自动跳转到下一个点位并调整显示
        if (GPS_Point_Index < MAX_GPS_POINTS - 1)
        {
            GPS_Point_Index++; // 自动跳到下一个点位
            // 滚动显示逻辑
            if (GPS_Point_Index >= start_index + visible_items)
                start_index = GPS_Point_Index - visible_items + 1;
        }
        // GPS_Point_Index = (GPS_Point_Index + 1) % MAX_GPS_POINTS;
        // // 滚动显示逻辑
        // if(GPS_Point_Index >= start_index + visible_items || GPS_Point_Index < start_index)
        //     start_index = (GPS_Point_Index / visible_items) * visible_items;
        key_clear_state(KEY_3);
    }
    if (key4_state == KEY_SHORT_PRESS)
    {
        menu_state = MENU_MAIN;
        key_clear_state(KEY_4);
    }
}

// GPS路径设置按键处理
void GPS_INS_Path_Menu_Key_Process(void)
{
    if (edit_mode)
    {
        // 编辑模式处理
        if (key1_state == KEY_SHORT_PRESS)
        {
            switch (current_item)
            {
            case 0:
                Start_GPS_Point = (Start_GPS_Point + 1) % MAX_GPS_POINTS;
                break;
            case 1:
                End_GPS_Point = (End_GPS_Point + 1) % MAX_GPS_POINTS;
                break;
            case 2:
                Start_INS_Point = (Start_INS_Point + 1) % MAX_INS_POINTS;
                break;
            case 3:
                End_INS_Point = (End_INS_Point + 1) % MAX_INS_POINTS;
                break;
            case 4:
                GPS_TO_INS_POINT = (GPS_TO_INS_POINT + 1) % MAX_GPS_POINTS;
                break;
            case 5:
                Start_S_Point = (Start_S_Point + 1) % MAX_INS_POINTS;
                break;
            case 6:
                End_S_Point = (End_S_Point + 1) % MAX_INS_POINTS;
                break;
            }
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            switch (current_item)
            {
            case 0:
                Start_GPS_Point = (Start_GPS_Point + MAX_GPS_POINTS - 1) % MAX_GPS_POINTS;
                break;
            case 1:
                End_GPS_Point = (End_GPS_Point + MAX_GPS_POINTS - 1) % MAX_GPS_POINTS;
                break;
            case 2:
                Start_INS_Point = (Start_INS_Point + MAX_INS_POINTS - 1) % MAX_INS_POINTS;
                break;
            case 3:
                End_INS_Point = (End_INS_Point + MAX_INS_POINTS - 1) % MAX_INS_POINTS;
                break;
            case 4:
                GPS_TO_INS_POINT = (GPS_TO_INS_POINT + MAX_GPS_POINTS - 1) % MAX_GPS_POINTS;
                break;
            case 5:
                Start_S_Point = (Start_S_Point + MAX_INS_POINTS - 1) % MAX_INS_POINTS;
                break;
            case 6:
                End_S_Point = (End_S_Point + MAX_INS_POINTS - 1) % MAX_INS_POINTS;
                break;
            }
            key_clear_state(KEY_2);
        }
        if (key3_state == KEY_SHORT_PRESS || key4_state == KEY_SHORT_PRESS)
        {
            edit_mode = false;
            key_clear_state(KEY_3);
            key_clear_state(KEY_4);
        }
    }
    else
    {
        // 非编辑模式处理
        if (key1_state == KEY_SHORT_PRESS)
        {
            if (current_item > 0)
            {
                current_item--;
                if (current_item < start_index)
                    start_index = current_item;
            }
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            if (current_item < sizeof(gps_ins_path_menu) / sizeof(GPSINSPathMenuItem) - 1)
            {
                current_item++;
                if (current_item >= start_index + visible_items)
                    start_index = current_item - visible_items + 1;
            }
            key_clear_state(KEY_2);
        }
        if (key3_state == KEY_SHORT_PRESS)
        {
            edit_mode = true;
            key_clear_state(KEY_3);
        }
        if (key4_state == KEY_SHORT_PRESS)
        {
            menu_state = MENU_MAIN;
            Save_Basic_Data();
            key_clear_state(KEY_4);
        }
    }
}

// 电机调节按键处理
void Speed_Manage_Menu_Key_Process(void)
{
    if (edit_mode)
    {
        if (key1_state == KEY_SHORT_PRESS)
        {
            *motor_menu[current_item].num += motor_menu[current_item].step;
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            *motor_menu[current_item].num -= motor_menu[current_item].step;
            key_clear_state(KEY_2);
        }
        if (key3_state == KEY_SHORT_PRESS || key4_state == KEY_SHORT_PRESS)
        {
            edit_mode = false;
            key_clear_state(KEY_3);
            key_clear_state(KEY_4);
        }
    }
    else
    {
        if (key1_state == KEY_SHORT_PRESS)
        {
            if (current_item > 0)
            {
                current_item--;
            }
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            if (current_item < 4)
            {
                current_item++;
            }
            key_clear_state(KEY_2);
        }
        if (key3_state == KEY_SHORT_PRESS)
        {
            edit_mode = true;
            key_clear_state(KEY_3);
        }
        if (key4_state == KEY_SHORT_PRESS)
        {
            menu_state = MENU_MAIN;
            Save_Basic_Data();
            key_clear_state(KEY_4);
        }
    }
}

void Calibrate_Gyro_Menu_Key_Process(void)
{
    if (key3_state == KEY_SHORT_PRESS)
    {
        Calibrate_Gyro();
        // Calibrate_Acc();
        key_clear_state(KEY_3);
    }
    if (key4_state == KEY_SHORT_PRESS)
    {
        menu_state = MENU_MAIN;
        Save_Basic_Data();
        key_clear_state(KEY_4);
    }
}

void INS_Point_Menu_Key_Process(void)
{
    Update_Adjust_Step();
    if (edit_mode)
    {
        // 编辑模式下的按键处理
        if (key1_state == KEY_SHORT_PRESS)
        {
            // 增加坐标值
            if (!edit_coord)
            {
                INS_Point[INS_Point_Index][0] += adjust_step;
            }
            else
            {
                INS_Point[INS_Point_Index][1] += adjust_step;
            }
            key_clear_state(KEY_1);
        }

        if (key2_state == KEY_SHORT_PRESS)
        {
            // 减少坐标值
            if (!edit_coord)
            {
                INS_Point[INS_Point_Index][0] -= adjust_step;
            }
            else
            {
                INS_Point[INS_Point_Index][1] -= adjust_step;
            }
            key_clear_state(KEY_2);
        }

        if (key3_state == KEY_SHORT_PRESS)
        {
            // 切换编辑X/Y坐标
            edit_coord = !edit_coord;
            key_clear_state(KEY_3);
        }

        if (key4_state == KEY_SHORT_PRESS)
        {
            // 退出编辑模式
            edit_mode = false;
            key_clear_state(KEY_4);
        }
    }
    else
    {
        // 非编辑模式下的按键处理
        if (key1_state == KEY_SHORT_PRESS)
        {
            if (INS_Point_Index > 0)
            {
                INS_Point_Index--;
                // 滚动逻辑：当当前索引小于起始索引时调整显示范围
                if (INS_Point_Index < start_index)
                    start_index = INS_Point_Index;
            }
            key_clear_state(KEY_1);
        }

        if (key2_state == KEY_SHORT_PRESS)
        {
            if (INS_Point_Index < MAX_INS_POINTS - 1)
            {
                INS_Point_Index++;
                // 滚动逻辑：当当前索引超过显示范围时调整显示范围
                if (INS_Point_Index >= start_index + visible_items)
                    start_index = INS_Point_Index - visible_items + 1;
            }
            key_clear_state(KEY_2);
        }

        if (key3_state == KEY_SHORT_PRESS)
        {
            // 根据SWITCH_4的状态决定KEY3的功能
            if (SWITCH_4_STATUS == SWITCH_LEFT)
            {
                // 当拨码开关在左边时，进入编辑模式
                edit_mode = true;
                edit_coord = false; // 默认先编辑X坐标
            }
            else if (SWITCH_4_STATUS == SWITCH_RIGHT)
            {
                // 当拨码开关在右边时，保存当前位置
                Save_INS_Point();
                // 保存后自动跳转到下一个点位并调整显示
                if (INS_Point_Index < MAX_INS_POINTS - 1)
                {
                    INS_Point_Index++; // 自动跳到下一个点位
                    // 滚动显示逻辑
                    if (INS_Point_Index >= start_index + visible_items)
                        start_index = INS_Point_Index - visible_items + 1;
                }
            }
            key_clear_state(KEY_3);
        }

        if (key4_state == KEY_SHORT_PRESS)
        {
            Save_INS_Point();
            menu_state = MENU_MAIN;
            key_clear_state(KEY_4);
        }
    }
}

void S_Point_Menu_Key_Process(void)
{
    Update_Adjust_Step();
    if (edit_mode)
    {
        // 编辑模式下的按键处理
        if (key1_state == KEY_SHORT_PRESS)
        {
            // 增加坐标值
            if (!edit_coord)
            {
                S_Point[S_Point_Index][0] += adjust_step;
            }
            else
            {
                S_Point[S_Point_Index][1] += adjust_step;
            }
            key_clear_state(KEY_1);
        }

        if (key2_state == KEY_SHORT_PRESS)
        {
            // 减少坐标值
            if (!edit_coord)
            {
                S_Point[S_Point_Index][0] -= adjust_step;
            }
            else
            {
                S_Point[S_Point_Index][1] -= adjust_step;
            }
            key_clear_state(KEY_2);
        }

        if (key3_state == KEY_SHORT_PRESS)
        {
            // 切换编辑X/Y坐标
            edit_coord = !edit_coord;
            key_clear_state(KEY_3);
        }

        if (key4_state == KEY_SHORT_PRESS)
        {
            // 退出编辑模式
            edit_mode = false;
            key_clear_state(KEY_4);
        }
    }
    else
    {
        // 非编辑模式下的按键处理
        if (key1_state == KEY_SHORT_PRESS)
        {
            if (S_Point_Index > 0)
            {
                S_Point_Index--;
                // 滚动逻辑：当当前索引小于起始索引时调整显示范围
                if (S_Point_Index < start_index)
                    start_index = S_Point_Index;
            }
            key_clear_state(KEY_1);
        }

        if (key2_state == KEY_SHORT_PRESS)
        {
            if (S_Point_Index < MAX_INS_POINTS - 1)
            {
                S_Point_Index++;
                // 滚动逻辑：当当前索引超过显示范围时调整显示范围
                if (S_Point_Index >= start_index + visible_items)
                    start_index = S_Point_Index - visible_items + 1;
            }
            key_clear_state(KEY_2);
        }

        if (key3_state == KEY_SHORT_PRESS)
        {
            // 根据拨码开关SWITCH_4的状态决定按键3的功能
            if (SWITCH_4_STATUS == SWITCH_RIGHT)
            {
                // 当拨码开关在左边时，生成S点位
                S_Point_Generate_All();
            }
            else if (SWITCH_4_STATUS == SWITCH_LEFT)
            {
                // 当拨码开关在右边时，进入编辑模式
                edit_mode = true;
                edit_coord = false; // 默认先编辑X坐标
            }
            key_clear_state(KEY_3);
        }

        if (key4_state == KEY_SHORT_PRESS)
        {
            menu_state = MENU_MAIN;
            Save_S_Point(); // 保存S点位数据
            key_clear_state(KEY_4);
        }
    }
}

void ENU_Point_Menu_Key_Process(void)
{
    if (key1_state == KEY_SHORT_PRESS)
    {
        if (GPS_Point_Index > 0)
        {
            GPS_Point_Index--;
            // 滚动逻辑：当当前索引小于起始索引时调整显示范围
            if (GPS_Point_Index < start_index)
                start_index = GPS_Point_Index;
        }
        key_clear_state(KEY_1);
    }
    if (key2_state == KEY_SHORT_PRESS)
    {
        if (GPS_Point_Index < MAX_GPS_POINTS - 1)
        {
            GPS_Point_Index++;
            // 滚动逻辑：当当前索引超过显示范围时调整显示范围
            if (GPS_Point_Index >= start_index + visible_items)
                start_index = GPS_Point_Index - visible_items + 1;
        }
        key_clear_state(KEY_2);
    }
    if (key3_state == KEY_SHORT_PRESS)
    {
        Save_GPS_Point();
        // 保存后自动跳转到下一个点位并调整显示
        if (GPS_Point_Index < MAX_GPS_POINTS - 1)
        {
            GPS_Point_Index++; // 自动跳到下一个点位
            // 滚动显示逻辑
            if (GPS_Point_Index >= start_index + visible_items)
                start_index = GPS_Point_Index - visible_items + 1;
        }
        // GPS_Point_Index = (GPS_Point_Index + 1) % MAX_GPS_POINTS;
        // // 滚动显示逻辑
        // if(GPS_Point_Index >= start_index + visible_items || GPS_Point_Index < start_index)
        //     start_index = (GPS_Point_Index / visible_items) * visible_items;
        key_clear_state(KEY_3);
    }
    if (key4_state == KEY_SHORT_PRESS)
    {
        menu_state = MENU_MAIN;
        key_clear_state(KEY_4);
    }
}

// 新增导航模式菜单按键处理函数
void Nav_Mode_Key_Process(void)
{
    if (key1_state == KEY_SHORT_PRESS)
    {
        if ((uint8_t)Navigation_Flag > 0)
        {
            Navigation_Flag = (uint8_t)Navigation_Flag - 1;
        }
        key_clear_state(KEY_1);
    }
    if (key2_state == KEY_SHORT_PRESS)
    {
        if ((uint8_t)Navigation_Flag < NAV_MODE_COUNT - 1)
        {
            Navigation_Flag = (uint8_t)Navigation_Flag + 1;
        }
        else
        {
            Navigation_Flag = 0;
        }
        key_clear_state(KEY_2);
    }
    if (key3_state == KEY_SHORT_PRESS)
    {
        menu_state = MENU_MAIN;
        Save_Basic_Data();
        key_clear_state(KEY_3);
    }
    if (key4_state == KEY_SHORT_PRESS)
    {
        menu_state = MENU_MAIN;
        key_clear_state(KEY_4);
    }
}

// Camera按键处理函数
void Camera_Menu_Key_Process(void)
{
    if (key1_state == KEY_SHORT_PRESS)
    {
        Camera_Choose = Camera_Choose ? 0 : 1;
        key_clear_state(KEY_1);
    }
    if (key4_state == KEY_SHORT_PRESS)
    {
        menu_state = MENU_MAIN;
        key_clear_state(KEY_4);
    }
}

// 编辑边界按键处理函数
void Boundary_Menu_Key_Process(void)
{
    Update_Adjust_Step();
    if (edit_mode)
    {
        if (key1_state == KEY_SHORT_PRESS)
        {
            *boundary_menu[current_item].num += adjust_step;
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            *boundary_menu[current_item].num -= adjust_step;
            key_clear_state(KEY_2);
        }
        if (key3_state == KEY_SHORT_PRESS || key4_state == KEY_SHORT_PRESS)
        {
            edit_mode = false;
            key_clear_state(KEY_3);
            key_clear_state(KEY_4);
        }
    }
    else
    {
        if (key1_state == KEY_SHORT_PRESS)
        {
            if (current_item > 0)
            {
                current_item--;
            }
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            if (current_item < 3)
            {
                current_item++;
            }
            key_clear_state(KEY_2);
        }
        if (key3_state == KEY_SHORT_PRESS)
        {
            edit_mode = true;
            key_clear_state(KEY_3);
        }
        if (key4_state == KEY_SHORT_PRESS)
        {
            menu_state = MENU_MAIN;
            Save_Basic_Data();
            key_clear_state(KEY_4);
        }
    }
}

// 路径显示界面按键处理
void Path_Menu_Key_Process(void)
{

    // 按键1：改变路径类型
    if (key1_state == KEY_SHORT_PRESS)
    {
        path_type = (path_type + 1) % 5; // 循环切换路径类型
        key_clear_state(KEY_1);
    }

    // 按键2：切换显示选项
    if (key2_state == KEY_SHORT_PRESS)
    {
        // 循环切换显示选项: 全部显示 -> 只显示点 -> 只显示线 -> 全部显示
        if (show_points && show_current)
        {
            show_points = 0;
        }
        else if (!show_points && show_current)
        {
            show_current = 0;
            show_points = 1;
        }
        else
        {
            show_points = 1;
            show_current = 1;
        }
        key_clear_state(KEY_2);
    }

    // 按键3：缩放控制
    if (key3_state == KEY_SHORT_PRESS)
    {
        // 增加缩放因子
        zoom_factor *= 1.2f;
        if (zoom_factor > 10.0f)
        {
            zoom_factor = 10.0f;
        }
        key_clear_state(KEY_3);
    }
    else if (key3_state == KEY_LONG_PRESS)
    {
        // 减小缩放因子
        zoom_factor *= 0.8f;
        if (zoom_factor < 0.1f)
        {
            zoom_factor = 0.1f;
        }
        key_clear_state(KEY_3);
    }

    // 按键4：退出路径显示
    if (key4_state == KEY_SHORT_PRESS)
    {
        // 返回主菜单
        menu_state = MENU_MAIN;
        key_clear_state(KEY_4);
    }
}
