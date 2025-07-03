/*
 * test4.h
 *
 *  Created on: 2025年5月28日
 *      Author: Ye
 */

#ifndef TEST4_H_
#define TEST4_H_

#define MAX_COMMAND_BUFFER              20                                      // 最大命令缓冲区大小

//************************************全局变量声明****************************************//

// 命令缓冲区
extern uint8_t command_buffer[MAX_COMMAND_BUFFER];
extern uint8_t command_buffer_index;

// 定时器和状态标志
extern uint8_t timer_10ms_flag;
extern uint8_t command_complete_flag;
extern uint8_t voice_recognition_flag;

//************************************函数声明****************************************//
// 延时函数
void delay_ms(uint16_t ms);

// 语音识别处理函数
void Recognize_Voice_Command(const char* voice_input);
void Execute_Command_Buffer(void);
void Execute_Complete_Command(void);

// 灯光控制命令实现
void Handle_Open_Hazard(void);
void Handle_Open_Left_Turn(void);
void Handle_Open_Right_Turn(void);
void Handle_Open_Low_Beam(void);
void Handle_Open_High_Beam(void);
void Handle_Open_Fog_Light(void);

void Test4_Navigation(void);

// 运动控制命令实现
void Handle_Move_Forward_10M(void);
void Handle_Move_Backward_10M(void);
void Handle_Snake_Forward_10M(void);
void Handle_Snake_Backward_10M(void);
void Handle_Turn_Counterclock(void);
void Handle_Turn_Clockwise(void);

// 停车控制命令实现
void Handle_Parking_1(void);
void Handle_Parking_2(void);
void Handle_Parking_3(void);

#endif
