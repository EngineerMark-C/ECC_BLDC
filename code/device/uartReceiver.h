#ifndef _uartReceiver_h_
#define _uartReceiver_h_

#define RECEIVER_MIDDLE                 1024                                    // 遥控器中位值
#define RECEIVER_CHANGE                 800                                     // 遥控器变化范围

void UartReceiver_Init(void);
void Remote_Control(void);

#endif
