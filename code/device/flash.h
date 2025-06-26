#ifndef _flash_h_
#define _flash_h_

#define FLASH_SECTION_INDEX             (0)                                     // 存储数据用的扇区
#define FLASH_PAGE_INDEX                (8)                                     // 存储数据用的页码       目前未使用


//************************************GPS点位处理****************************************//

#define FLASH_GPS_DATA_INDEX            (5)                                     // 存储 GPS 数据用的页码
#define MAX_GPS_POINTS                  (16)                                    // 最大 GPS 点位数
#define GPS_DATA_SIZE                   (5)                                     // 5个存储单元（索引 + 纬度高低位 + 经度高低位）

void Save_GPS_Point(void);
void GPS_Points_Init(void);

//************************************INS点位处理****************************************//

#define FLASH_INS_DATA_INDEX            (4)                                     // 存储 INS 数据用的页码
#define MAX_INS_POINTS                  (16)                                    // 最大 INS 点位数
#define INS_DATA_SIZE                   (7)                                     // 3个存储单元（索引 + x轴坐标 + y轴坐标） 第3页有问题

void Save_INS_Point(void);
void INS_Points_Init(void);

//************************************S点位处理****************************************//
#define FLASH_S_DATA_INDEX              (3)                                     // 存储 S 数据用的页码
#define MAX_S_POINTS                    (16)                                    // 最大 S 点位数

void Save_S_Point(void);
void S_Point_Init(void);

//************************************基础数据处理****************************************//
#define FLASH_BASIC_DATA_INDEX          (6)                                     // 存储基础数据用的页码

void Save_Basic_Data(void);
void Basic_Data_Init(void);

void Reset_All_Flash_Data(void);

#endif
