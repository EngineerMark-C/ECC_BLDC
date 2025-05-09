#include "zf_common_headfile.h"
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
        //多次取值求平均
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
    ips114_show_string(60, 32, "Loading GPS Points...");
    
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
        ips114_show_string(60, 32, "GPS Points Loaded.");
        system_delay_ms(1000);  // 显示1秒
        ips114_clear();         // 清屏
    }
}

// 擦除 GPS 数据
void Erase_GPS_Points(void)
{
    flash_erase_page(FLASH_SECTION_INDEX, FLASH_GPS_DATA_INDEX);
    ips114_show_string(60, 32, "GPS Points Erased.");
    system_delay_ms(500);
}

//************************************INS点位处理****************************************//
//                     | 索引 | 数据类型    | 说明                  |
//                     |------|------------|----------------------|
//                     | 0    | uint8      | 点位索引              |
//                     | 1    | float      | x轴坐标               |
//                     | 2    | float      | y轴坐标               |
// 保存 INS 点位
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

// 上电初始化时调用
void INS_Points_Init(void)
{
    ips114_show_string(60, 32, "Loading INS Points...");
    
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
        ips114_show_string(60, 32, "INS Points Loaded.");
        system_delay_ms(1000);  // 显示1秒
        ips114_clear();         // 清屏
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
//                     | 8    | uint8      | GPS_TO_INS_POINT     |
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
    flash_union_buffer[8].uint8_type = GPS_TO_INS_POINT;
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
    GPS_TO_INS_POINT = flash_union_buffer[8].uint8_type;
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

    NOW_GPS_Point = Start_GPS_Point;
    NOW_INS_Point = Start_INS_Point;
    NOW_S_Point = Start_S_Point;

    ips114_show_string(60, 32, "Basic Data Loaded.");
    system_delay_ms(500);
}
