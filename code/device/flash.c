#include "init.h"

// 64 bit GPS数据存储
typedef union {
    struct {
        uint32_t high;
        uint32_t low;
    } parts;
    double value;
} double_convert;

//************************************GPS点位处理****************************************//
//                     | 索引 | 数据类型    | 说明                  |
//                     |------|------------|----------------------|
//                     | 0    | uint8      | 点位索引              |
//                     | 1    | uint32     | 纬度高位              |
//                     | 2    | uint32     | 纬度低位              |
//                     | 3    | uint32     | 经度高位              |
//                     | 4    | uint32     | 经度低位              |
// 保存 GPS 点位
void Save_GPS_Point(void)
{
    if(GPS_Point_Index < MAX_GPS_POINTS)
    {
        // 1. 保存到内存
        //    多次取值求平均
        double LATSUM = 0;
        double LONSUM = 0;
        for (uint8_t j = 0; j < 10; j++)
        {
            gnss_data_parse();
            LATSUM += NOW_location.latitude;
            LONSUM += NOW_location.longitude;
            system_delay_ms(100);
        }
        GPS_Point[GPS_Point_Index][0] = LATSUM / 10;
        GPS_Point[GPS_Point_Index][1] = LONSUM / 10;
        WGS84_to_ENU(GPS_Point[GPS_Point_Index][0], GPS_Point[GPS_Point_Index][1], &GPS_ENU[GPS_Point_Index][0], &GPS_ENU[GPS_Point_Index][1]);
        // 2. 同步到Flash
        flash_buffer_clear();
        
        // 2.1 写入所有内存中的点位数据(包括新点位)
        for(uint8_t i = 0; i < MAX_GPS_POINTS; i++)
        {
            double_convert lat, lon;
            lat.value = GPS_Point[i][0];
            lon.value = GPS_Point[i][1];
            
            flash_union_buffer[i * GPS_DATA_SIZE].uint8_type = i;
            flash_union_buffer[i * GPS_DATA_SIZE + 1].uint32_type = lat.parts.high;
            flash_union_buffer[i * GPS_DATA_SIZE + 2].uint32_type = lat.parts.low;
            flash_union_buffer[i * GPS_DATA_SIZE + 3].uint32_type = lon.parts.high;
            flash_union_buffer[i * GPS_DATA_SIZE + 4].uint32_type = lon.parts.low;
        }
        
        // 2.2 擦除并写入Flash
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
        flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
        ips114_show_string(60, 32, "GPS Point Saved.");
        system_delay_ms(500);
    }
}

// 上电初始化时调用
void GPS_Points_Init(void)
{
    ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "Loading GPS Points...");

    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);

    if(flash_union_buffer[0].uint8_type != 0xFF)  // 检查首字节是否有效
    {
        for(uint8_t i = 0; i < MAX_GPS_POINTS; i++)
        {        
            if(flash_union_buffer[i * GPS_DATA_SIZE].uint8_type == i)
            {
                double_convert lat, lon;
                lat.parts.high = flash_union_buffer[i * GPS_DATA_SIZE + 1].uint32_type;
                lat.parts.low = flash_union_buffer[i * GPS_DATA_SIZE + 2].uint32_type;
                lon.parts.high = flash_union_buffer[i * GPS_DATA_SIZE + 3].uint32_type;
                lon.parts.low = flash_union_buffer[i * GPS_DATA_SIZE + 4].uint32_type;
                
                GPS_Point[i][0] = lat.value;
                GPS_Point[i][1] = lon.value;
            }
            else
            {
                continue;
            }
        }
        ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "GPS Points Loaded.");
        system_delay_ms(1000);  // 显示1秒
        ips114_clear_lines(CENTER_Y + IMAGE_HEIGHT + 10, CENTER_Y + IMAGE_HEIGHT + 16);
    }
}

// 方向向量存储
void Save_Direction_Point(void)
{
    if (Direction_Point_Index < 2)
    {
        double LATSUM = 0;
        double LONSUM = 0;

        // 1. 保存到内存
        for (uint8_t j = 0; j < 10; j++)
        {
            gnss_data_parse();
            LATSUM += NOW_location.latitude;
            LONSUM += NOW_location.longitude;
            system_delay_ms(100);
        }
        Direction_Point[Direction_Point_Index][0] = LATSUM / 10;
        Direction_Point[Direction_Point_Index][1] = LONSUM / 10;

        // 2. 同步到Flash
        flash_buffer_clear();
        
        // 2.1 写入所有内存中的点位数据(包括新点位)
        for(uint8_t i = 0; i < 2; i++)
        {
            double_convert lat, lon;
            lat.value = Direction_Point[i][0];
            lon.value = Direction_Point[i][1];
            
            flash_union_buffer[i * GPS_DATA_SIZE].uint8_type = i;
            flash_union_buffer[i * GPS_DATA_SIZE + 1].uint32_type = lat.parts.high;
            flash_union_buffer[i * GPS_DATA_SIZE + 2].uint32_type = lat.parts.low;
            flash_union_buffer[i * GPS_DATA_SIZE + 3].uint32_type = lon.parts.high;
            flash_union_buffer[i * GPS_DATA_SIZE + 4].uint32_type = lon.parts.low;
        }
        
        // 2.2 擦除并写入Flash
        flash_erase_page(FLASH_SECTION_INDEX, DIRECTION_POINT_INDEX);
        flash_write_page_from_buffer(FLASH_SECTION_INDEX, DIRECTION_POINT_INDEX);
        ips114_show_string(60, 32, "Direction Point Saved.");
        system_delay_ms(500);
    }
}

// 上电初始化时调用
void Direction_Point_Init(void)
{
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, DIRECTION_POINT_INDEX);
    if(flash_union_buffer[0].uint8_type != 0xFF)  // 检查首字节是否有效
    {
        for(uint8_t i = 0; i < 2; i++)
        {
            if(flash_union_buffer[i * GPS_DATA_SIZE].uint8_type == i)
            {
                double_convert lat, lon;
                lat.parts.high = flash_union_buffer[i * GPS_DATA_SIZE + 1].uint32_type;
                lat.parts.low = flash_union_buffer[i * GPS_DATA_SIZE + 2].uint32_type;
                lon.parts.high = flash_union_buffer[i * GPS_DATA_SIZE + 3].uint32_type;
                lon.parts.low = flash_union_buffer[i * GPS_DATA_SIZE + 4].uint32_type;
                
                Direction_Point[i][0] = lat.value;
                Direction_Point[i][1] = lon.value;
            }
            else
            {
                continue;
            }
        }
        Get_Start_Direction();  // 获取发车方向角度
        ips114_show_string(CENTER_X - 50, CENTER_Y + IMAGE_HEIGHT + 10, "Direction Points Loaded.");
        system_delay_ms(1000);  // 显示1秒
        ips114_clear();
    }
}


//************************************INS点位处理****************************************//
//                     | 索引 | 数据类型    | 说明                  |
//                     |------|------------|----------------------|
//                     | 0    | uint8      | 点位索引              |
//                     | 1    | float      | x轴坐标               |
//                     | 2    | float      | y轴坐标               |
// 保存 INS 点位 (踩点)
void Save_INS_Point(void)
{
    if(INS_Point_Index < MAX_INS_POINTS)
    {
        // 1. 保存到内存
        INS_Point[INS_Point_Index][0] = position[0];
        INS_Point[INS_Point_Index][1] = position[1];
        
        // 2. 同步到Flash
        flash_buffer_clear();
        
        // 2.1 写入所有内存中的点位数据(包括新点位)
        for(uint8_t i = 0; i < MAX_INS_POINTS; i++)
        {
            flash_union_buffer[i * INS_DATA_SIZE].uint8_type = i;
            flash_union_buffer[i * INS_DATA_SIZE + 1].float_type = INS_Point[i][0];
            flash_union_buffer[i * INS_DATA_SIZE + 2].float_type = INS_Point[i][1];
        }
        
        // 2.2 擦除并写入Flash
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
        flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
        ips114_show_string(60, 32, "INS Point Saved.");
        system_delay_ms(500);
    }
}

//保存 INS 点位 (内存到Flash)
void Save_INS_Point_Memory(void)
{
    // 清空数据缓冲区
    flash_buffer_clear();
    
    // 写入缓冲区
    for(uint8_t i = 0; i < MAX_INS_POINTS; i++)
    {
        flash_union_buffer[i * INS_DATA_SIZE].uint8_type = i;
        flash_union_buffer[i * INS_DATA_SIZE + 1].float_type = INS_Point[i][0];
        flash_union_buffer[i * INS_DATA_SIZE + 2].float_type = INS_Point[i][1];
    }
    
    // 擦除并写入Flash
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
    ips114_show_string(30, 32, "INS Point Memory Saved.");
    system_delay_ms(500);
}

// 上电初始化时调用
void INS_Points_Init(void)
{
    ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "Loading INS Points...");

    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);

    if(flash_union_buffer[0].uint8_type != 0xFF)  // 检查首字节是否有效
    {
        for(uint8_t i = 0; i < MAX_INS_POINTS; i++)
        {        
            if(flash_union_buffer[i * INS_DATA_SIZE].uint8_type == i)
            {
                INS_Point[i][0] = flash_union_buffer[i * INS_DATA_SIZE + 1].float_type;
                INS_Point[i][1] = flash_union_buffer[i * INS_DATA_SIZE + 2].float_type;
            }
            else
            {
                continue;
            }
        }
        ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "INS Points Loaded.");
        system_delay_ms(1000);  // 显示1秒
        ips114_clear_lines(CENTER_Y + IMAGE_HEIGHT + 10, CENTER_Y + IMAGE_HEIGHT + 16);
    }
}

//************************************S 点位处理****************************************//
//                     | 索引 | 数据类型    | 说明                  |
//                     |------|------------|----------------------|
//                     | 0    | uint8      | 点位索引              |
//                     | 1    | float      | x轴坐标               |
//                     | 2    | float      | y轴坐标               |

// 保存 S 型走位点
void Save_S_Point(void)
{
    if(S_Point_Index < MAX_INS_POINTS)
    {
        // 清空数据缓冲区
        flash_buffer_clear();
        
        // 写入缓冲区
        for(uint8_t i = 0; i < MAX_INS_POINTS; i++)
        {
            flash_union_buffer[i * INS_DATA_SIZE].uint8_type = i;
            flash_union_buffer[i * INS_DATA_SIZE + 1].float_type = S_Point[i][0];
            flash_union_buffer[i * INS_DATA_SIZE + 2].float_type = S_Point[i][1];
        }
        
        // 擦除并写入Flash
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_S_DATA_INDEX);
        flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_S_DATA_INDEX);
        ips114_show_string(60, 32, "S Point Saved.");
        system_delay_ms(500);
    }
}

// 上电初始化时调用
void S_Point_Init(void)
{
    ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "Loading S Points...");

    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_S_DATA_INDEX);

    if(flash_union_buffer[0].uint8_type != 0xFF)  // 检查首字节是否有效
    {
        for(uint8_t i = 0; i < MAX_S_POINTS; i++)
        {
            if(flash_union_buffer[i * INS_DATA_SIZE].uint8_type == i)
            {
                S_Point[i][0] = flash_union_buffer[i * INS_DATA_SIZE + 1].float_type;
                S_Point[i][1] = flash_union_buffer[i * INS_DATA_SIZE + 2].float_type;
            }
            else
            {
                continue;
            }
        }
        ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "S Points Loaded.");
        system_delay_ms(1000);  // 显示1秒
        ips114_clear_lines(CENTER_Y + IMAGE_HEIGHT + 10, CENTER_Y + IMAGE_HEIGHT + 16);
    }
}

//************************************基本参数存储****************************************//
//                     | 索引 | 数据类型    | 说明                 |
//                     |------|------------|----------------------|
//                     | 0    | uint8      | Start_GPS_Point      |
//                     | 1    | uint8      | End_GPS_Point        |
//                     | 2    | float      | target_speed         |
//                     | 3    | float      | gyro_bias[0]         |
//                     | 4    | float      | gyro_bias[1]         |
//                     | 5    | float      | gyro_bias[2]         |
//                     | 6    | uint8      | Start_INS_Point      |
//                     | 7    | uint8      | End_INS_Point        |
//                     | 8    | uint8      | GPS_TO_INS_Point     |
//                     | 9    | uint8      | Navigation_Flag      |
//                     | 10   | uint8      | Start_S_Point        |
//                     | 11   | uint8      | End_S_Point          |
//                     | 12   | float      | SAFETY_X_MAX         |
//                     | 13   | float      | SAFETY_X_MIN         |
//                     | 14   | float      | SAFETY_Y_MAX         |
//                     | 15   | float      | SAFETY_Y_MIN         |
//                     | 16   | float      | MAX_SPEED            |
//                     | 17   | float      | MIN_SPEED            |
//                     | 18   | float      | APPROACH_SPEED       |
//                     | 19   | float      | BRAKING_DISTANCE     |
//                     | 20   | float      | S_Distance           |
//                     | 21   | float      | acc_bias[0]          |
//                     | 22   | float      | acc_bias[1]          |
//                     | 23   | float      | acc_bias[2]          |
//                     | 24   | uint8      | Camera_Threshold     |
//                     | 25   | float      | GPS_SWITCH_DISTANCE  |
//                     | 26   | float      | INS_SWITCH_DISTANCE  |
//                     | 27   | float      | SAFETY_MARGIN        |
//                     | 28   | uint8      | Back_INS_Point       |

// 保存基础数据
void Save_Basic_Data(void)
{
    flash_buffer_clear();

    flash_union_buffer[0].uint8_type = Start_GPS_Point;
    flash_union_buffer[1].uint8_type = End_GPS_Point;
    flash_union_buffer[2].float_type = target_speed;
    flash_union_buffer[3].float_type = gyro_bias[0];
    flash_union_buffer[4].float_type = gyro_bias[1];
    flash_union_buffer[5].float_type = gyro_bias[2];
    flash_union_buffer[6].uint8_type = Start_INS_Point;
    flash_union_buffer[7].uint8_type = End_INS_Point;
    flash_union_buffer[8].uint8_type = GPS_TO_INS_Point;
    flash_union_buffer[9].uint8_type = (uint8_t)Navigation_Flag;
    flash_union_buffer[10].uint8_type = Start_S_Point;
    flash_union_buffer[11].uint8_type = End_S_Point;
    flash_union_buffer[12].float_type = SAFETY_X_MAX;
    flash_union_buffer[13].float_type = SAFETY_X_MIN;
    flash_union_buffer[14].float_type = SAFETY_Y_MAX;
    flash_union_buffer[15].float_type = SAFETY_Y_MIN;
    flash_union_buffer[16].float_type = MAX_SPEED;
    flash_union_buffer[17].float_type = MIN_SPEED;
    flash_union_buffer[18].float_type = APPROACH_SPEED;
    flash_union_buffer[19].float_type = BRAKING_DISTANCE;
    flash_union_buffer[20].float_type = S_Distance;
    flash_union_buffer[21].float_type = acc_bias[0];
    flash_union_buffer[22].float_type = acc_bias[1];
    flash_union_buffer[23].float_type = acc_bias[2];
    flash_union_buffer[24].uint8_type = Camera_Threshold;
    flash_union_buffer[25].float_type = GPS_SWITCH_DISTANCE;
    flash_union_buffer[26].float_type = INS_SWITCH_DISTANCE;
    flash_union_buffer[27].float_type = SAFETY_MARGIN;
    flash_union_buffer[28].uint8_type = Back_INS_Point;

    // 擦除并写入Flash
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_BASIC_DATA_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_BASIC_DATA_INDEX);
    ips114_show_string(60, 32, "Saved.");
    system_delay_ms(500);
}

void Basic_Data_Init(void)
{
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_BASIC_DATA_INDEX);

    Start_GPS_Point = flash_union_buffer[0].uint8_type;
    End_GPS_Point = flash_union_buffer[1].uint8_type;
    target_speed = flash_union_buffer[2].float_type;
    gyro_bias[0] = flash_union_buffer[3].float_type;
    gyro_bias[1] = flash_union_buffer[4].float_type;
    gyro_bias[2] = flash_union_buffer[5].float_type;
    Start_INS_Point = flash_union_buffer[6].uint8_type;
    End_INS_Point = flash_union_buffer[7].uint8_type;
    GPS_TO_INS_Point = flash_union_buffer[8].uint8_type;
    Navigation_Flag = flash_union_buffer[9].uint8_type;
    Start_S_Point = flash_union_buffer[10].uint8_type;
    End_S_Point = flash_union_buffer[11].uint8_type;
    SAFETY_X_MAX = flash_union_buffer[12].float_type;
    SAFETY_X_MIN = flash_union_buffer[13].float_type;
    SAFETY_Y_MAX = flash_union_buffer[14].float_type;
    SAFETY_Y_MIN = flash_union_buffer[15].float_type;
    MAX_SPEED = flash_union_buffer[16].float_type;
    MIN_SPEED = flash_union_buffer[17].float_type;
    APPROACH_SPEED = flash_union_buffer[18].float_type;
    BRAKING_DISTANCE = flash_union_buffer[19].float_type;
    S_Distance = flash_union_buffer[20].float_type;
    acc_bias[0] = flash_union_buffer[21].float_type;
    acc_bias[1] = flash_union_buffer[22].float_type;
    acc_bias[2] = flash_union_buffer[23].float_type;
    Camera_Threshold = flash_union_buffer[24].uint8_type;
    GPS_SWITCH_DISTANCE = flash_union_buffer[25].float_type;
    INS_SWITCH_DISTANCE = flash_union_buffer[26].float_type;
    SAFETY_MARGIN = flash_union_buffer[27].float_type;
    Back_INS_Point = flash_union_buffer[28].uint8_type;

    NOW_GPS_Point = Start_GPS_Point;
    NOW_INS_Point = Start_INS_Point;
    NOW_S_Point = Start_S_Point;

    ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "Basic Data Loaded.");
    system_delay_ms(500);
    ips114_clear_lines(CENTER_Y + IMAGE_HEIGHT + 10, CENTER_Y + IMAGE_HEIGHT + 16);
}

//************************************Flash完全重置****************************************//
/**
 * 重置所有Flash数据区域
 * 
 * 此函数会清除所有存储在Flash中的数据，包括：
 * 1. GPS点位数据
 * 2. INS点位数据
 * 3. 基本参数数据
 * 
 * 调用此函数后，所有Flash区域将被擦除为0xFF（擦除状态）
 */
void Reset_All_Flash_Data(void)
{
    // 显示开始重置提示
    ips114_clear();
    ips114_show_string(60, 16, "Resetting Flash...");
    
    // 1. 擦除GPS点位数据区
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
    ips114_show_string(60, 32, "GPS Data Reset");
    system_delay_ms(300);
    
    // 2. 擦除INS点位数据区
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
    ips114_show_string(60, 48, "INS Data Reset");
    system_delay_ms(300);
    
    // 3. 擦除基本参数数据区
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_BASIC_DATA_INDEX);
    ips114_show_string(60, 64, "Basic Data Reset");
    system_delay_ms(300);
    
    // 4. 重置内存中的数据结构（可选）
    // 重置GPS点位数组
    for(uint8_t i = 0; i < MAX_GPS_POINTS; i++)
    {
        GPS_Point[i][0] = 0.0;
        GPS_Point[i][1] = 0.0;
        GPS_ENU[i][0] = 0.0;
        GPS_ENU[i][1] = 0.0;
    }
    GPS_Point_Index = 0;
    
    // 重置INS点位数组
    for(uint8_t i = 0; i < MAX_INS_POINTS; i++)
    {
        INS_Point[i][0] = 0.0;
        INS_Point[i][1] = 0.0;
    }
    INS_Point_Index = 0;
    
    // 重置基本参数为默认值
    Start_GPS_Point = 0;
    End_GPS_Point = 0;
    target_speed = 1.0f;  // 默认目标速度
    gyro_bias[0] = 0.0f;
    gyro_bias[1] = 0.0f;
    gyro_bias[2] = 0.0f;
    Start_INS_Point = 0;
    End_INS_Point = 0;
    GPS_TO_INS_Point = 0;
    Navigation_Flag = 0;
    Start_S_Point = 0;
    End_S_Point = 0;
    SAFETY_X_MAX = 100.0f;  // 默认安全边界
    SAFETY_X_MIN = -100.0f;
    SAFETY_Y_MAX = 100.0f;
    SAFETY_Y_MIN = -100.0f;
    MAX_SPEED = 4.0f;     // 默认速度限制
    MIN_SPEED = 2.0f;
    APPROACH_SPEED = 3.0f;
    BRAKING_DISTANCE = 5.0f;
    S_Distance = 0.5f;
    acc_bias[0] = 0.0f;
    acc_bias[1] = 0.0f;
    acc_bias[2] = 0.0f;
    Camera_Threshold = 64; // 默认二值化阈值为64
    GPS_SWITCH_DISTANCE = 1.0f; // 默认GPS切换距离
    INS_SWITCH_DISTANCE = 0.2f;  // 默认INS切换距离
    SAFETY_MARGIN = 5.0f;
    Back_INS_Point = 0;
    
    NOW_GPS_Point = 0;
    NOW_INS_Point = 0;
    NOW_S_Point = 0;
    
    // 显示重置完成提示
    ips114_clear();
    ips114_show_string(60, 32, "Flash Reset Complete");
    system_delay_ms(1000);
    ips114_clear();
}
