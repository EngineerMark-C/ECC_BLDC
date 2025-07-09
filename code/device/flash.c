#include "init.h"

// 64 bit GPS数据存储
typedef union {
    struct {
        uint32_t high;
        uint32_t low;
    } parts;
    double value;
} double_convert;

// 上电Flash初始化
// 1. 初始化 Basic_Data
// 2. 初始化 Test_Data
// 3. 初始化 GPS 点位
// 4. 初始化 ENU 点位
// 5. 初始化 INS 点位
// 6. 初始化 S 型走位点
// 7. 初始化发车方向

void Flash_Init(void)
{
    Basic_Data_Init();
    Test_Data_Init();
    Direction_Point_Init();
    GPS_Points_Init();
    WGS84_to_ENU_Init();
    INS_Points_Init();
    S_Point_Init();
    ips114_show_string(CENTER_X , CENTER_Y + IMAGE_HEIGHT + 10, "Welcome!");

}

//************************************GPS点位处理****************************************//
//                     | 索引 | 数据类型     | 说明                  |
//                     |------|------------|----------------------|
//                     | 0    | uint8      | 点位索引              |
//                     | 1    | uint32     | 纬度高位              |
//                     | 2    | uint32     | 纬度低位              |
//                     | 3    | uint32     | 经度高位              |
//                     | 4    | uint32     | 经度低位              |

// 保存 GPS 点位 (踩点)
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
        
        // 2. 同步到Flash - 根据test_flag选择存储区域
        flash_buffer_clear();
        
        // 先读取整页数据到缓冲区，保持其他区域数据不变
        flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
        
        // 计算存储偏移地址
        uint16_t offset = 0;
        switch(test_flag)
        {
            case NO_TEST:
                offset = 0;                                                     // 无科目存储区域
                break;
            case TEST__1:
                offset = MAX_GPS_POINTS * GPS_DATA_SIZE;                        // 科目一存储区域
                break;
            case TEST__2:
                offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 2;                    // 科目二存储区域
                break;
            case TEST__3:
                offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 3;                    // 科目三存储区域
                break;
            case TEST__4:
                offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 4;                    // 科目四存储区域
                break;
            default:
                offset = 0;
                break;
        }
        
        // 2.1 写入当前test_flag对应区域的GPS数据
        for(uint8_t i = 0; i < MAX_GPS_POINTS; i++)
        {
            double_convert lat, lon;
            lat.value = GPS_Point[i][0];
            lon.value = GPS_Point[i][1];
            
            flash_union_buffer[offset + i * GPS_DATA_SIZE].uint8_type = i;
            flash_union_buffer[offset + i * GPS_DATA_SIZE + 1].uint32_type = lat.parts.high;
            flash_union_buffer[offset + i * GPS_DATA_SIZE + 2].uint32_type = lat.parts.low;
            flash_union_buffer[offset + i * GPS_DATA_SIZE + 3].uint32_type = lon.parts.high;
            flash_union_buffer[offset + i * GPS_DATA_SIZE + 4].uint32_type = lon.parts.low;
        }
        
        // 2.2 擦除并写入Flash
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
        flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
        ips114_show_string(60, CENTER_Y, "GPS Point Saved.");
        system_delay_ms(500);
    }
}

// 保存 GPS 点位 (内存到Flash)
void Save_GPS_Point_Memory(void)
{
        flash_buffer_clear();

        flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
        // 计算存储偏移地址
        uint16_t offset = 0;
        switch(test_flag)
        {
            case NO_TEST:
                offset = 0;                                                     // 无科目存储区域
                break;
            case TEST__1:
                offset = MAX_GPS_POINTS * GPS_DATA_SIZE;                        // 科目一存储区域
                break;
            case TEST__2:
                offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 2;                    // 科目二存储区域
                break;
            case TEST__3:
                offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 3;                    // 科目三存储区域
                break;
            case TEST__4:
                offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 4;                    // 科目四存储区域
                break;
            default:
                offset = 0;
                break;
        }
        // 写入所有点位数据
        for(uint8_t i = 0; i < MAX_GPS_POINTS; i++)
        {
            double_convert lat, lon;
            lat.value = GPS_Point[i][0];
            lon.value = GPS_Point[i][1];
            flash_union_buffer[offset + i * GPS_DATA_SIZE].uint8_type = i;
            flash_union_buffer[offset + i * GPS_DATA_SIZE + 1].uint32_type = lat.parts.high;
            flash_union_buffer[offset + i * GPS_DATA_SIZE + 2].uint32_type = lat.parts.low;
            flash_union_buffer[offset + i * GPS_DATA_SIZE + 3].uint32_type = lon.parts.high;
            flash_union_buffer[offset + i * GPS_DATA_SIZE + 4].uint32_type = lon.parts.low;
        }
        // 擦除并写入Flash
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
        flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
        ips114_show_string(30, CENTER_Y, "GPS Point Memory Saved.");
        system_delay_ms(500);
        ips114_clear();
}

void GPS_Drift_Correction(void)
{
    ips114_show_string(30, 16, "GPS Drift Correcting...");

    // 保存原始基准点位置
    double original_lat = GPS_Point[0][0];
    double original_lon = GPS_Point[0][1];
    
    // 获取当前基准点的新位置（多次采样求平均）
    double current_lat_sum = 0;
    double current_lon_sum = 0;
    
    for(uint8_t j = 0; j < 15; j++)  // 增加采样次数提高精度
    {
        gnss_data_parse();
        current_lat_sum += NOW_location.latitude;
        current_lon_sum += NOW_location.longitude;
        system_delay_ms(100);
    }
    
    double current_lat = current_lat_sum / 15;
    double current_lon = current_lon_sum / 15;
    
    // 计算漂移量
    double drift_lat = current_lat - original_lat;
    double drift_lon = current_lon - original_lon;
    
    // 显示漂移信息
    ips114_show_string(60, 32, "Drift calculated.");
    char drift_info[32];
    sprintf(drift_info, "Lat:%.6f Lon:%.6f", drift_lat, drift_lon);
    ips114_show_string(10, 48, drift_info);
    system_delay_ms(1500);
    
    // 检查漂移是否在合理范围内（防止异常数据）
    double max_drift = 0.001;  // 最大允许漂移约100米
    if(fabs(drift_lat) > max_drift || fabs(drift_lon) > max_drift)
    {
        ips114_show_string(60, 64, "Drift too large!");
        system_delay_ms(1000);
        return;
    }
    
    // 对所有GPS点位进行漂移校正
    for(uint8_t i = 0; i < MAX_GPS_POINTS; i++)
    {
        // 跳过未设置的点位
        if(GPS_Point[i][0] == 0.0 && GPS_Point[i][1] == 0.0)
            continue;
            
        // 应用漂移校正
        GPS_Point[i][0] += drift_lat;
        GPS_Point[i][1] += drift_lon;
        
        // 同时更新ENU坐标
        WGS84_to_ENU(GPS_Point[i][0], GPS_Point[i][1], &GPS_ENU[i][0], &GPS_ENU[i][1]);
    }
    
    // 同步校正后的数据到Flash
    flash_buffer_clear();
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);

    // 计算存储偏移地址
    uint16_t offset = 0;
    switch(test_flag)
    {
        case NO_TEST:
            offset = 0;                                                     // 无科目存储区域
            break;
        case TEST__1:
            offset = MAX_GPS_POINTS * GPS_DATA_SIZE;                        // 科目一存储区域
            break;
        case TEST__2:
            offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 2;                    // 科目二存储区域
            break;
        case TEST__3:
            offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 3;                    // 科目三存储区域
            break;
        case TEST__4:
            offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 4;                    // 科目四存储区域
            break;
        default:
            offset = 0;
            break;
    }
    
    // 写入所有校正后的点位数据
    for(uint8_t i = 0; i < MAX_GPS_POINTS; i++)
    {
        double_convert lat, lon;
        lat.value = GPS_Point[i][0];
        lon.value = GPS_Point[i][1];
        flash_union_buffer[offset + i * GPS_DATA_SIZE].uint8_type = i;
        flash_union_buffer[offset + i * GPS_DATA_SIZE + 1].uint32_type = lat.parts.high;
        flash_union_buffer[offset + i * GPS_DATA_SIZE + 2].uint32_type = lat.parts.low;
        flash_union_buffer[offset + i * GPS_DATA_SIZE + 3].uint32_type = lon.parts.high;
        flash_union_buffer[offset + i * GPS_DATA_SIZE + 4].uint32_type = lon.parts.low;
    }
    
    // 擦除并写入Flash
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
    
    ips114_show_string(60, 64, "Correction Complete!");
    system_delay_ms(1000);
    ips114_clear();
}

// 上电初始化时调用
void GPS_Points_Init(void)
{
    ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "Loading GPS Points...");

    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);

    // 计算读取偏移地址
    uint16_t offset = 0;
    switch(test_flag)
    {
        case NO_TEST:
            offset = 0;                                                     // 无科目存储区域
            break;
        case TEST__1:
            offset = MAX_GPS_POINTS * GPS_DATA_SIZE;                        // 科目一存储区域
            break;
        case TEST__2:
            offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 2;                    // 科目二存储区域
            break;
        case TEST__3:
            offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 3;                    // 科目三存储区域
            break;
        case TEST__4:
            offset = MAX_GPS_POINTS * GPS_DATA_SIZE * 4;                    // 科目四存储区域
            break;
        default:
            offset = 0;
            break;
    }

    if(flash_union_buffer[offset].uint8_type != 0xFF)  // 检查首字节是否有效
    {
        for(uint8_t i = 0; i < MAX_GPS_POINTS; i++)
        {        
            if(flash_union_buffer[offset + i * GPS_DATA_SIZE].uint8_type == i)
            {
                double_convert lat, lon;
                lat.parts.high = flash_union_buffer[offset + i * GPS_DATA_SIZE + 1].uint32_type;
                lat.parts.low = flash_union_buffer[offset + i * GPS_DATA_SIZE + 2].uint32_type;
                lon.parts.high = flash_union_buffer[offset + i * GPS_DATA_SIZE + 3].uint32_type;
                lon.parts.low = flash_union_buffer[offset + i * GPS_DATA_SIZE + 4].uint32_type;

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
        ips114_clear_lines(CENTER_Y + IMAGE_HEIGHT + 10, CENTER_Y + IMAGE_HEIGHT + 26);
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
        ips114_show_string(60, CENTER_Y, "Direction Point Saved.");
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

        Start_Direction = yaw_flag ? Start_Direction : 0.0f;

        char buf[32];
        sprintf(buf, "Start Dir: %.1f", Start_Direction);
        ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, buf);
        system_delay_ms(1000);  // 显示1秒
        ips114_clear_lines(CENTER_Y + IMAGE_HEIGHT + 10, CENTER_Y + IMAGE_HEIGHT + 26);
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
        
        // 2. 同步到Flash - 根据test_flag选择存储区域
        flash_buffer_clear();
        
        // 先读取整页数据到缓冲区，保持其他区域数据不变
        flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
        
        // 计算存储偏移地址
        uint16_t offset = 0;
        switch(test_flag)
        {
            case NO_TEST:
                offset = 0;                                                     // 无科目存储区域
                break;
            case TEST__1:
                offset = MAX_INS_POINTS * INS_DATA_SIZE;                        // 科目一存储区域
                break;
            case TEST__2:
                offset = MAX_INS_POINTS * INS_DATA_SIZE * 2;                    // 科目二存储区域
                break;
            case TEST__3:
                offset = MAX_INS_POINTS * INS_DATA_SIZE * 3;                    // 科目三存储区域
                break;
            case TEST__4:
                offset = MAX_INS_POINTS * INS_DATA_SIZE * 4;                    // 科目四存储区域
                break;
            default:
                offset = 0;
                break;
        }
        
        // 2.1 写入当前test_flag对应区域的INS数据
        for(uint8_t i = 0; i < MAX_INS_POINTS; i++)
        {
            flash_union_buffer[offset + i * INS_DATA_SIZE].uint8_type = i;
            flash_union_buffer[offset + i * INS_DATA_SIZE + 1].float_type = INS_Point[i][0];
            flash_union_buffer[offset + i * INS_DATA_SIZE + 2].float_type = INS_Point[i][1];
        }
        
        // 2.2 擦除并写入Flash
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
        flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
        ips114_show_string(60, CENTER_Y, "INS Point Saved.");
        system_delay_ms(500);
    }
}

//保存 INS 点位 (内存到Flash)
void Save_INS_Point_Memory(void)
{
    // 清空数据缓冲区
    flash_buffer_clear();
    
    // 先读取整页数据到缓冲区，保持其他区域数据不变
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
    
    // 计算存储偏移地址
    uint16_t offset = 0;
    switch(test_flag)
    {
        case NO_TEST:
            offset = 0;                                                         // 无科目存储区域
            break;
        case TEST__1:
            offset = MAX_INS_POINTS * INS_DATA_SIZE;                            // 科目一存储区域
            break;
        case TEST__2:
            offset = MAX_INS_POINTS * INS_DATA_SIZE * 2;                        // 科目二存储区域
            break;
        case TEST__3:
            offset = MAX_INS_POINTS * INS_DATA_SIZE * 3;                        // 科目三存储区域
            break;
        case TEST__4:
            offset = MAX_INS_POINTS * INS_DATA_SIZE * 4;                        // 科目四存储区域
            break;
        default:
            offset = 0;
            break;
    }

    // 写入当前test_flag对应区域的INS数据
    for(uint8_t i = 0; i < MAX_INS_POINTS; i++)
    {
        flash_union_buffer[offset + i * INS_DATA_SIZE].uint8_type = i;
        flash_union_buffer[offset + i * INS_DATA_SIZE + 1].float_type = INS_Point[i][0];
        flash_union_buffer[offset + i * INS_DATA_SIZE + 2].float_type = INS_Point[i][1];
    }
    
    // 擦除并写入Flash
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
    ips114_show_string(30, CENTER_Y, "INS Point Memory Saved.");
    system_delay_ms(500);
}

// 上电初始化时调用
void INS_Points_Init(void)
{
    ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "Loading INS Points...");

    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);

    // 计算读取偏移地址
    uint16_t offset = 0;
    switch(test_flag)
    {
        case NO_TEST:
            offset = 0;                                                         // 无科目存储区域
            break;
        case TEST__1:
            offset = MAX_INS_POINTS * INS_DATA_SIZE;                            // 科目一存储区域
            break;
        case TEST__2:
            offset = MAX_INS_POINTS * INS_DATA_SIZE * 2;                        // 科目二存储区域
            break;
        case TEST__3:
            offset = MAX_INS_POINTS * INS_DATA_SIZE * 3;                        // 科目三存储区域
            break;
        case TEST__4:
            offset = MAX_INS_POINTS * INS_DATA_SIZE * 4;                        // 科目四存储区域
            break;
        default:
            offset = 0;
            break;
    }

    if(flash_union_buffer[offset].uint8_type != 0xFF)  // 检查首字节是否有效
    {
        for(uint8_t i = 0; i < MAX_INS_POINTS; i++)
        {        
            if(flash_union_buffer[offset + i * INS_DATA_SIZE].uint8_type == i)
            {
                INS_Point[i][0] = flash_union_buffer[offset + i * INS_DATA_SIZE + 1].float_type;
                INS_Point[i][1] = flash_union_buffer[offset + i * INS_DATA_SIZE + 2].float_type;
            }
            else
            {
                continue;
            }
        }
        Vehicle_To_Navigation_INS();
        ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "INS Points Loaded.");
        system_delay_ms(1000);  // 显示1秒
        ips114_clear_lines(CENTER_Y + IMAGE_HEIGHT + 10, CENTER_Y + IMAGE_HEIGHT + 26);
    }
}

//************************************S 点位处理****************************************//
//                     | 索引 | 数据类型    | 说明                  |
//                     |------|-----------|----------------------|
//                     | 0    | uint8     | 点位索引              |
//                     | 1    | float     | x轴坐标               |
//                     | 2    | float     | y轴坐标               |

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
        ips114_show_string(60, CENTER_Y, "S Point Saved.");
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
        Vehicle_To_Navigation_S();
        ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "S Points Loaded.");
        system_delay_ms(1000);  // 显示1秒
        ips114_clear_lines(CENTER_Y + IMAGE_HEIGHT + 10, CENTER_Y + IMAGE_HEIGHT + 26);
    }
}

//************************************基本参数存储****************************************//
//                     | 索引 | 数据类型    | 说明                 |
//                     |------|-----------|---------------------|
//                     | 0    | uint8     | test_flag           |
//                     | 1    | uint8     | yaw_flag            |
//                     | 2    | uint8     | Camera_Threshold    |
//                     | 3    | float     | gyro_bias[0]        |
//                     | 4    | float     | gyro_bias[1]        |
//                     | 5    | float     | gyro_bias[2]        |
//                     | 6    | float     | acc_bias[0]         |
//                     | 7    | float     | acc_bias[1]         |
//                     | 8    | float     | acc_bias[2]         |

// 保存基础数据
void Save_Basic_Data(void)
{
    flash_buffer_clear();
    flash_union_buffer[0].uint8_type = (uint8_t)test_flag;
    flash_union_buffer[1].uint8_type = (uint8_t)yaw_flag;
    flash_union_buffer[2].uint8_type = Camera_Threshold;
    flash_union_buffer[3].float_type = gyro_bias[0];
    flash_union_buffer[4].float_type = gyro_bias[1];
    flash_union_buffer[5].float_type = gyro_bias[2];
    flash_union_buffer[6].float_type = acc_bias[0];
    flash_union_buffer[7].float_type = acc_bias[1];
    flash_union_buffer[8].float_type = acc_bias[2];

    // 擦除并写入Flash
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_BASIC_DATA_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_BASIC_DATA_INDEX);
    ips114_show_string(80, CENTER_Y, "Saved.");
    system_delay_ms(500);
}

// 上电初始化时调用
void Basic_Data_Init(void)
{
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_BASIC_DATA_INDEX);

    test_flag = flash_union_buffer[0].uint8_type;
    yaw_flag = flash_union_buffer[1].uint8_type;
    Camera_Threshold = flash_union_buffer[2].uint8_type;
    gyro_bias[0] = flash_union_buffer[3].float_type;
    gyro_bias[1] = flash_union_buffer[4].float_type;
    gyro_bias[2] = flash_union_buffer[5].float_type;
    acc_bias[0] = flash_union_buffer[6].float_type;
    acc_bias[1] = flash_union_buffer[7].float_type;
    acc_bias[2] = flash_union_buffer[8].float_type;

    // 第一次烧录确保初始化赋初值
    // test_flag = 3;
    // yaw_flag = 0;
    // Camera_Threshold = 64;
    // gyro_bias[0] = 0.0f;
    // gyro_bias[1] = 0.0f;
    // gyro_bias[2] = 0.0f;
    // acc_bias[0] = 0.0f;
    // acc_bias[1] = 0.0f;
    // acc_bias[2] = 0.0f;

    ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, "Basic Data Loaded.");
    system_delay_ms(500);
    ips114_clear_lines(CENTER_Y + IMAGE_HEIGHT + 10, CENTER_Y + IMAGE_HEIGHT + 26);
}

//************************************科目参数存储****************************************//
//                     | 索引 | 数据类型    | 说明                 |
//                     |-----|------------|---------------------|
//                     | 0   | uint8      | Start_GPS_Point     |
//                     | 1   | uint8      | End_GPS_Point       |
//                     | 2   | uint8      | Start_INS_Point     |
//                     | 3   | uint8      | Back_INS_Point      |
//                     | 4   | uint8      | End_INS_Point       |
//                     | 5   | uint8      | GPS_TO_INS_Point    |
//                     | 6   | uint8      | Navigation_Flag     |
//                     | 7   | uint8      | Start_S_Point       |
//                     | 8   | uint8      | End_S_Point         |
//                     | 9   | float      | S_Distance          |
//                     | 10  | float      | GPS_SWITCH_DISTANCE |
//                     | 11  | float      | INS_SWITCH_DISTANCE |
//                     | 12  | float      | SAFETY_X_MAX        |
//                     | 13  | float      | SAFETY_X_MIN        |
//                     | 14  | float      | SAFETY_Y_MAX        |
//                     | 15  | float      | SAFETY_Y_MIN        |
//                     | 16  | float      | SAFETY_MARGIN       |
//                     | 17  | float      | MAX_SPEED           |
//                     | 18  | float      | MIN_SPEED           |
//                     | 19  | float      | BRAKING_DISTANCE    |
//                     | 20  | float      | S_MAX_SPEED         |
//                     | 21  | float      | S_MIN_SPEED         |
//                     | 22  | float      | S_BRAKING_DISTANCE  |


// 保存科目数据
void Save_Test_Data(void)
{
    flash_buffer_clear();
    // 先读取整页数据到缓冲区，保持其他区域数据不变
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);

    // 计算存储偏移地址
    uint16_t offset = 0;
    switch(test_flag)
    {
        case NO_TEST:
            offset = 0;                                                         // 无科目存储区域
            break;
        case TEST__1:
            offset = TEST_DATA_SIZE;                                            // 科目一存储区域
            break;
        case TEST__2:
            offset = TEST_DATA_SIZE * 2;                                        // 科目二存储区域
            break;
        case TEST__3:
            offset = TEST_DATA_SIZE * 3;                                        // 科目三存储区域
            break;
        case TEST__4:
            offset = TEST_DATA_SIZE * 4;                                        // 科目四存储区域
            break;
        default:
            offset = 0;
            break;
    }

    flash_union_buffer[offset + 0].uint8_type = Start_GPS_Point;
    flash_union_buffer[offset + 1].uint8_type = End_GPS_Point;
    flash_union_buffer[offset + 2].uint8_type = Start_INS_Point;
    flash_union_buffer[offset + 3].uint8_type = Back_INS_Point;
    flash_union_buffer[offset + 4].uint8_type = End_INS_Point;
    flash_union_buffer[offset + 5].uint8_type = GPS_TO_INS_Point;
    flash_union_buffer[offset + 6].uint8_type = (uint8_t)Navigation_Flag;
    flash_union_buffer[offset + 7].uint8_type = Start_S_Point;
    flash_union_buffer[offset + 8].uint8_type = End_S_Point;
    flash_union_buffer[offset + 9].float_type = S_Distance;
    flash_union_buffer[offset + 10].float_type = GPS_SWITCH_DISTANCE;
    flash_union_buffer[offset + 11].float_type = INS_SWITCH_DISTANCE;
    flash_union_buffer[offset + 12].float_type = SAFETY_X_MAX;
    flash_union_buffer[offset + 13].float_type = SAFETY_X_MIN;
    flash_union_buffer[offset + 14].float_type = SAFETY_Y_MAX;
    flash_union_buffer[offset + 15].float_type = SAFETY_Y_MIN;
    flash_union_buffer[offset + 16].float_type = SAFETY_MARGIN;
    flash_union_buffer[offset + 17].float_type = MAX_SPEED;
    flash_union_buffer[offset + 18].float_type = MIN_SPEED;
    flash_union_buffer[offset + 19].float_type = BRAKING_DISTANCE;
    flash_union_buffer[offset + 20].float_type = S_MAX_SPEED;
    flash_union_buffer[offset + 21].float_type = S_MIN_SPEED;
    flash_union_buffer[offset + 22].float_type = S_BRAKING_DISTANCE;

    // 擦除并写入Flash
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
    ips114_show_string(60, CENTER_Y, "Test Data Saved.");
    system_delay_ms(500);
}

// 上电初始化时调用
void Test_Data_Init(void)
{
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);

    // 计算读取偏移地址
    uint16_t offset = 0;
    switch(test_flag)
    {
        case NO_TEST:
            offset = 0;                                                         // 无科目存储区域
            break;
        case TEST__1:
            offset = TEST_DATA_SIZE;                                            // 科目一存储区域
            break;
        case TEST__2:
            offset = TEST_DATA_SIZE * 2;                                        // 科目二存储区域
            break;
        case TEST__3:
            offset = TEST_DATA_SIZE * 3;                                        // 科目三存储区域
            break;
        case TEST__4:
            offset = TEST_DATA_SIZE * 4;                                        // 科目四存储区域
            break;
        default:
            offset = 0;
            break;
    }

    Start_GPS_Point = flash_union_buffer[offset + 0].uint8_type;
    End_GPS_Point = flash_union_buffer[offset + 1].uint8_type;
    Start_INS_Point = flash_union_buffer[offset + 2].uint8_type;
    Back_INS_Point = flash_union_buffer[offset + 3].uint8_type;
    End_INS_Point = flash_union_buffer[offset + 4].uint8_type;
    GPS_TO_INS_Point = flash_union_buffer[offset + 5].uint8_type;
    Navigation_Flag = flash_union_buffer[offset + 6].uint8_type;
    Start_S_Point = flash_union_buffer[offset + 7].uint8_type;
    End_S_Point = flash_union_buffer[offset + 8].uint8_type;
    S_Distance = flash_union_buffer[offset + 9].float_type;
    GPS_SWITCH_DISTANCE = flash_union_buffer[offset + 10].float_type;
    INS_SWITCH_DISTANCE = flash_union_buffer[offset + 11].float_type;
    SAFETY_X_MAX = flash_union_buffer[offset + 12].float_type;
    SAFETY_X_MIN = flash_union_buffer[offset + 13].float_type;
    SAFETY_Y_MAX = flash_union_buffer[offset + 14].float_type;
    SAFETY_Y_MIN = flash_union_buffer[offset + 15].float_type;
    SAFETY_MARGIN = flash_union_buffer[offset + 16].float_type;
    MAX_SPEED = flash_union_buffer[offset + 17].float_type;
    MIN_SPEED = flash_union_buffer[offset + 18].float_type;
    BRAKING_DISTANCE = flash_union_buffer[offset + 19].float_type;
    S_MAX_SPEED = flash_union_buffer[offset + 20].float_type;
    S_MIN_SPEED = flash_union_buffer[offset + 21].float_type;
    S_BRAKING_DISTANCE = flash_union_buffer[offset + 22].float_type;

    // 第一次烧录确保过初始化赋初值
    // Start_GPS_Point = 1;
    // End_GPS_Point = 2;
    // Start_INS_Point = 0;
    // Back_INS_Point = 0;
    // End_INS_Point = 0;
    // GPS_TO_INS_Point = 0;
    // Navigation_Flag = 0;
    // Start_S_Point = 0;
    // End_S_Point = 0;
    // S_Distance = 1.2f;
    // GPS_SWITCH_DISTANCE = 1.2f;
    // INS_SWITCH_DISTANCE = 0.2f;
    // SAFETY_X_MAX = 0.0f;
    // SAFETY_X_MIN = 0.0f;
    // SAFETY_Y_MAX = 0.0f;
    // SAFETY_Y_MIN = 0.0f;
    // SAFETY_MARGIN = 8.0f;
    // MAX_SPEED = 3.0f;
    // MIN_SPEED = 2.0f;
    // BRAKING_DISTANCE = 3.0f;

    NOW_GPS_Point = Start_GPS_Point;
    NOW_INS_Point = Start_INS_Point;
    NOW_S_Point = Start_S_Point;

    char buf[32];
    sprintf(buf, "Test%d Data Loaded", test_flag);
    ips114_show_string(CENTER_X - 30, CENTER_Y + IMAGE_HEIGHT + 10, buf);
    system_delay_ms(500);
    ips114_clear_lines(CENTER_Y + IMAGE_HEIGHT + 10, CENTER_Y + IMAGE_HEIGHT + 26);
}

void Save_All_Flash_Data(void)
{
    // 保存所有数据到Flash
    Save_Basic_Data();
    Save_Test_Data();
    Save_GPS_Point_Memory();
    Save_INS_Point_Memory();
    Save_S_Point();
    Save_Direction_Point();
    ips114_show_string(60, CENTER_Y, "All Data Saved.");
    system_delay_ms(500);
    ips114_clear();
}   


/**
 * 重置所有Flash数据区域
 * 
 * 此函数会清除所有存储在Flash中的数据，包括：
 * 1. GPS点位数据
 * 2. INS点位数据
 * 3. 基本参数数据
 * 4. 科目参数数据
 * 5. S点位数据
 * 6. 方向点位数据
 * 
 * 调用此函数后，所有Flash区域将被置为0x00
 */
void Reset_All_Flash_Data(void)
{
    ips114_show_string(60, CENTER_Y, "Resetting Flash...");
    system_delay_ms(500);
    
    // 清空缓冲区，将所有数据设为0
    flash_buffer_clear();
    
    // 擦除并写入0到GPS数据区域
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
    
    // 擦除并写入0到INS数据区域
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_INS_DATA_INDEX);
    
    // 擦除并写入0到基础数据区域
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_BASIC_DATA_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_BASIC_DATA_INDEX);
    
    // 擦除并写入0到科目数据区域
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_TEST_DATA_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_TEST_DATA_INDEX);
    
    // 擦除并写入0到S点位数据区域
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_S_DATA_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_S_DATA_INDEX);
    
    // 擦除并写入0到方向点位数据区域
    flash_erase_page(FLASH_SECTION_INDEX, DIRECTION_POINT_INDEX);
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, DIRECTION_POINT_INDEX);

    ips114_show_string(60, CENTER_Y + 16, "Flash Reset Complete.");
    system_delay_ms(1000);
    ips114_clear();
}
