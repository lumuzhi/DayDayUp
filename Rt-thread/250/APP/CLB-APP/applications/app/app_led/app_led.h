/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_LED_H__
#define __APP_LED_H__

/* include */
#include "app_board.h"

/* macro */
#define APP_LED_1_PIN         GET_PIN(G, 8)
#define APP_LED_2_PIN         GET_PIN(B, 6)

/* type declaration */
typedef enum
{
    LED_1,
    LED_2,
    LED_MAX,
}led_x_e;
typedef enum
{
    NORM_FLICKER,   //正常闪烁
    FAST_FLICKER,   //快速闪烁
    ALWAY_BRIGHT,   //常亮
    ALWAY_QUENCH,   //常灭
}led_mode_e;
typedef enum
{
    SYS_LED = LED_1,      //系统运行灯，闪烁
    CAN_LED = LED_2,      //CAN通信灯，闪烁
}app_led_e;

/* variable */

/* function */
void app_led_init(void);
void app_led_scan(void);

#endif /*__APP_LED_H__*/
