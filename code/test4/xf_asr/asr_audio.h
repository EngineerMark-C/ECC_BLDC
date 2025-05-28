#include "zf_common_headfile.h"
#include "hmac_sha256.h" 
#include "base64.h"  
#include "asr_ctrl.h"

#define MAX_WORD_LENGTH 256 // 最大单词长度

void audio_init(void);
void audio_callback(void);
void audio_loop(void);


extern uint8 audio_need_net_flag;

extern char words[][MAX_WORD_LENGTH];
extern char w_value[256];
