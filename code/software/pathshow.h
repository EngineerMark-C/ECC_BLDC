#ifndef _pathshow_h_
#define _pathshow_h_

// 路径类型常量
#define PATH_TYPE_GPS      0
#define PATH_TYPE_ENU      1 
#define PATH_TYPE_INS      2
#define PATH_TYPE_S        3
#define PATH_TYPE_ALL      4

// 显示屏物理参数
#define DISPLAY_WIDTH      180
#define DISPLAY_HEIGHT     112
#define MARGIN_TOP         16
#define MARGIN_BOTTOM      16
#define MARGIN_LEFT        10
#define MARGIN_RIGHT       10
#define PATH_AREA_WIDTH    (DISPLAY_WIDTH - MARGIN_LEFT - MARGIN_RIGHT)
#define PATH_AREA_HEIGHT   (DISPLAY_HEIGHT - MARGIN_TOP - MARGIN_BOTTOM)

// 路径可视化参数
extern float zoom_factor;
extern float offset_x;
extern float offset_y;

// 标记点参数
extern uint8_t show_points;
extern uint8_t show_current; 
extern uint8_t path_type;

void Find_Path_Bounds(float* min_x, float* max_x, float* min_y, float* max_y);
void Calculate_Auto_Zoom(float min_x, float max_x, float min_y, float max_y);
void Map_Coord_To_Screen(float x, float y, int16_t* screen_x, int16_t* screen_y);
void Draw_Coordinate_System(void);
void Draw_Path_Lines(void);
void Draw_Points(void);
void Draw_UI_Info(void);


#endif