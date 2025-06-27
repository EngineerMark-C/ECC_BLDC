#include "init.h"

// 状态变量
float position[2] = {0};   // 东北天坐标系位置 (x,y)
// float velocity[2] = {0};   // 东北天坐标系速度 (vx, vy)

// // 上一次加速度值
// float last_ax = 0.0f;
// float last_ay = 0.0f;
// float last_vx = 0.0f;
// float last_vy = 0.0f;

// 初始化惯导系统
void INS_Init(void)
{
    memset(position, 0, sizeof(position));
    // memset(velocity, 0, sizeof(velocity));
}

// 速度位置更新
void Update_Position_Encoder(void)
{
    // 将角度转换为弧度
    float yaw_radian = ANGLE_TO_RAD(yaw);  
    
    // 使用偏航角进行航位推算
    float cos_yaw = cos(yaw_radian);
    float sin_yaw = sin(yaw_radian);
    
    // 速度积分得到位置
    position[0] += speed * cos_yaw * (1.0f / sampleFreq);
    position[1] += speed * sin_yaw * (1.0f / sampleFreq);

    // 立即进行边界安全检查
    Safety_Boundary_Check();
}

// void Update_Position_Imu(void)
// {
//     float yaw_radian = ANGLE_TO_RAD(yaw);
//     float cos_yaw = cos(yaw_radian);
//     float sin_yaw = sin(yaw_radian);

//     float acc_x = ax * cos_yaw - ay * sin_yaw;
//     float acc_y = ax * sin_yaw + ay * cos_yaw;

//     //printf("%.2f, %.2f\n", acc_x, acc_y);  // 打印加速度数据
//     // 3. 使用梯形积分法计算速度
//     velocity[0] += (acc_x + last_ax) * (1.0f / sampleFreq) * 0.5f;
//     velocity[1] += (acc_y + last_ay) * (1.0f / sampleFreq) * 0.5f;

//     // 4. 使用梯形积分法计算位置
//     position[0] += (velocity[0] + last_vx) * (1.0f / sampleFreq) * 0.5f;
//     position[1] += (velocity[1] + last_vy) * (1.0f / sampleFreq) * 0.5f;

//     // 5. 更新上一次的加速度值和速度值
//     last_ax = acc_x;
//     last_ay = acc_y;
//     last_vx = velocity[0];
//     last_vy = velocity[1];

//     // 6. 边界安全检查
//     Safety_Boundary_Check();
// }