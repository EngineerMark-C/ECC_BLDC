#include "init.h"

#define Camera_Kp                     0.4f                                    // 摄像头控制Kp参数

uint8_t Camera_Threshold;                                                       // 二值化阈值
uint8_t Camera_Exposure;                                                        // 曝光时间
uint8_t Camera_Image[MT9V03X_H][MT9V03X_W];                                     // 摄像头图像数据
float angle_adjustment;

// 白线检测相关变量
int16_t Line_Center_Position = -1;                                              // 白线中心位置 (-1表示未检测到)
uint8_t Line_Detected = 0;                                                      // 线条检测标志
int8_t Line_Direction = 0;                                                      // 线条方向: -1左偏, 0居中, 1右偏
int16_t Line_Offset = 0;                                                        // 白线偏移量

line_info_t Line_Info[MT9V03X_H];                                               // 每行的白线信息

void Camera_Init(void)
{
    // 初始化摄像头
    mt9v03x_init();
    mt9v03x_set_exposure_time(Camera_Exposure);
}

/**
 * @brief 复制摄像头图像数据到处理缓冲区
 */
void Camera_Copy_Image(void)
{
    for(int row = 0; row < MT9V03X_H; row++)
    {
        for(int col = 0; col < MT9V03X_W; col++)
        {
            Camera_Image[row][col] = mt9v03x_image[row][col];
        }
    }
}

/**
 * @brief 对图像进行二值化处理
 * @param threshold 二值化阈值
 */
void Camera_Binarization(uint8_t threshold)
{
    for(int row = 0; row < MT9V03X_H; row++)
    {
        for(int col = 0; col < MT9V03X_W; col++)
        {
            if(Camera_Image[row][col] > threshold)
                Camera_Image[row][col] = 255;  // 白色
            else
                Camera_Image[row][col] = 0;    // 黑色
        }
    }
}

/**
 * @brief 在指定行寻找白线边界
 * @param row 扫描行
 * @param left_edge 返回左边界位置
 * @param right_edge 返回右边界位置
 * @return 是否找到白线
 */
uint8_t Find_Line_Edges(int row, int *left_edge, int *right_edge)
{
    int left = -1, right = -1;
    
    // 从左往右扫描找左边界
    for(int col = 0; col < MT9V03X_W; col++)
    {
        if(Camera_Image[row][col] == 255)
        {
            left = col;
            break;
        }
    }
    
    // 从右往左扫描找右边界
    for(int col = MT9V03X_W - 1; col >= 0; col--)
    {
        if(Camera_Image[row][col] == 255)
        {
            right = col;
            break;
        }
    }
    
    // 检查是否找到有效的白线
    if(left != -1 && right != -1 && (right - left) > 5)  // 最小宽度阈值
    {
        *left_edge = left;
        *right_edge = right;
        return 1;
    }
    
    return 0;
}

/**
 * @brief 白线检测和位置计算
 */
void Camera_Line_Detection(void)
{
    // 先复制图像数据
    Camera_Copy_Image();
    
    // 进行二值化
    Camera_Binarization(Camera_Threshold);
    
    int valid_lines = 0;
    int total_center = 0;
    int screen_center = MT9V03X_W / 2;  // 屏幕中心位置 (94)
    
    // 初始化线条信息
    for(int i = 0; i < MT9V03X_H; i++)
    {
        Line_Info[i].valid = 0;
    }
    
    // 遍历每一行寻找白线
    for(int row = 0; row < MT9V03X_H - 5; row++)
    {
        int left_edge, right_edge;
        
        if(Find_Line_Edges(row, &left_edge, &right_edge))
        {
            int line_center = (left_edge + right_edge) / 2;
            total_center += line_center;
            valid_lines++;
            
            // 保存线条信息用于显示
            Line_Info[row].left_edge = left_edge;
            Line_Info[row].right_edge = right_edge;
            Line_Info[row].center = line_center;
            Line_Info[row].valid = 1;
        }
    }
    
    // 判断是否检测到白线
    if(valid_lines >= 15)  // 至少15行检测到白线才认为有效
    {
        Line_Detected = 1;
        Line_Center_Position = total_center / valid_lines;
        
        // 计算偏移方向
        Line_Offset = Line_Center_Position - screen_center;

        if(Line_Offset < -10)
            Line_Direction = -1;  // 白线偏左
        else if(Line_Offset > 10)
            Line_Direction = 1;   // 白线偏右
        else
            Line_Direction = 0;   // 白线居中
    }
    else
    {
        Line_Detected = 0;
        Line_Center_Position = -1;
        Line_Direction = 0;
    }
}

/**
 * @brief 获取白线检测结果
 * @return 线条方向: -1左偏, 0居中或未检测到, 1右偏
 */
int8_t Camera_Get_Line_Direction(void)
{
    return Line_Direction;
}

/**
 * @brief 获取白线中心位置
 * @return 白线中心位置 (-1表示未检测到)
 */
int16_t Camera_Get_Line_Position(void)
{
    return Line_Center_Position;
}

/**
 * @brief 检查是否检测到白线
 * @return 1表示检测到白线, 0表示未检测到
 */
uint8_t Camera_Is_Line_Detected(void)
{
    return Line_Detected;
}

int16_t Camera_Get_Line_Offset(void)
{
    return Line_Offset;
}

/**
 * @brief 基于摄像头白线检测的舵机角度控制
 * @return 需要调整的角度
 */
void Camera_Steer_Control(void)
{
    if(Camera_Is_Line_Detected())
    {
        angle_adjustment = Line_Offset * Camera_Kp;
    }
    else
        angle_adjustment = 0.0f;
}

/**
 * @brief 摄像头主处理函数
 * 建议在主循环中调用
 */
void Camera_Process(void)
{
    if(mt9v03x_finish_flag)  // 摄像头采集完成
    {
        Camera_Line_Detection();
        Camera_Steer_Control();
        mt9v03x_finish_flag = 0;  // 清除标志
    }
}
