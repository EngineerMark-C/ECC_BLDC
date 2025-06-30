#ifndef CODE_ASR_CTRL_H_
#define CODE_ASR_CTRL_H_

#define ASR_TARGET_IP                   "ws-api.xfyun.cn"
#define ASR_TARGET_PORT                 "80"
#define ASR_DOMAIN                      "wss://ws-api.xfyun.cn/v2/iat"
#define ASR_AUDIO_ADC                   ADC0_CH4_A4
// #define ASR_BUTTON                      P20_9
#define ASR_PIT                         CCU61_CH1

#define ASR_WIFI_SSID                   "ccc"                                   // wifi名称 wifi需要是2.4G频率
#define ASR_WIFI_PASSWORD               "1234567890"                            // wifi密码

#define ASR_APIID                       "5e78d438"                              // 讯飞的id
#define ASR_APISecret                   "Y2VhMWFjMTI0ZGI2ZjlhMmNjY2MzMTc5"      // 讯飞的Secret
#define ASR_APIKey                      "a63643713683a5d0a80f1af9c26759f5"      // 讯飞的Key

#define RANDOM_NUM_ADC                  ADC0_CH5_A5                             // 使用ADC生成随机数

#endif /* CODE_ASR_CTRL_H_ */
