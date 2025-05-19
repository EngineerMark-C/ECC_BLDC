/*********************************************************************************************************************
* 修改记录
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

#include "zf_common_debug.h"
#include "zf_driver_delay.h"
#include "zf_driver_spi.h"
#include "zf_driver_gpio.h"
#include "zf_driver_soft_iic.h"
#include "icm42688.h"

int16 icm42688_gyro_x = 0, icm42688_gyro_y = 0, icm42688_gyro_z = 0;            // 三轴陀螺仪数据
int16 icm42688_acc_x = 0, icm42688_acc_y = 0, icm42688_acc_z = 0;               // 三轴加速度计数据
float icm42688_transition_factor[2];                                            // 转换实际值的比例

#if ICM42688_USE_SOFT_IIC
static soft_iic_info_struct icm42688_iic_struct;

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     ICM42688 写寄存器
// 参数说明     reg             寄存器地址
// 参数说明     data            数据
// 返回参数     void
// 使用示例     icm42688_write_register(ICM42688_PWR_MGMT0, 0x00);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
#define icm42688_write_register(reg, data)      (soft_iic_write_8bit_register(&icm42688_iic_struct, (reg), (data)))

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     ICM42688 读寄存器
// 参数说明     reg             寄存器地址
// 返回参数     uint8           数据
// 使用示例     icm42688_read_register(ICM42688_WHO_AM_I);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
#define icm42688_read_register(reg)             (soft_iic_read_8bit_register(&icm42688_iic_struct, (reg)))

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     ICM42688 读数据
// 参数说明     reg             寄存器地址
// 参数说明     data            数据缓冲区
// 参数说明     len             数据长度
// 返回参数     void
// 使用示例     icm42688_read_registers(ICM42688_ACCEL_DATA_X1, dat, 6);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
#define icm42688_read_registers(reg, data, len) (soft_iic_read_8bit_registers(&icm42688_iic_struct, (reg), (data), (len)))
#else
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     ICM42688 写寄存器
// 参数说明     reg             寄存器地址
// 参数说明     data            数据
// 返回参数     void
// 使用示例     icm42688_write_register(ICM42688_PWR_MGMT0, 0x00);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static void icm42688_write_register(uint8 reg, uint8 data)
{
    ICM42688_CS(0);
    spi_write_8bit_register(ICM42688_SPI, reg | ICM42688_SPI_W, data);
    ICM42688_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     ICM42688 读寄存器
// 参数说明     reg             寄存器地址
// 返回参数     uint8           数据
// 使用示例     icm42688_read_register(ICM42688_WHO_AM_I);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static uint8 icm42688_read_register(uint8 reg)
{
    uint8 data = 0;
    ICM42688_CS(0);
    data = spi_read_8bit_register(ICM42688_SPI, reg | ICM42688_SPI_R);
    ICM42688_CS(1);
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     ICM42688 读数据
// 参数说明     reg             寄存器地址
// 参数说明     data            数据缓冲区
// 参数说明     len             数据长度
// 返回参数     void
// 使用示例     icm42688_read_registers(ICM42688_ACCEL_DATA_X1, dat, 6);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static void icm42688_read_registers(uint8 reg, uint8 *data, uint32 len)
{
    ICM42688_CS(0);
    spi_read_8bit_registers(ICM42688_SPI, reg | ICM42688_SPI_R, data, len);
    ICM42688_CS(1);
}
#endif

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     ICM42688 自检
// 参数说明     void
// 返回参数     uint8           1-自检失败 0-自检成功
// 使用示例     icm42688_self_check();
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static uint8 icm42688_self_check(void)
{
    uint8 dat = 0, return_state = 0;
    uint16 timeout_count = 0;

    while(0x47 != dat)                                                          // 判断 ID 是否正确（ICM42688 WHO_AM_I寄存器应为0x47）
    {
        if(ICM42688_TIMEOUT_COUNT < timeout_count++)
        {
            return_state = 1;
            break;
        }
        dat = icm42688_read_register(ICM42688_WHO_AM_I);
        system_delay_ms(10);
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取 ICM42688 加速度计数据
// 参数说明     void
// 返回参数     void
// 使用示例     icm42688_get_acc();                                             // 执行该函数后，直接查看对应的变量即可
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void icm42688_get_acc(void)
{
    uint8 dat[6];

    icm42688_read_registers(ICM42688_ACCEL_DATA_X1, dat, 6);
    icm42688_acc_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    icm42688_acc_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    icm42688_acc_z = (int16)(((uint16)dat[4] << 8 | dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取 ICM42688 陀螺仪数据
// 参数说明     void
// 返回参数     void
// 使用示例     icm42688_get_gyro();                                            // 执行该函数后，直接查看对应的变量即可
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void icm42688_get_gyro(void)
{
    uint8 dat[6];

    icm42688_read_registers(ICM42688_GYRO_DATA_X1, dat, 6);
    icm42688_gyro_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    icm42688_gyro_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    icm42688_gyro_z = (int16)(((uint16)dat[4] << 8 | dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     初始化 ICM42688
// 参数说明     void
// 返回参数     uint8           1-初始化失败 0-初始化成功
// 使用示例     icm42688_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 icm42688_init(void)
{
    uint8 return_state = 0;
    // uint8 val = 0x0;
    // uint16 timeout_count = 0;

    system_delay_ms(10);                                                        // 上电延时

#if ICM42688_USE_SOFT_IIC
    soft_iic_init(&icm42688_iic_struct, ICM42688_DEV_ADDR, ICM42688_SOFT_IIC_DELAY, ICM42688_SCL_PIN, ICM42688_SDA_PIN);
#else
    spi_init(ICM42688_SPI, SPI_MODE0, ICM42688_SPI_SPEED, ICM42688_SPC_PIN, ICM42688_SDI_PIN, ICM42688_SDO_PIN, SPI_CS_NULL);
    gpio_init(ICM42688_CS_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
#endif

    do
    {
        if(icm42688_self_check())
        {
            // 如果程序在输出了断言信息 并且提示出错位置在这里
            // 那么就是 ICM42688 自检出错并超时退出了
            // 检查一下接线有没有问题 如果没问题可能就是坏了
            zf_log(0, "icm42688 self check error.");
            return_state = 1;
            break;
        }

        icm42688_write_register(ICM42688_PWR_MGMT0, 0x00);                      // 复位设备
        system_delay_ms(2);                                                    // 操作完PWR_MGMT0寄存器后200us内不能有任何读写寄存器的操作

        // do
        // {                                                                       // 等待复位成功
        //     val = icm42688_read_register(ICM42688_PWR_MGMT0);
        //     if(ICM42688_TIMEOUT_COUNT < timeout_count ++)
        //     {
        //         // 如果程序在输出了断言信息 并且提示出错位置在这里
        //         // 那么就是 ICM42688 自检出错并超时退出了
        //         // 检查一下接线有没有问题 如果没问题可能就是坏了
        //         zf_log(0, "ICM42688 reset error.");
        //         return_state = 1;
        //         break;
        //     }
        // }while(0x41 != val);
        // if(1 == return_state)
        // {
        //     break;
        // }
        // 设置ICM42688加速度计和陀螺仪的量程和输出速率
        icm42688_write_register(ICM42688_ACCEL_CONFIG0, 0x00);                  // 先进行默认配置
        icm42688_write_register(ICM42688_GYRO_CONFIG0, 0x00);                   // 先进行默认配置

        // 设置加速度计量程
        switch(ICM42688_ACC_SAMPLE_DEFAULT)
        {
            case ICM42688_ACC_SAMPLE_SGN_2G:
            {
                icm42688_write_register(ICM42688_ACCEL_CONFIG0, (3 << 5) | (ICM42688_ACC_RATE_DEFAULT + 1));
                icm42688_transition_factor[0] = 2.0f / 32768.0f;
            }break;
            case ICM42688_ACC_SAMPLE_SGN_4G:
            {
                icm42688_write_register(ICM42688_ACCEL_CONFIG0, (2 << 5) | (ICM42688_ACC_RATE_DEFAULT + 1));
                icm42688_transition_factor[0] = 4.0f / 32768.0f;
            }break;
            case ICM42688_ACC_SAMPLE_SGN_8G:
            {
                icm42688_write_register(ICM42688_ACCEL_CONFIG0, (1 << 5) | (ICM42688_ACC_RATE_DEFAULT + 1));
                icm42688_transition_factor[0] = 8.0f / 32768.0f;
            }break;
            case ICM42688_ACC_SAMPLE_SGN_16G:
            {
                icm42688_write_register(ICM42688_ACCEL_CONFIG0, (0 << 5) | (ICM42688_ACC_RATE_DEFAULT + 1));
                icm42688_transition_factor[0] = 16.0f / 32768.0f;
            }break;
            default:
            {
                zf_log(0, "ICM42688_ACC_SAMPLE_DEFAULT set error.");
                return_state = 1;
            }break;
        }
        if(1 == return_state)
        {
            break;
        }

        // 设置陀螺仪量程
        switch(ICM42688_GYRO_SAMPLE_DEFAULT)
        {
            case ICM42688_GYRO_SAMPLE_SGN_15_625DPS:
            {
                icm42688_write_register(ICM42688_GYRO_CONFIG0, (7 << 5) | (ICM42688_GYRO_RATE_DEFAULT + 1));
                icm42688_transition_factor[1] = 15.625f / 32768.0f;
            }break;
            case ICM42688_GYRO_SAMPLE_SGN_31_25DPS:
            {
                icm42688_write_register(ICM42688_GYRO_CONFIG0, (6 << 5) | (ICM42688_GYRO_RATE_DEFAULT + 1));
                icm42688_transition_factor[1] = 31.25f / 32768.0f;
            }break;
            case ICM42688_GYRO_SAMPLE_SGN_62_5DPS:
            {
                icm42688_write_register(ICM42688_GYRO_CONFIG0, (5 << 5) | (ICM42688_GYRO_RATE_DEFAULT + 1));
                icm42688_transition_factor[1] = 62.5f / 32768.0f;
            }break;
            case ICM42688_GYRO_SAMPLE_SGN_125DPS:
            {
                icm42688_write_register(ICM42688_GYRO_CONFIG0, (4 << 5) | (ICM42688_GYRO_RATE_DEFAULT + 1));
                icm42688_transition_factor[1] = 125.0f / 32768.0f;
            }break;
            case ICM42688_GYRO_SAMPLE_SGN_250DPS:
            {
                icm42688_write_register(ICM42688_GYRO_CONFIG0, (3 << 5) | (ICM42688_GYRO_RATE_DEFAULT + 1));
                icm42688_transition_factor[1] = 250.0f / 32768.0f;
            }break;
            case ICM42688_GYRO_SAMPLE_SGN_500DPS:
            {
                icm42688_write_register(ICM42688_GYRO_CONFIG0, (2 << 5) | (ICM42688_GYRO_RATE_DEFAULT + 1));
                icm42688_transition_factor[1] = 500.0f / 32768.0f;
            }break;
            case ICM42688_GYRO_SAMPLE_SGN_1000DPS:
            {
                icm42688_write_register(ICM42688_GYRO_CONFIG0, (1 << 5) | (ICM42688_GYRO_RATE_DEFAULT + 1));
                icm42688_transition_factor[1] = 1000.0f / 32768.0f;
            }break;
            case ICM42688_GYRO_SAMPLE_SGN_2000DPS:
            {
                icm42688_write_register(ICM42688_GYRO_CONFIG0, (0 << 5) | (ICM42688_GYRO_RATE_DEFAULT + 1));
                icm42688_transition_factor[1] = 2000.0f / 32768.0f;
            }break;
            default:
            {
                zf_log(0, "ICM42688_GYRO_SAMPLE_DEFAULT set error.");
                return_state = 1;
            }break;
        }
        if(1 == return_state)
        {
            break;
        }
        
        icm42688_write_register(ICM42688_PWR_MGMT0, 0x0F);                      // 设置GYRO_MODE,ACCEL_MODE为低噪声模式
        system_delay_ms(10);
    }while(0);
    
    return return_state;
}
