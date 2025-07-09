#include "zf_common_headfile.h"
#include "hmac_sha256.h" 
#include "base64.h"  
#include "asr_ctrl.h"

#define MAX_WORD_LENGTH 256 // 最大单词长度

void audio_init(void);
void audio_callback(void);
void audio_loop(void);


extern uint8 audio_need_net_flag;
extern uint8 audio_start_flag;
extern uint8 audio_server_link_flag;
extern uint8 audio_send_data_flag;
extern uint32 asr_max_time;
extern int audio_get_count;

extern char words[][MAX_WORD_LENGTH];
extern char w_value[256];
