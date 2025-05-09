#include "zf_common_headfile.h"
#include "init.h"

void IPS_Init(void)
{
    ips114_init();
    ips114_clear();
    ips114_set_color(RGB565_BLACK, RGB565_WHITE);
    system_delay_ms(200);  // 等待屏幕稳定
}
