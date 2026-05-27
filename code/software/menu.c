#include "init.h"

#define NAV_MODE_COUNT (sizeof(nav_mode_names) / sizeof(nav_mode_names[0]))
#define GPS_INS_PATH_MENU_COUNT (sizeof(gps_ins_path_menu) / sizeof(GPSINSPathMenuItem))
#define Motor_MENU_ITEMS_COUNT (sizeof(motor_menu) / sizeof(MotorMenuItem))
#define TEST_MODE_COUNT (sizeof(test_mode_names) / sizeof(test_mode_names[0]))
#define TEST3_MENU_ITEMS_COUNT (sizeof(test3_menu) / sizeof(Test3MenuItem))


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
    MENU_Direction,      // 方向向量采集状态
    MENU_Voice_Led,      // 语音LED状态
    MENU_TEST_MODE,      // 科目模式选择状态
    MENU_TEST3_Element,  // 科目三元素数据编辑状态
} MenuState;

// 主菜单项定义
typedef struct
{
    const char *name;
    uint8_t menu_index;
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
    float *num;
} MotorMenuItem;

// GPS路径设置菜单项定义
typedef struct
{
    const char *name;
    uint8_t *num;
} GPSINSPathMenuItem;

// 边界编辑菜单项定义
typedef struct
{
    const char *name;
    float *num;
} BoundaryMenuItem;

// 科目三菜单项定义
typedef struct
{
    const char *name;
    uint8_t *index;
    float *speed;
} Test3MenuItem;

// 全部菜单项（NO_TEST模式）
MainMenuItem all_menu_items[] = {
    {"Calibrate Gyro", 1},
    {"GPS Point", 2},
    {"ENU Point", 3},
    {"INS Point", 4},
    {"GPS Path Setup", 5},
    {"Speed Manage", 6},
    {"GPS Info", 7},
    {"Speed & IMU", 8},
    {"Steer Control", 9},
    {"Navigation Mode", 10},
    {"S Point", 11},
    {"Camera", 12},
    {"Boundary", 13},
    {"Path Display", 14},
    {"Direction Vector", 15},
    {"Voice LED", 16},
    {"Test Mode", 17}
};

// TEST__1模式菜单项
MainMenuItem test1_menu_items[] = {
    {"Calibrate Gyro", 1},
    {"GPS Point", 2},
    {"ENU Point", 3},
    {"INS Point", 4},
    {"GPS Path Setup", 5},
    {"Speed Manage", 6},
    {"GPS Info", 7},
    {"Speed & IMU", 8},
    {"Navigation Mode", 10},
    {"Boundary", 13},
    {"Path Display", 14},
    {"Direction Vector", 15},
    {"Test Mode", 17}
};

// TEST__2模式菜单项
MainMenuItem test2_menu_items[] = {
    {"Calibrate Gyro", 1},
    {"GPS Point", 2},
    {"ENU Point", 3},
    {"INS Point", 4},
    {"GPS Path Setup", 5},
    {"Speed Manage", 6},
    {"GPS Info", 7},
    {"Speed & IMU", 8},
    {"Navigation Mode", 10},
    {"S Point", 11},
    {"Boundary", 13},
    {"Path Display", 14},
    {"Direction Vector", 15},
    {"Test Mode", 17}
};

// TEST__3模式菜单项
MainMenuItem test3_menu_items[] = {
    {"Calibrate Gyro", 1},
    {"GPS Point", 2},
    {"ENU Point", 3},
    {"INS Point", 4},
    {"GPS Path Setup", 5},
    {"Test3 Element", 18},
    {"Speed Manage", 6},
    {"GPS Info", 7},
    {"Speed & IMU", 8},
    {"Navigation Mode", 10},
    {"Camera", 12},
    {"Boundary", 13},
    {"Path Display", 14},
    {"Direction Vector", 15},
    {"Test Mode", 17}
};

// TEST__4模式菜单项
MainMenuItem test4_menu_items[] = {
    {"Calibrate Gyro", 1},
    {"Voice LED", 16},
    {"GPS Point", 2},
    {"ENU Point", 3},
    {"INS Point", 4},
    {"GPS Path Setup", 5},
    {"Speed Manage", 6},
    {"GPS Info", 7},
    {"Speed & IMU", 8},
    {"Navigation Mode", 10},
    {"Camera", 12},
    {"Boundary", 13},
    {"Path Display", 14},
    {"Direction Vector", 15},
    {"Test Mode", 17}
};

// 路径设置菜单项
GPSINSPathMenuItem gps_ins_path_menu[] = {
    {"Start GPS Point", &Start_GPS_Point},
    {"End GPS Point", &End_GPS_Point},
    {"Start INS Point", &Start_INS_Point},
    {"End INS Point", &End_INS_Point},
    {"Back INS Point", &Back_INS_Point},
    {"GPS to INS Point", &GPS_TO_INS_Point},
    {"Start S Point", &Start_S_Point},
    {"End S Point", &End_S_Point}
};

// 边界编辑菜单项
BoundaryMenuItem boundary_menu[] = {
    {"SAFETY_MARGIN_X", &SAFETY_MARGIN_X},
    {"SAFETY_MARGIN_Y", &SAFETY_MARGIN_Y},
    {"SAFETY_X_MAX", &SAFETY_X_MAX},
    {"SAFETY_X_MIN", &SAFETY_X_MIN},
    {"SAFETY_Y_MAX", &SAFETY_Y_MAX},
    {"SAFETY_Y_MIN", &SAFETY_Y_MIN}
};

// 舵机菜单项
SteerMenuItem steer_menu = {
    "Steer PWM",
    PWM_STEER_MIDDLE_R,
    PWM_STEER_MIN_R,
    PWM_STEER_MAX_R,
    5};

// 电机菜单项
MotorMenuItem motor_menu[] = {
    {"MAX_SPEED", &MAX_SPEED},
    {"MIN_SPEED", &MIN_SPEED},
    {"BRAKING_DISTANCE", &BRAKING_DISTANCE},
    {"S_MAX_SPEED", &S_MAX_SPEED},
    {"S_MIN_SPEED", &S_MIN_SPEED},
    {"S_BRAKING_DISTANCE", &S_BRAKING_DISTANCE},
    {"S_Distance", &S_Distance},
    {"GPS_SWITCH_DISTANCE", &GPS_SWITCH_DISTANCE},
    {"INS_SWITCH_DISTANCE", &INS_SWITCH_DISTANCE},
    {"S_SWITCH_DISTANCE", &S_SWITCH_DISTANCE},
    {"GO_ACCEL_DISTANCE", &GO_ACCELERATION_DISTANCE},
    {"BK_ACCEL_DISTANCE", &BACK_ACCELERATION_DISTANCE},
    {"TURN_ANGLE", &turn_angle}
};

// 导航模式菜单显示文本数组
const char *nav_mode_names[] = {
    "GPS Navigation",
    "GPS-ENU Navigation",
    "INS Navigation",
    "GPS-INS Navigation",
    "GPS-ENU-INS Navigation"
};

// 科目三元素数据
Test3MenuItem test3_menu[] = {
    {"Ramp Point", &Test3Element[0].Point_Index, &Test3Element[0].Through_Speed},
    {"Grass Point", &Test3Element[1].Point_Index, &Test3Element[1].Through_Speed},
    {"Bump Point", &Test3Element[2].Point_Index, &Test3Element[2].Through_Speed},
    {"Narrow Point", &Test3Element[3].Point_Index, &Test3Element[3].Through_Speed}
};

// 科目模式名称数组
const char *test_mode_names[] = {
    "No Test",
    "Test 1",
    "Test 2", 
    "Test 3",
    "Test 4"
};

// 当前使用的菜单项指针
MainMenuItem *main_menu_items = all_menu_items;

// 菜单全局变量
static uint8_t current_item = 0;         // 当前选中的菜单项
static MenuState menu_state = MENU_MAIN; // 当前菜单状态
static bool edit_mode = false;           // 编辑模式标志
static uint8_t start_index = 0;          // 新增：当前显示起始索引
static const uint8_t visible_items = 6;  // 一屏显示6个条目（16px/item）
uint8_t Camera_Choose = 0;               // 摄像头选择
static MenuState last_state = MENU_MAIN; // 记录上次菜单状态
static uint8_t need_clear = 1;           // 清屏标志
static uint8_t current_menu_count = 0;   // 当前菜单项数量

static bool edit_coord = true;   // 编辑坐标选择，false=X坐标，true=Y坐标
static float adjust_step = 0.1f; // 默认调整步长
static uint8_t ins_display_mode = 0;  // 0: INS_Point, 1: INS_Point_Navigation_Frame
static uint8_t s_display_mode = 0;  // 0: S_Point, 1: S_Point_Navigation_Frame

// 添加全局按键状态变量声明
static key_state_enum key1_state;
static key_state_enum key2_state;
static key_state_enum key3_state;
static key_state_enum key4_state;
static key_state_enum key5_state;

// 根据科目模式更新菜单配置
void Update_Menu_For_Test_Mode(void)
{
    switch(test_flag)
    {
        case NO_TEST:
            main_menu_items = all_menu_items;
            current_menu_count = sizeof(all_menu_items) / sizeof(MainMenuItem);
            break;
        case TEST__1:
            main_menu_items = test1_menu_items;
            current_menu_count = sizeof(test1_menu_items) / sizeof(MainMenuItem);
            break;
        case TEST__2:
            main_menu_items = test2_menu_items;
            current_menu_count = sizeof(test2_menu_items) / sizeof(MainMenuItem);
            break;
        case TEST__3:
            main_menu_items = test3_menu_items;
            current_menu_count = sizeof(test3_menu_items) / sizeof(MainMenuItem);
            break;
        case TEST__4:
            main_menu_items = test4_menu_items;
            current_menu_count = sizeof(test4_menu_items) / sizeof(MainMenuItem);
            break;
        default:
            main_menu_items = all_menu_items;
            current_menu_count = sizeof(all_menu_items) / sizeof(MainMenuItem);
            break;
    }
    
    // 重置菜单索引，防止越界
    current_item = 0;
    start_index = 0;
}

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
        adjust_step = 0.01f;
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
    case MENU_Direction:
        Display_Direction();
        break;
    case MENU_Voice_Led:
        Display_Voice_Led();
        break;
    case MENU_TEST_MODE:
        Display_Test_Mode_Menu();
        break;
    case MENU_TEST3_Element:
        Display_Test3_Element_Menu();
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
    key5_state = key_get_state(KEY_5); // 新增按键

    if (last_state != menu_state || key1_state || key2_state || key3_state || key4_state || key5_state)
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
    case MENU_PATH:
        Path_Menu_Key_Process();
        break;
    case MENU_Direction:
        Direction_Menu_Key_Process();
        break;
    case MENU_Voice_Led:
        Voice_Led_Menu_Key_Process();
        break;
    case MENU_TEST_MODE:
        Test_Mode_Key_Process();
        break;
    case MENU_TEST3_Element:
        Test3_Element_Menu_Key_Process();
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
        if (item_num >= current_menu_count)
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
            (current_menu_count + visible_items - 1) / visible_items);
    ips114_show_string(0, 112, buffer);
    ips114_show_int(200, 112, Fire_Flag, 2);
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
    ips114_show_float(200, 0, adjust_step, 2, 1);
    
    // 显示当前可见范围的菜单项（Y轴间隔16像素）
    for (uint8_t i = 0; i < visible_items; i++)
    {
        uint8_t item_num = start_index + i;
        if (item_num >= Motor_MENU_ITEMS_COUNT)
            break;

        char buffer[32];
        sprintf(buffer, "%s%s: %.1f",
                (item_num == current_item) ? "> " : "  ",
                motor_menu[item_num].name,
                *motor_menu[item_num].num);
        ips114_show_string(0, 16 + i * 16, buffer);
    }
    
    // 底部提示信息
    ips114_show_string(0, 112, edit_mode ? "KEY1:+  KEY2:-" : "KEY3:Edit KEY4:Back");
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
    // 根据显示模式显示不同的标题
    if (ins_display_mode == 0)
    {
        ips114_show_string(0, 0, "INS Points");
    }
    else
    {
        ips114_show_string(0, 0, "INS Nav Frame");
    }
    
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
        
        // 根据显示模式选择不同的数据源
        if (ins_display_mode == 0)
        {
            sprintf(point_info, "%sP%d:%.3f,%.3f",
                    (point_num == INS_Point_Index) ? ">" : " ",
                    point_num,
                    INS_Point[point_num][0],
                    INS_Point[point_num][1]);
        }
        else
        {
            sprintf(point_info, "%sP%d:%.3f,%.3f",
                    (point_num == INS_Point_Index) ? ">" : " ",
                    point_num,
                    INS_Point_Navigation_Frame[point_num][0],
                    INS_Point_Navigation_Frame[point_num][1]);
        }
        
        ips114_show_string(0, 16 + i * 16, point_info);
    }

    // 底部提示信息
    if (edit_mode)
    {
        char buffer[32];
        sprintf(buffer, "P%d: %s K1:+ K2:- K3:Switch",
                INS_Point_Index,
                edit_coord ? "Y" : "X");
        ips114_show_string(0, 112, buffer);
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
            if (SWITCH_1_STATUS == SWITCH_RIGHT)
            {
                sprintf(buffer, "Idx:%02d K3:Mirror K5:Y0", INS_Point_Index);
            }
            else
            {
                sprintf(buffer, "Idx:%02d K3:Save K4:Back", INS_Point_Index);
            }
        }
        ips114_show_string(0, 112, buffer);
    }
}

// 显示 S型走位 点位管理界面
void Display_S_Point(void)
{
    // 根据显示模式显示不同的标题
    if (s_display_mode == 0)
    {
        ips114_show_string(0, 0, "S Points");
    }
    else
    {
        ips114_show_string(0, 0, "S Nav Frame");
    }

    ips114_show_float(200, 0, adjust_step, 2, 1);
    
    // 显示当前可见范围的点位（Y轴间隔16像素）
    for (uint8_t i = 0; i < visible_items; i++)
    {
        uint8_t point_num = start_index + i;
        if (point_num >= MAX_INS_POINTS)
            break;

        char point_info[32];
        
        // 根据显示模式选择不同的数据源
        if (s_display_mode == 0)
        {
            sprintf(point_info, "%sP%d:%.3f,%.3f",
                    (point_num == S_Point_Index) ? ">" : " ",
                    point_num,
                    S_Point[point_num][0],
                    S_Point[point_num][1]);
        }
        else
        {
            sprintf(point_info, "%sP%d:%.3f,%.3f",
                    (point_num == S_Point_Index) ? ">" : " ",
                    point_num,
                    S_Point_Navigation_Frame[point_num][0],
                    S_Point_Navigation_Frame[point_num][1]);
        }
        
        ips114_show_string(0, 16 + i * 16, point_info);
    }

    // 底部提示信息
    if (edit_mode)
    {
        char buffer[32];
        sprintf(buffer, "P%d: %s K1:+ K2:- K3:Switch",
                S_Point_Index,
                edit_coord ? "Y" : "X");
        ips114_show_string(0, 112, buffer);
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
    if (SWITCH_1_STATUS == SWITCH_RIGHT)
    {
        sprintf(buffer, "Idx:%02d KEY3:Correct KEY5:2INS", GPS_Point_Index);
    }
    else
    {
        sprintf(buffer, "Idx:%02d KEY3:Save KEY4:Back", GPS_Point_Index);
    }
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
    ips114_show_float(200, 0, adjust_step, 2, 1);

    if (mt9v03x_finish_flag)
    {
        if (Camera_Choose == 0)
        {
            ips114_displayimage03x((const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H); // 显示原始图像
        }
        else if (Camera_Choose == 1)
        {
            // ips114_show_gray_image(0, 0, (const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H, 188, 120, Camera_Threshold); // 显示二值化图像
            Camera_Show_Line_Detection(0, 0, MT9V03X_W, MT9V03X_H, Camera_Threshold);
        }
        mt9v03x_finish_flag = 0;
    }
    // 显示相机类型和阈值
    char buffer[32];
    if (Camera_Choose == 0)
    {
        sprintf(buffer, "Mode:%s Expos:%d","Normal",
            Camera_Exposure);
    }
    else
    {
        sprintf(buffer, "Mode:%s Thres:%d","Binary",
            Camera_Threshold);
    }
    ips114_show_string(0, 112, buffer);

    ips114_show_string(200, 16, Camera_Is_Line_Detected() ? "Yes" : "No");

    ips114_show_int(200, 32, Camera_Get_Line_Direction(), 2);
    // 显示方向含义
    char dir_text[1];
    int8_t direction = Camera_Get_Line_Direction();
    if (direction == -1)
        sprintf(dir_text, "L");
    else if (direction == 0)
        sprintf(dir_text, "C");
    else if (direction == 1)
        sprintf(dir_text, "R");
    else
        sprintf(dir_text, "N");
    ips114_show_string(200, 48, dir_text);

    if (Camera_Is_Line_Detected())
    {

        ips114_show_int(200, 64, Camera_Get_Line_Position(), 3);
        
        ips114_show_int(200, 80, MT9V03X_W / 2, 3);

        ips114_show_int(200, 96, Camera_Get_Line_Offset(), 3);

        ips114_show_int(200, 112,(int8_t)angle_adjustment, 3);
    }
    else
    {
        ips114_show_string(200, 64, "Pos");
        ips114_show_string(200, 80, "Mid");
        ips114_show_string(200, 96, "Off");
        ips114_show_string(200, 112, "Ang");
    }
}

// 边界编辑界面
void Display_Boundary(void)
{
    ips114_show_string(0, 0, "Boundary");
    ips114_show_float(200, 0, adjust_step, 2, 1);
    
    // 显示前两个可编辑的安全边界外扩距离
    for (uint8_t i = 0; i < 2; i++)
    {
        char buffer[32];
        sprintf(buffer, "%s%s: %.2f",
                (i == current_item) ? "> " : "  ",
                boundary_menu[i].name,
                *boundary_menu[i].num);
        ips114_show_string(0, 16 + i * 16, buffer);
    }
    
    // 显示只读的边界值
    for (uint8_t i = 2; i < 6; i++)
    {
        char buffer[32];
        sprintf(buffer, "  %s: %.2f",
                boundary_menu[i].name,
                *boundary_menu[i].num);
        ips114_show_string(0, 16 + i * 16, buffer);
    }
    
    ips114_show_string(0, 112, "K3:Switch K4:Back K5:Update");
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

// 显示方向向量采集界面
void Display_Direction(void)
{
    ips114_show_string(0, 0, "Direction Setup");
    ips114_show_int(200, 0, yaw_flag, 1);
    
    // 显示当前方向角度
    ips114_show_string(0, 16, "Current Direction:");
    ips114_show_float(150, 16, Start_Direction, 3, 2);
    ips114_show_string(200, 16, "deg");

     // 显示方向点位信息num
    for (uint8_t i = 0; i < 2; i++)
    {
        char point_info[32];
        sprintf(point_info, "%sP%d: %.6f, %.6f",
                (i == Direction_Point_Index) ? ">" : " ",
                i,
                Direction_Point[i][0],
                Direction_Point[i][1]);
         ips114_show_string(0, 32 + i * 16, point_info);
    }
    
    // 显示GPS状态
    ips114_show_string(0, 64, "GPS Status:");
    ips114_show_string(100, 64, gnss.state ? "Valid" : "No Fix");
    
    // 显示当前GPS位置
    if (gnss.state)
    {
        ips114_show_double(0, 80, NOW_location.latitude, 4, 8);
        ips114_show_double(0, 96, NOW_location.longitude, 4, 8);
    }
    
    // 底部提示信息
    char buffer[32];
    sprintf(buffer, "Index:%d KEY3:Save KEY4:Back", Direction_Point_Index);
    ips114_show_string(0, 112, buffer);
}

//语音识别
void Display_Voice_Led(void)
{
    ips114_show_string(0, 0, "Voice Recognition");
    
    // 显示语音识别状态
    ips114_show_string(0, 16, "Status:");
    if (audio_start_flag)
    {
        ips114_show_string(50, 16, "Recording...");
    }
    else if (voice_recognition_flag)
    {
        ips114_show_string(50, 16, "Processing");
    }
    else
    {
        ips114_show_string(50, 16, "Ready");
    }
    
    // 显示连接状态
    ips114_show_string(0, 32, "Server:");
    ips114_show_string(50, 32, audio_server_link_flag ? "Connected" : "Disconnected");
    
    // 显示语音命令执行状态
    ips114_show_string(0, 48, "Command:");
    ips114_show_string(50, 48, command_complete_flag ? "Ready" : "Executing");
    
    // 显示已识别命令数量
    ips114_show_string(0, 64, "Commands:");
    ips114_show_int(70, 64, command_buffer_index, 2);
    
    // 显示最大录音时间进度
    if (audio_start_flag && asr_max_time > 0)
    {
        uint8_t progress = (uint8_t)((asr_max_time * 100) / (60 * 8000));
        ips114_show_string(0, 80, "Progress:");
        ips114_show_int(70, 80, progress, 3);
        ips114_show_string(100, 80, "%");
    }
    
    // 底部提示信息
    if (audio_start_flag)
    {
        ips114_show_string(0, 96, "Recording... K3:Stop");
        ips114_show_string(0, 112, "");
    }
    else
    {
        ips114_show_string(0, 96, "K1:Init K2:Clear K3:Start");
        ips114_show_string(0, 112, "K4:Back K5:Execute");
    }
}

void Display_Test_Mode_Menu(void)
{
    ips114_show_string(0, 0, "Select Test Mode");
    
    // 显示当前test_flag值
    char buf[32];
    sprintf(buf, "Current: %d", (uint8_t)test_flag);
    ips114_show_string(120, 0, buf);

    // 显示所有科目模式选项
    for (uint8_t i = 0; i < TEST_MODE_COUNT; i++)
    {
        char buffer[32];
        sprintf(buffer, "%s%s",
                (i == (uint8_t)test_flag) ? "> " : "  ",
                test_mode_names[i]);
        ips114_show_string(0, 16 + i * 16, buffer);
    }
    
    // 底部提示信息
    ips114_show_string(0, 96, "KEY1:Up  KEY2:Down");
    ips114_show_string(0, 112, "KEY3:Select  KEY4:Back");
}

void Display_Test3_Element_Menu(void)
{
    ips114_show_string(0, 0, "Test3 Element");
    ips114_show_float(200, 0, adjust_step, 2, 1);
    
    // 显示当前可见范围的菜单项（Y轴间隔16像素）
    for (uint8_t i = 0; i < visible_items && i < TEST3_MENU_ITEMS_COUNT; i++)
    {
        uint8_t item_num = start_index + i;
        if (item_num >= TEST3_MENU_ITEMS_COUNT)
            break;

        char buffer[32];
        sprintf(buffer, "%s%s: %d, Speed: %.1f",
                (item_num == current_item) ? "> " : "  ",
                test3_menu[item_num].name,
                *test3_menu[item_num].index,
                *test3_menu[item_num].speed);
        ips114_show_string(0, 16 + i * 16, buffer);
    }
    
    // 底部提示信息
    if (edit_mode)
    {
        ips114_show_string(0, 96, "K1:+ K2:- K3:Switch K4:Exit");
        char edit_info[32];
        sprintf(edit_info, "Editing: %s", edit_coord ? "Index" : "Speed");
        ips114_show_string(0, 112, edit_info);
    }
    else
    {
        ips114_show_string(0, 112, "K1:+ K2:- K3:Edit K4:Exit");
    }
}

// 主菜单按键处理
void Main_Menu_Key_Process(void)
{
    if (key1_state == KEY_SHORT_PRESS)
    {
        // 修改后的上键循环逻辑
        current_item = (current_item == 0) ? (current_menu_count - 1) : (current_item - 1);
        // 更新显示起始索引
        if (current_item < start_index)
            start_index = (current_item / visible_items) * visible_items;
        key_clear_state(KEY_1);
    }
    if (key2_state == KEY_SHORT_PRESS)
    {
        // 修改后的下键循环逻辑
        current_item = (current_item == current_menu_count - 1) ? 0 : (current_item + 1);
        // 更新显示起始索引
        if (current_item >= start_index + visible_items || current_item < start_index)
            start_index = (current_item / visible_items) * visible_items;
        key_clear_state(KEY_2);
    }
    if (key3_state == KEY_SHORT_PRESS)
    {
        // 根据menu_index进入对应的子菜单
        uint8_t menu_index = main_menu_items[current_item].menu_index;
        switch (menu_index)
        {
        case 1:
            menu_state = MENU_Calibrate_Gyro;
            break;
        case 2:
            menu_state = MENU_GPS_Point;
            start_index = 0;
            break;
        case 3:
            menu_state = MENU_ENU_Point;
            start_index = 0;
            break;
        case 4:
            menu_state = MENU_INS_Point;
            start_index = 0;
            break;
        case 5:
            menu_state = MENU_GPS_PATH;
            break;
        case 6:
            menu_state = MENU_SPEED_MANAGE;
            start_index = 0;
            break;
        case 7:
            menu_state = MENU_GPS_INFO;
            break;
        case 8:
            menu_state = MENU_SPEED_IMU;
            break;
        case 9:
            menu_state = MENU_STEER;
            break;
        case 10:
            menu_state = MENU_NAV_MODE;
            break;
        case 11:
            menu_state = MENU_S_Point;
            start_index = 0;
            break;
        case 12:
            menu_state = MENU_Camera;
            break;
        case 13:
            menu_state = MENU_Boundary;
            break;
        case 14:
            menu_state = MENU_PATH;
            break;
        case 15:
            menu_state = MENU_Direction;
            break;
        case 16:
            menu_state = MENU_Voice_Led;
            break;
        case 17:
            menu_state = MENU_TEST_MODE;
            break;
        case 18:
            menu_state = MENU_TEST3_Element;
            break;
        }
        key_clear_state(KEY_3);
    }
    if (key5_state == KEY_SHORT_PRESS)
    {
        if (Fire_Flag == 0)
        {
            system_delay_ms(2000);
        }
        Fire_Flag = Fire_Flag ? 0 : 1;
        key_clear_state(KEY_5);
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
            Steer_set_duty(steer_menu.value);
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            if (steer_menu.value > steer_menu.min)
                steer_menu.value -= steer_menu.step;
            Steer_set_duty(steer_menu.value);
            key_clear_state(KEY_2);
        }
        if (key3_state == KEY_SHORT_PRESS)
        {
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
                Start_GPS_Point = Start_GPS_Point ? Start_GPS_Point : 1;        // 确保起始点不为0
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
                Back_INS_Point = (Back_INS_Point + 1) % MAX_INS_POINTS;
                break;
            case 5:
                GPS_TO_INS_Point = (GPS_TO_INS_Point + 1) % MAX_INS_POINTS;
                break;
            case 6:
                Start_S_Point = (Start_S_Point + 1) % MAX_INS_POINTS;
                break;
            case 7:
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
                Back_INS_Point = (Back_INS_Point + MAX_INS_POINTS - 1) % MAX_INS_POINTS;
                break;
            case 5:
                GPS_TO_INS_Point = (GPS_TO_INS_Point + MAX_GPS_POINTS - 1) % MAX_GPS_POINTS;
                break;
            case 6:
                Start_S_Point = (Start_S_Point + MAX_INS_POINTS - 1) % MAX_INS_POINTS;
                break;
            case 7:
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
            Save_Test_Data();
            key_clear_state(KEY_4);
        }
    }
}

// 电机调节按键处理
void Speed_Manage_Menu_Key_Process(void)
{
    Update_Adjust_Step();

    if (edit_mode)
    {
        if (key1_state == KEY_SHORT_PRESS)
        {
            *motor_menu[current_item].num += adjust_step;
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            *motor_menu[current_item].num -= adjust_step;
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
                // 添加滚动逻辑：当当前索引小于起始索引时调整显示范围
                if (current_item < start_index)
                    start_index = current_item;
            }
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            if (current_item < Motor_MENU_ITEMS_COUNT - 1)
            {
                current_item++;
                // 添加滚动逻辑：当当前索引超过显示范围时调整显示范围
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
            Save_Test_Data();
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
            // 根据SWITCH_4和SWITCH_1的状态决定KEY3的功能
            if (SWITCH_4_STATUS == SWITCH_LEFT)
            {
                // 当拨码开关4在左边时，进入编辑模式
                edit_mode = true;
                edit_coord = true; // 默认先编辑X坐标
            }
            else if (SWITCH_4_STATUS == SWITCH_RIGHT)
            {
                // 当拨码开关4在右边时
                if (SWITCH_1_STATUS == SWITCH_RIGHT)
                {
                    Mirror_INS_Point_Generate();
                }
                else
                {
                    // 当拨码开关1在左边时，保存当前位置
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
            }
            key_clear_state(KEY_3);
        }

        if (key4_state == KEY_SHORT_PRESS)
        {
            Save_INS_Point_Memory();
            menu_state = MENU_MAIN;
            key_clear_state(KEY_4);
        }

        if (key5_state == KEY_SHORT_PRESS)
        {
            if (SWITCH_4_STATUS == SWITCH_RIGHT && SWITCH_1_STATUS == SWITCH_RIGHT)
            {
                INS_Point_Y_Zero();
            }
            else
            {
            ins_display_mode = ins_display_mode ? 0 : 1;  // 切换显示模式
            Vehicle_To_Navigation_INS();
            }
            key_clear_state(KEY_5);
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
                edit_coord = true; // 默认先编辑Y坐标
            }
            key_clear_state(KEY_3);
        }

        if (key4_state == KEY_SHORT_PRESS)
        {
            menu_state = MENU_MAIN;
            Save_S_Point(); // 保存S点位数据
            key_clear_state(KEY_4);
        }
        if (key5_state == KEY_SHORT_PRESS)
        {
            s_display_mode = s_display_mode ? 0 : 1;  // 切换显示模式
            Vehicle_To_Navigation_S();
            key_clear_state(KEY_5);
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
        if (SWITCH_1_STATUS == SWITCH_RIGHT)
        {
            // 当拨码开关1在右边时，执行GPS矫正功能
            GPS_Drift_Correction();
        }
        else
        {
            // 当拨码开关1在左边时，保存GPS点位
            Save_GPS_Point();
            // 保存后自动跳转到下一个点位并调整显示
            if (GPS_Point_Index < MAX_GPS_POINTS - 1)
            {
                GPS_Point_Index++; // 自动跳到下一个点位
                // 滚动显示逻辑
                if (GPS_Point_Index >= start_index + visible_items)
                    start_index = GPS_Point_Index - visible_items + 1;
            }
        }
        key_clear_state(KEY_3);
    }
    if (key4_state == KEY_SHORT_PRESS)
    {
        menu_state = MENU_MAIN;
        key_clear_state(KEY_4);
    }
    if (key5_state == KEY_SHORT_PRESS)
    {
        // 当SWITCH_1_STATUS == SWITCH_RIGHT时，执行ENU_To_INS_Points转换
        if (SWITCH_1_STATUS == SWITCH_RIGHT)
        {
            ENU_To_INS_Points();
        }
        key_clear_state(KEY_5);
    }
}

// 导航模式菜单按键处理函数
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
        Save_Test_Data();
        Calculate_Safety_Boundary(Navigation_Flag);
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
    Update_Adjust_Step();
    if (key3_state == KEY_SHORT_PRESS)
    {
        Camera_Choose = Camera_Choose ? 0 : 1;
        key_clear_state(KEY_3);
    }
    // 仅在二值化模式下调节阈值
    if (Camera_Choose == 1)
    {
        if (key1_state == KEY_SHORT_PRESS)
        {
            if (Camera_Threshold > 0)
            {
                Camera_Threshold -= (uint8_t)adjust_step;
            }
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            if (Camera_Threshold < 255)
            {
                Camera_Threshold += (uint8_t)adjust_step;
            }
            key_clear_state(KEY_2);
        }
    }
    if (Camera_Choose == 0)
    {
        if (key1_state == KEY_SHORT_PRESS)
        {
            if (Camera_Exposure > 0)
            {
                Camera_Exposure -= (uint8_t)adjust_step;
            }
            mt9v03x_set_exposure_time(Camera_Exposure);
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            if (Camera_Exposure < 255)
            {
                Camera_Exposure += (uint8_t)adjust_step;
            }
            mt9v03x_set_exposure_time(Camera_Exposure);
            key_clear_state(KEY_2);
        }
    }
    // 按键4：返回主菜单
    if (key4_state == KEY_SHORT_PRESS)
    {
        menu_state = MENU_MAIN;
        Save_Basic_Data();
        key_clear_state(KEY_4);
    }
}

// 编辑边界按键处理函数
void Boundary_Menu_Key_Process(void)
{
    Update_Adjust_Step();
    
    if (key1_state == KEY_SHORT_PRESS)
    {
        // 增加当前选中的安全边界外扩距离
        *boundary_menu[current_item].num += adjust_step;
        if (*boundary_menu[current_item].num < 5.0f) 
            *boundary_menu[current_item].num = 5.0f; // 设置最小值
        key_clear_state(KEY_1);
    }
    
    if (key2_state == KEY_SHORT_PRESS)
    {
        // 减少当前选中的安全边界外扩距离
        *boundary_menu[current_item].num -= adjust_step;
        if (*boundary_menu[current_item].num < 5.0f) 
            *boundary_menu[current_item].num = 5.0f; // 设置最小值
        key_clear_state(KEY_2);
    }
    
    if (key3_state == KEY_SHORT_PRESS)
    {
        // 在SAFETY_MARGIN_X和SAFETY_MARGIN_Y之间切换
        current_item = current_item ? 0 : 1;
        key_clear_state(KEY_3);
    }
    
    if (key4_state == KEY_SHORT_PRESS)
    {
        // 返回主菜单
        menu_state = MENU_MAIN;
        Save_Test_Data();
        key_clear_state(KEY_4);
    }
    
    if (key5_state == KEY_SHORT_PRESS)
    {
        // 更新安全边界计算
        Calculate_Safety_Boundary(Navigation_Flag);
        key_clear_state(KEY_5);
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

// 方向向量菜单按键处理
void Direction_Menu_Key_Process(void)
{
    if (key1_state == KEY_SHORT_PRESS)
    {
        // 向上切换方向点索引
        if (Direction_Point_Index > 0)
        {
            Direction_Point_Index--;
        }
        key_clear_state(KEY_1);
    }
    
    if (key2_state == KEY_SHORT_PRESS)
    {
        // 向下切换方向点索引
        if (Direction_Point_Index < 1)
        {
            Direction_Point_Index++;
        }
        key_clear_state(KEY_2);
    }
    
    if (key3_state == KEY_SHORT_PRESS)
    {
        // 保存当前GPS位置作为方向点
        if (gnss.state)  // 确保GPS有效
        {
            Save_Direction_Point();
            
            // 如果已经保存了两个点，则计算方向角度
            if (Direction_Point_Index == 1 && 
                Direction_Point[0][0] != 0.0 && Direction_Point[0][1] != 0.0 &&
                Direction_Point[1][0] != 0.0 && Direction_Point[1][1] != 0.0)
            {
                Get_Start_Direction();
                ips114_show_string(0, 48, "Direction Calculated!");
                system_delay_ms(1000);
            }
            
            // 自动切换到下一个点位
            if (Direction_Point_Index < 1)
            {
                Direction_Point_Index++;
            }
        }
        else
        {
            ips114_show_string(0, 48, "GPS Invalid! Wait...");
            system_delay_ms(1000);
        }
        key_clear_state(KEY_3);
    }
    
    if (key4_state == KEY_SHORT_PRESS)
    {
        // 返回主菜单
        menu_state = MENU_MAIN;
        key_clear_state(KEY_4);
    }

    if (key5_state == KEY_SHORT_PRESS)
    {
        // 切换方向角度显示模式
        yaw_flag = !yaw_flag;
        Save_Basic_Data();
        key_clear_state(KEY_5);
    }
}

//语音识别按键处理
void Voice_Led_Menu_Key_Process(void)
{
    // 按键1：初始化语音识别系统
    if (key1_state == KEY_SHORT_PRESS)
    {
        if (!audio_start_flag && audio_get_count == -1)
        {
            audio_init(); // 重新初始化语音识别系统
            // 清空命令数组
            command_buffer_index = 0;
            memset(command_buffer, 0, sizeof(command_buffer));
            voice_recognition_flag = 0;
            command_complete_flag = 1;
        }
        key_clear_state(KEY_1);
    }
    
    // 按键2：清空已识别命令
    if (key2_state == KEY_SHORT_PRESS)
    {
        if (!audio_start_flag)
        {
            command_buffer_index = 0;
            memset(command_buffer, 0, sizeof(command_buffer));
            voice_recognition_flag = 0;
            command_complete_flag = 1;
        }
        key_clear_state(KEY_2);
    }
    
    // 按键3：开始/停止语音识别 (替代原来的ASR_BUTTON)
    if (key3_state == KEY_SHORT_PRESS)
    {
        if (!audio_start_flag && audio_get_count == -1)
        {
            // 开始录音 - 模拟原来的按键触发逻辑
            audio_get_count = 0;
            audio_server_link_flag = 0;
            audio_start_flag = 1;
            asr_max_time = 0;
            
            // 清空之前的命令
            command_buffer_index = 0;
            memset(command_buffer, 0, sizeof(command_buffer));
            voice_recognition_flag = 0;
            command_complete_flag = 1;
            
            printf("手动开始语音识别...\r\n");
        }
        else if (audio_start_flag && audio_server_link_flag)
        {
            // 停止录音 - 模拟原来的按键释放逻辑
            audio_start_flag = 0;
            audio_get_count = 0;
            audio_send_data_flag = 0;
            audio_need_net_flag = 1;
            
            printf("手动停止语音识别...\r\n");
        }
        key_clear_state(KEY_3);
    }
    
    // 按键4：返回主菜单
    if (key4_state == KEY_SHORT_PRESS)
    {
        if (!audio_start_flag) // 只有在非录音状态下才能返回
        {
            menu_state = MENU_MAIN;
            dot_matrix_screen_set_brightness(0);
        }
        key_clear_state(KEY_4);
    }
    
    // 按键5：执行已识别的语音命令
    if (key5_state == KEY_SHORT_PRESS)
    {
        if (!audio_start_flag && voice_recognition_flag == 1 && command_buffer_index > 0)
        {
            // 调用命令执行函数
            Execute_Complete_Command();
            
            // 执行完成后清空命令数组
            command_buffer_index = 0;
            memset(command_buffer, 0, sizeof(command_buffer));
            voice_recognition_flag = 0;
            command_complete_flag = 1;
        }
        key_clear_state(KEY_5);
    }
    
    audio_loop();
}


void Test_Mode_Key_Process(void)
{
    if (key1_state == KEY_SHORT_PRESS)
    {
        // 向上选择
        if ((uint8_t)test_flag > 0)
        {
            test_flag = (TestFlag)((uint8_t)test_flag - 1);
        }
        else
        {
            test_flag = (TestFlag)(TEST_MODE_COUNT - 1); // 循环到最后一个
        }
        key_clear_state(KEY_1);
    }
    
    if (key2_state == KEY_SHORT_PRESS)
    {
        // 向下选择
        if ((uint8_t)test_flag < TEST_MODE_COUNT - 1)
        {
            test_flag = (TestFlag)((uint8_t)test_flag + 1);
        }
        else
        {
            test_flag = NO_TEST; // 循环到第一个
        }
        key_clear_state(KEY_2);
    }
    
    if (key3_state == KEY_SHORT_PRESS)
    {
        // 确认选择并保存
        Save_Basic_Data(); // 保存test_flag到Flash
        
        // 显示切换成功信息
        char success_msg[32];
        sprintf(success_msg, "Switched to %s", test_mode_names[(uint8_t)test_flag]);
        ips114_show_string(50, 80, success_msg);
        ips114_show_string(50, 96, "Please Press Reset");
        system_delay_ms(1500);
        
        // 返回主菜单
        menu_state = MENU_MAIN;
        key_clear_state(KEY_3);
    }
    
    if (key4_state == KEY_SHORT_PRESS)
    {
        menu_state = MENU_MAIN;
        key_clear_state(KEY_4);
    }
}

void Test3_Element_Menu_Key_Process(void)
{
    Update_Adjust_Step();

    if (edit_mode)
    {
        // 编辑模式处理
        if (key1_state == KEY_SHORT_PRESS)
        {
            if (!edit_coord)
            {
                *test3_menu[current_item].speed += adjust_step;
            }
            else
            {
                *test3_menu[current_item].index += adjust_step;
            }
            key_clear_state(KEY_1);
        }
        if (key2_state == KEY_SHORT_PRESS)
        {
            if (!edit_coord)
            {
                *test3_menu[current_item].speed -= adjust_step;
            }
            else
            {
                *test3_menu[current_item].index -= adjust_step;
            }
            key_clear_state(KEY_2);
        }
        if (key3_state == KEY_SHORT_PRESS)
        {
            // 切换编辑项目
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
            if (current_item < TEST3_MENU_ITEMS_COUNT - 1)
            {
                current_item++;
                if (current_item >= start_index + visible_items)
                    start_index = current_item - visible_items + 1;
            }
            key_clear_state(KEY_2);
        }
        
        if (key3_state == KEY_SHORT_PRESS)
        {
            // 进入编辑模式
            edit_mode = true;
            edit_coord = true; // 默认先编辑Point_Index
            key_clear_state(KEY_3);
        }
        
        if (key4_state == KEY_SHORT_PRESS)
        {
            // 返回主菜单
            Save_Test3Element();
            menu_state = MENU_MAIN;
            key_clear_state(KEY_4);
        }
    }
}
