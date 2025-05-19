/*********************************************************************************************************************
* 日期              作者               备注
* 2023-11-10        呆萌侠智能科技      first version
* 2025-05-19        GitHub Copilot     重构为zf_device风格
********************************************************************************************************************/
/*********************************************************************************************************************
* 接线定义：
*                   ------------------------------------
*                   模块管脚           单片机管脚
*                   // 硬件 SPI 引脚
*                   SCL/SPC           查看 zf_device_icm42688.h 中 ICM42688_SPC_PIN 宏定义
*                   SDA/DSI           查看 zf_device_icm42688.h 中 ICM42688_SDI_PIN 宏定义
*                   SA0/SDO           查看 zf_device_icm42688.h 中 ICM42688_SDO_PIN 宏定义
*                   CS                查看 zf_device_icm42688.h 中 ICM42688_CS_PIN 宏定义
*                   VCC               3.3V电源
*                   GND               电源地
*                   其余引脚悬空
*
*                   // 软件 IIC 引脚
*                   SCL/SPC           查看 zf_device_icm42688.h 中 ICM42688_SCL_PIN 宏定义
*                   SDA/DSI           查看 zf_device_icm42688.h 中 ICM42688_SDA_PIN 宏定义
*                   VCC               3.3V电源
*                   GND               电源地
*                   其余引脚悬空
*                   ------------------------------------
********************************************************************************************************************/
#ifndef _zf_device_icm42688_h_
#define _zf_device_icm42688_h_

#include "zf_common_typedef.h"

//================================================定义 ICM42688 基本配置================================================
// ICM42688_USE_SOFT_IIC定义为0表示使用硬件SPI驱动 定义为1表示使用软件IIC驱动
#define ICM42688_USE_SOFT_IIC       (0)                                         // 默认使用硬件 SPI 方式驱动
#if ICM42688_USE_SOFT_IIC                                                       // 这两段 颜色正常的才是正确的 颜色灰的就是没有用的
//====================================================软件 IIC 驱动====================================================
#define ICM42688_SOFT_IIC_DELAY     (59 )                                       // 软件 IIC 的时钟延时周期 数值越小 IIC 通信速率越快
#define ICM42688_SCL_PIN            (P20_11)                                    // 软件 IIC SCL 引脚 连接 ICM42688 的 SCL 引脚
#define ICM42688_SDA_PIN            (P20_14)                                    // 软件 IIC SDA 引脚 连接 ICM42688 的 SDA 引脚
//====================================================软件 IIC 驱动====================================================
#else
//====================================================硬件 SPI 驱动====================================================
#define ICM42688_SPI_SPEED          (10 * 1000 * 1000)                          // 硬件 SPI 速率
#define ICM42688_SPI                (SPI_0)                                     // 硬件 SPI 号
#define ICM42688_SPC_PIN            (SPI0_SCLK_P20_11)                          // 硬件 SPI SCK 引脚
#define ICM42688_SDI_PIN            (SPI0_MOSI_P20_14)                          // 硬件 SPI MOSI 引脚
#define ICM42688_SDO_PIN            (SPI0_MISO_P20_12)                          // 硬件 SPI MISO 引脚
//====================================================硬件 SPI 驱动====================================================
#endif

#define ICM42688_CS_PIN             (P20_13)                                    // CS 片选引脚
#define ICM42688_CS(x)              ((x) ? (gpio_high(ICM42688_CS_PIN)) : (gpio_low(ICM42688_CS_PIN)))
#define ICM42688_TIMEOUT_COUNT      (0x00FF)                                    // ICM42688 超时计数

typedef enum
{
    ICM42688_ACC_SAMPLE_SGN_2G ,                                                // 加速度计量程 ±2G
    ICM42688_ACC_SAMPLE_SGN_4G ,                                                // 加速度计量程 ±4G
    ICM42688_ACC_SAMPLE_SGN_8G ,                                                // 加速度计量程 ±8G
    ICM42688_ACC_SAMPLE_SGN_16G,                                                // 加速度计量程 ±16G
}icm42688_acc_sample_config_enum;

typedef enum
{
    ICM42688_GYRO_SAMPLE_SGN_15_625DPS ,                                        // 陀螺仪量程 ±15.625DPS
    ICM42688_GYRO_SAMPLE_SGN_31_25DPS ,                                         // 陀螺仪量程 ±31.25DPS
    ICM42688_GYRO_SAMPLE_SGN_62_5DPS ,                                          // 陀螺仪量程 ±62.5DPS
    ICM42688_GYRO_SAMPLE_SGN_125DPS ,                                           // 陀螺仪量程 ±125DPS
    ICM42688_GYRO_SAMPLE_SGN_250DPS ,                                           // 陀螺仪量程 ±250DPS
    ICM42688_GYRO_SAMPLE_SGN_500DPS ,                                           // 陀螺仪量程 ±500DPS
    ICM42688_GYRO_SAMPLE_SGN_1000DPS,                                           // 陀螺仪量程 ±1000DPS
    ICM42688_GYRO_SAMPLE_SGN_2000DPS,                                           // 陀螺仪量程 ±2000DPS
}icm42688_gyro_sample_config_enum;

typedef enum
{
    ICM42688_OUTPUT_RATE_32000HZ,                                               // 输出速率 32000Hz
    ICM42688_OUTPUT_RATE_16000HZ,                                               // 输出速率 16000Hz
    ICM42688_OUTPUT_RATE_8000HZ,                                                // 输出速率 8000Hz
    ICM42688_OUTPUT_RATE_4000HZ,                                                // 输出速率 4000Hz
    ICM42688_OUTPUT_RATE_2000HZ,                                                // 输出速率 2000Hz
    ICM42688_OUTPUT_RATE_1000HZ,                                                // 输出速率 1000Hz
    ICM42688_OUTPUT_RATE_200HZ,                                                 // 输出速率 200Hz
    ICM42688_OUTPUT_RATE_100HZ,                                                 // 输出速率 100Hz
    ICM42688_OUTPUT_RATE_50HZ,                                                  // 输出速率 50Hz
    ICM42688_OUTPUT_RATE_25HZ,                                                  // 输出速率 25Hz
    ICM42688_OUTPUT_RATE_12_5HZ,                                                // 输出速率 12.5Hz
    ICM42688_OUTPUT_RATE_500HZ,                                                 // 输出速率 500Hz
}icm42688_output_rate_enum;

#define ICM42688_ACC_SAMPLE_DEFAULT     ( ICM42688_ACC_SAMPLE_SGN_8G )          // 在这设置默认的 加速度计 初始化量程
#define ICM42688_GYRO_SAMPLE_DEFAULT    ( ICM42688_GYRO_SAMPLE_SGN_2000DPS )    // 在这设置默认的 陀螺仪 初始化量程
#define ICM42688_ACC_RATE_DEFAULT       ( ICM42688_OUTPUT_RATE_1000HZ )         // 在这设置默认的 加速度计 初始化速率
#define ICM42688_GYRO_RATE_DEFAULT      ( ICM42688_OUTPUT_RATE_1000HZ )         // 在这设置默认的 陀螺仪 初始化速率
//================================================定义 ICM42688 基本配置================================================

//================================================定义 ICM42688 内部地址================================================
// ICM42688Bank0内部地址
#define ICM42688_DEVICE_CONFIG             0x11
#define ICM42688_DRIVE_CONFIG              0x13
#define ICM42688_INT_CONFIG                0x14
#define ICM42688_FIFO_CONFIG               0x16
#define ICM42688_TEMP_DATA1                0x1D
#define ICM42688_TEMP_DATA0                0x1E
#define ICM42688_ACCEL_DATA_X1             0x1F
#define ICM42688_ACCEL_DATA_X0             0x20
#define ICM42688_ACCEL_DATA_Y1             0x21
#define ICM42688_ACCEL_DATA_Y0             0x22
#define ICM42688_ACCEL_DATA_Z1             0x23
#define ICM42688_ACCEL_DATA_Z0             0x24
#define ICM42688_GYRO_DATA_X1              0x25
#define ICM42688_GYRO_DATA_X0              0x26
#define ICM42688_GYRO_DATA_Y1              0x27
#define ICM42688_GYRO_DATA_Y0              0x28
#define ICM42688_GYRO_DATA_Z1              0x29
#define ICM42688_GYRO_DATA_Z0              0x2A
#define ICM42688_TMST_FSYNCH               0x2B
#define ICM42688_TMST_FSYNCL               0x2C
#define ICM42688_INT_STATUS                0x2D
#define ICM42688_FIFO_COUNTH               0x2E
#define ICM42688_FIFO_COUNTL               0x2F
#define ICM42688_FIFO_DATA                 0x30
#define ICM42688_APEX_DATA0                0x31
#define ICM42688_APEX_DATA1                0x32
#define ICM42688_APEX_DATA2                0x33
#define ICM42688_APEX_DATA3                0x34
#define ICM42688_APEX_DATA4                0x35
#define ICM42688_APEX_DATA5                0x36
#define ICM42688_INT_STATUS2               0x37
#define ICM42688_INT_STATUS3               0x38
#define ICM42688_SIGNAL_PATH_RESET         0x4B
#define ICM42688_INTF_CONFIG0              0x4C
#define ICM42688_INTF_CONFIG1              0x4D
#define ICM42688_PWR_MGMT0                 0x4E
#define ICM42688_GYRO_CONFIG0              0x4F
#define ICM42688_ACCEL_CONFIG0             0x50
#define ICM42688_GYRO_CONFIG1              0x51
#define ICM42688_GYRO_ACCEL_CONFIG0        0x52
#define ICM42688_ACCEL_CONFIG1             0x53
#define ICM42688_TMST_CONFIG               0x54
#define ICM42688_APEX_CONFIG0              0x56
#define ICM42688_SMD_CONFIG                0x57
#define ICM42688_FIFO_CONFIG1              0x5F
#define ICM42688_FIFO_CONFIG2              0x60
#define ICM42688_FIFO_CONFIG3              0x61
#define ICM42688_FSYNC_CONFIG              0x62
#define ICM42688_INT_CONFIG0               0x63
#define ICM42688_INT_CONFIG1               0x64
#define ICM42688_INT_SOURCE0               0x65
#define ICM42688_INT_SOURCE1               0x66
#define ICM42688_INT_SOURCE3               0x68
#define ICM42688_INT_SOURCE4               0x69
#define ICM42688_FIFO_LOST_PKT0            0x6C
#define ICM42688_FIFO_LOST_PKT1            0x6D
#define ICM42688_SELF_TEST_CONFIG          0x70
#define ICM42688_WHO_AM_I                  0x75
#define ICM42688_REG_BANK_SEL              0x76 // Banks
#define ICM42688_SENSOR_CONFIG0            0x03
#define ICM42688_GYRO_CONFIG_STATIC2       0x0B
#define ICM42688_GYRO_CONFIG_STATIC3       0x0C
#define ICM42688_GYRO_CONFIG_STATIC4       0x0D
#define ICM42688_GYRO_CONFIG_STATIC5       0x0E
#define ICM42688_GYRO_CONFIG_STATIC6       0x0F
#define ICM42688_GYRO_CONFIG_STATIC7       0x10
#define ICM42688_GYRO_CONFIG_STATIC8       0x11
#define ICM42688_GYRO_CONFIG_STATIC9       0x12
#define ICM42688_GYRO_CONFIG_STATIC10      0x13
#define ICM42688_XG_ST_DATA                0x5F
#define ICM42688_YG_ST_DATA                0x60
#define ICM42688_ZG_ST_DATA                0x61
#define ICM42688_TMSTVAL0                  0x62
#define ICM42688_TMSTVAL1                  0x63
#define ICM42688_TMSTVAL2                  0x64
#define ICM42688_INTF_CONFIG4              0x7A
#define ICM42688_INTF_CONFIG5              0x7B
#define ICM42688_INTF_CONFIG6              0x7C

#define ICM42688_SPI_W                     (0x00)
#define ICM42688_SPI_R                     (0x80)
//================================================定义 ICM42688 内部地址================================================

//================================================声明 ICM42688 全局变量================================================
extern int16 icm42688_gyro_x, icm42688_gyro_y, icm42688_gyro_z;                 // 三轴陀螺仪数据
extern int16 icm42688_acc_x,  icm42688_acc_y,  icm42688_acc_z;                  // 三轴加速度计数据
extern float icm42688_transition_factor[2];                                     // 转换实际值的比例
//================================================声明 ICM42688 全局变量================================================

//================================================声明 ICM42688 基础函数================================================
void    icm42688_get_acc            (void);                                     // 获取 ICM42688 加速度计数据
void    icm42688_get_gyro           (void);                                     // 获取 ICM42688 陀螺仪数据
uint8   icm42688_init               (void);                                     // 初始化 ICM42688
//================================================声明 ICM42688 基础函数================================================

//================================================声明 ICM42688 拓展函数================================================
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将 ICM42688 加速度计数据转换为实际物理数据
// 参数说明     acc_value       任意轴的加速度计数据
// 返回参数     float           转换后的物理数据
// 使用示例     float data = icm42688_acc_transition(icm42688_acc_x);           // 单位为 g(m/s^2)
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
#define icm42688_acc_transition(acc_value)      ((float)(acc_value) * icm42688_transition_factor[0])

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将 ICM42688 陀螺仪数据转换为实际物理数据
// 参数说明     gyro_value      任意轴的陀螺仪数据
// 返回参数     float           转换后的物理数据
// 使用示例     float data = icm42688_gyro_transition(icm42688_gyro_x);         // 单位为 °/s
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
#define icm42688_gyro_transition(gyro_value)    ((float)(gyro_value) * icm42688_transition_factor[1])
//================================================声明 ICM42688 拓展函数================================================

#endif
