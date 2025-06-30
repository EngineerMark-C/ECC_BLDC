/*
 * test4.h
 *
 *  Created on: 2025年5月28日
 *      Author: Ye
 */

#ifndef CODE_TEST44_H_
#define CODE_TEST44_H_

void recognize_command(const char* voice_input);
void complete_command(void);
void handle_open_high_beam(void);
void delay_ms(uint16_t ms);
void True_complete_command(void);

extern int8 completecommand_flag;
extern int8 voice_flag;
extern uint8_t timer_10ms_flag;
extern uint8_t arr_index;
extern uint8_t arr[20];



#endif /* CODE_TEST44_H_ */
