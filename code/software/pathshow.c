#include "init.h"

// 路径显示参数
float zoom_factor = 1.0f;          // 缩放因子
float offset_x = 0.0f;             // X轴偏移
float offset_y = 0.0f;             // Y轴偏移
uint8_t show_points = 1;           // 是否显示点位标记
uint8_t show_current = 1;          // 是否显示当前位置
uint8_t path_type = PATH_TYPE_ALL; // 路径显示类型

// 找出路径点的边界值
void Find_Path_Bounds(float *min_x, float *max_x, float *min_y, float *max_y)
{
    // 初始化为保守的值
    *min_x = *min_y = 1e6f;
    *max_x = *max_y = -1e6f;

    int valid_points = 0;

    // 检查GPS路径点 (ENU坐标)
    if (path_type == PATH_TYPE_GPS || path_type == PATH_TYPE_ENU || path_type == PATH_TYPE_ALL)
    {
        for (uint8_t i = Start_GPS_Point; i <= End_GPS_Point; i++)
        {
            if (GPS_ENU[i][0] != 0.0f || GPS_ENU[i][1] != 0.0f)
            {
                *min_x = fminf(*min_x, GPS_ENU[i][0]);
                *max_x = fmaxf(*max_x, GPS_ENU[i][0]);
                *min_y = fminf(*min_y, GPS_ENU[i][1]);
                *max_y = fmaxf(*max_y, GPS_ENU[i][1]);
                valid_points++;
            }
        }
    }

    // 检查INS路径点
    if (path_type == PATH_TYPE_INS || path_type == PATH_TYPE_ALL)
    {
        for (uint8_t i = Start_INS_Point; i <= End_INS_Point; i++)
        {
            if (INS_Point[i][0] != 0.0f || INS_Point[i][1] != 0.0f)
            {
                *min_x = fminf(*min_x, INS_Point[i][0]);
                *max_x = fmaxf(*max_x, INS_Point[i][0]);
                *min_y = fminf(*min_y, INS_Point[i][1]);
                *max_y = fmaxf(*max_y, INS_Point[i][1]);
                valid_points++;
            }
        }
    }

    // 检查S型走位点
    if (path_type == PATH_TYPE_S || path_type == PATH_TYPE_ALL)
    {
        for (uint8_t i = Start_S_Point; i <= End_S_Point; i++)
        {
            if (S_Point[i][0] != 0.0f || S_Point[i][1] != 0.0f)
            {
                *min_x = fminf(*min_x, S_Point[i][0]);
                *max_x = fmaxf(*max_x, S_Point[i][0]);
                *min_y = fminf(*min_y, S_Point[i][1]);
                *max_y = fmaxf(*max_y, S_Point[i][1]);
                valid_points++;
            }
        }
    }

    // 当前位置也应考虑在边界内
    if (show_current)
    {
        *min_x = fminf(*min_x, position[0]);
        *max_x = fmaxf(*max_x, position[0]);
        *min_y = fminf(*min_y, position[1]);
        *max_y = fmaxf(*max_y, position[1]);
    }

    // 如果没有有效点，设置默认值
    if (valid_points == 0)
    {
        *min_x = -10.0f;
        *max_x = 10.0f;
        *min_y = -10.0f;
        *max_y = 10.0f;
    }

    // 给边界添加些余量，防止点刚好处于边缘
    float margin_x = (*max_x - *min_x) * 0.1f;
    float margin_y = (*max_y - *min_y) * 0.1f;

    // 确保至少有一些边界
    margin_x = fmaxf(margin_x, 1.0f);
    margin_y = fmaxf(margin_y, 1.0f);

    *min_x -= margin_x;
    *max_x += margin_x;
    *min_y -= margin_y;
    *max_y += margin_y;
}

// 坐标映射到屏幕像素
void Map_Coord_To_Screen(float x, float y, int16_t *screen_x, int16_t *screen_y)
{
    // 应用缩放和偏移
    float scaled_x = (x + offset_x) * zoom_factor;
    float scaled_y = (y + offset_y) * zoom_factor;

    // 映射到屏幕坐标
    *screen_x = (int16_t)(MARGIN_LEFT + scaled_x);
    *screen_y = (int16_t)(MARGIN_TOP + PATH_AREA_HEIGHT - scaled_y); // Y轴反转

    // 确保在屏幕范围内
    *screen_x = (*screen_x < 0) ? 0 : ((*screen_x >= DISPLAY_WIDTH) ? DISPLAY_WIDTH - 1 : *screen_x);
    *screen_y = (*screen_y < MARGIN_TOP) ? MARGIN_TOP : ((*screen_y >= DISPLAY_HEIGHT - MARGIN_BOTTOM) ? DISPLAY_HEIGHT - MARGIN_BOTTOM - 1 : *screen_y);
}

// 计算自动缩放值
void Calculate_Auto_Zoom(float min_x, float max_x, float min_y, float max_y)
{
    // 计算数据范围
    float range_x = max_x - min_x;
    float range_y = max_y - min_y;

    // 计算缩放因子，使数据适合显示区域
    float zoom_x = (range_x > 0) ? (PATH_AREA_WIDTH / range_x) : 1.0f;
    float zoom_y = (range_y > 0) ? (PATH_AREA_HEIGHT / range_y) : 1.0f;

    // 选择较小的缩放因子，确保完全显示
    zoom_factor = fminf(zoom_x, zoom_y) * 0.9f; // 留出10%的边界

    // 计算中心点
    float center_x = (min_x + max_x) / 2.0f;
    float center_y = (min_y + max_y) / 2.0f;

    // 设置偏移使得中心点位于屏幕中央
    offset_x = -(center_x - (PATH_AREA_WIDTH / 2.0f / zoom_factor));
    offset_y = -(center_y - (PATH_AREA_HEIGHT / 2.0f / zoom_factor));
}

// 绘制坐标轴
void Draw_Coordinate_System(void)
{
    int16_t origin_x, origin_y;
    Map_Coord_To_Screen(0, 0, &origin_x, &origin_y);

    // X轴
    ips114_draw_line(MARGIN_LEFT, origin_y, DISPLAY_WIDTH - MARGIN_RIGHT, origin_y, RGB565_GRAY);

    // Y轴
    ips114_draw_line(origin_x, MARGIN_TOP, origin_x, DISPLAY_HEIGHT - MARGIN_BOTTOM, RGB565_GRAY);

    // 坐标原点标记
    ips114_draw_point(origin_x, origin_y, RGB565_WHITE);
    ips114_draw_point(origin_x + 1, origin_y, RGB565_WHITE);
    ips114_draw_point(origin_x - 1, origin_y, RGB565_WHITE);
    ips114_draw_point(origin_x, origin_y + 1, RGB565_WHITE);
    ips114_draw_point(origin_x, origin_y - 1, RGB565_WHITE);
}

// 绘制路径线条
void Draw_Path_Lines(void)
{
    int16_t prev_x, prev_y, curr_x, curr_y;

    // 绘制GPS/ENU路径线
    if (path_type == PATH_TYPE_GPS || path_type == PATH_TYPE_ENU || path_type == PATH_TYPE_ALL)
    {
        // 确保至少有两个点可以绘线
        if (Start_GPS_Point < End_GPS_Point)
        {
            for (uint8_t i = Start_GPS_Point; i < End_GPS_Point; i++)
            {
                Map_Coord_To_Screen(GPS_ENU[i][0], GPS_ENU[i][1], &prev_x, &prev_y);
                Map_Coord_To_Screen(GPS_ENU[i + 1][0], GPS_ENU[i + 1][1], &curr_x, &curr_y);

                // 绘制GPS路径线
                ips114_draw_line(prev_x, prev_y, curr_x, curr_y, RGB565_CYAN);
            }
        }
    }

    // 绘制INS路径线
    if (path_type == PATH_TYPE_INS || path_type == PATH_TYPE_ALL)
    {
        // 确保至少有两个点可以绘线
        if (Start_INS_Point < End_INS_Point)
        {
            for (uint8_t i = Start_INS_Point; i < End_INS_Point; i++)
            {
                Map_Coord_To_Screen(INS_Point[i][0], INS_Point[i][1], &prev_x, &prev_y);
                Map_Coord_To_Screen(INS_Point[i + 1][0], INS_Point[i + 1][1], &curr_x, &curr_y);

                // 绘制INS路径线
                ips114_draw_line(prev_x, prev_y, curr_x, curr_y, RGB565_YELLOW);
            }
        }
    }

    // 绘制S型走位路径线
    if (path_type == PATH_TYPE_S || path_type == PATH_TYPE_ALL)
    {
        // 确保至少有两个点可以绘线
        if (Start_S_Point < End_S_Point)
        {
            for (uint8_t i = Start_S_Point; i < End_S_Point; i++)
            {
                Map_Coord_To_Screen(S_Point[i][0], S_Point[i][1], &prev_x, &prev_y);
                Map_Coord_To_Screen(S_Point[i + 1][0], S_Point[i + 1][1], &curr_x, &curr_y);

                // 绘制S型走位路径线
                ips114_draw_line(prev_x, prev_y, curr_x, curr_y, RGB565_MAGENTA);
            }
        }
    }
}

// 绘制标记点
void Draw_Points(void)
{
    int16_t x, y;

    // 绘制GPS/ENU点位
    if (path_type == PATH_TYPE_GPS || path_type == PATH_TYPE_ENU || path_type == PATH_TYPE_ALL)
    {
        for (uint8_t i = Start_GPS_Point; i <= End_GPS_Point; i++)
        {
            Map_Coord_To_Screen(GPS_ENU[i][0], GPS_ENU[i][1], &x, &y);

            // 绘制GPS点位标记
            if (i == NOW_GPS_Point && show_current)
            {
                // 当前点使用大红色标记
                ips114_draw_point(x, y, RGB565_RED);
                ips114_draw_point(x + 1, y, RGB565_RED);
                ips114_draw_point(x - 1, y, RGB565_RED);
                ips114_draw_point(x, y + 1, RGB565_RED);
                ips114_draw_point(x, y - 1, RGB565_RED);
            }
            else if (show_points)
            {
                // 普通点为蓝色小点
                ips114_draw_point(x, y, RGB565_BLUE);
            }
        }
    }

    // 绘制INS点位
    if (path_type == PATH_TYPE_INS || path_type == PATH_TYPE_ALL)
    {
        for (uint8_t i = Start_INS_Point; i <= End_INS_Point; i++)
        {
            Map_Coord_To_Screen(INS_Point[i][0], INS_Point[i][1], &x, &y);

            // 绘制INS点位标记
            if (i == NOW_INS_Point && show_current)
            {
                // 当前点使用大绿色标记
                ips114_draw_point(x, y, RGB565_GREEN);
                ips114_draw_point(x + 1, y, RGB565_GREEN);
                ips114_draw_point(x - 1, y, RGB565_GREEN);
                ips114_draw_point(x, y + 1, RGB565_GREEN);
                ips114_draw_point(x, y - 1, RGB565_GREEN);
            }
            else if (show_points)
            {
                // 普通点为黄色小点
                ips114_draw_point(x, y, RGB565_YELLOW);
            }
        }
    }

    // 绘制S型走位点位
    if (path_type == PATH_TYPE_S || path_type == PATH_TYPE_ALL)
    {
        for (uint8_t i = Start_S_Point; i <= End_S_Point; i++)
        {
            Map_Coord_To_Screen(S_Point[i][0], S_Point[i][1], &x, &y);

            // 绘制S型走位点位标记
            if (i == NOW_S_Point && show_current)
            {
                // 当前点使用大洋红色标记
                ips114_draw_point(x, y, RGB565_MAGENTA);
                ips114_draw_point(x + 1, y, RGB565_MAGENTA);
                ips114_draw_point(x - 1, y, RGB565_MAGENTA);
                ips114_draw_point(x, y + 1, RGB565_MAGENTA);
                ips114_draw_point(x, y - 1, RGB565_MAGENTA);
            }
            else if (show_points)
            {
                // 普通点为紫色小点
                ips114_draw_point(x, y, RGB565_PURPLE);
            }
        }
    }

    // 总是绘制当前位置
    if (show_current)
    {
        Map_Coord_To_Screen(position[0], position[1], &x, &y);

        // 当前车辆位置用橙色X标记
        ips114_draw_point(x - 1, y - 1, RGB565_MAGENTA);
        ips114_draw_point(x + 1, y + 1, RGB565_MAGENTA);
        ips114_draw_point(x - 1, y + 1, RGB565_MAGENTA);
        ips114_draw_point(x + 1, y - 1, RGB565_MAGENTA);
    }
}

// 绘制界面标题和信息
void Draw_UI_Info(void)
{
    char buffer[32];

    // 显示标题
    ips114_show_string(0, 0, "Path Visualization");

    // 显示缩放和导航模式信息
    sprintf(buffer, "Z:%.1f %s", zoom_factor, nav_mode_names[(uint8_t)Navigation_Flag]);
    ips114_show_string(0, 0, buffer);

    // 显示当前点位信息
    if (path_type == PATH_TYPE_GPS || path_type == PATH_TYPE_ENU || path_type == PATH_TYPE_ALL)
    {
        sprintf(buffer, "G:%d/%d", NOW_GPS_Point, End_GPS_Point);
        ips114_show_string(0, DISPLAY_HEIGHT, buffer);
    }

    if (path_type == PATH_TYPE_INS || path_type == PATH_TYPE_ALL)
    {
        sprintf(buffer, "I:%d/%d", NOW_INS_Point, End_INS_Point);
        ips114_show_string(80, DISPLAY_HEIGHT, buffer);
    }

    // 显示车辆当前位置
    if (show_current)
    {
        sprintf(buffer, "X:%.1f Y:%.1f", position[0], position[1]);
        ips114_show_string(0, DISPLAY_HEIGHT, buffer);
    }
}
