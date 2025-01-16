/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_ADG1608_H__
#define __APP_ADG1608_H__

/* include */
#include "app_board.h"

/* macro */
#define APP_ADG1608_2_EN_PIN         GET_PIN(B, 14)
#define APP_ADG1608_2_A0_PIN         GET_PIN(B, 15)
#define APP_ADG1608_2_A1_PIN         GET_PIN(B, 1)
#define APP_ADG1608_2_A2_PIN         GET_PIN(A, 3)
#define APP_ADG1608_1_EN_PIN         GET_PIN(E, 5)
#define APP_ADG1608_1_A0_PIN         GET_PIN(E, 6)
#define APP_ADG1608_1_A1_PIN         GET_PIN(E, 2)
#define APP_ADG1608_1_A2_PIN         GET_PIN(C, 13)

/* type declaration */
typedef enum
{
    ADG1608_CH_1,
    ADG1608_CH_2,
    ADG1608_CH_3,
    ADG1608_CH_4,
    ADG1608_CH_5,
    ADG1608_CH_6,
    ADG1608_CH_7,
    ADG1608_CH_8,
    ADG1608_CH_MAX,
}adg1608_ch_e;
typedef enum
{
    AXLE_1   = ADG1608_CH_1,   //1位轴端
    AXLE_2   = ADG1608_CH_2,
    AXLE_3   = ADG1608_CH_3,
    AXLE_4   = ADG1608_CH_4,
    AXLE_5   = ADG1608_CH_5,
    AXLE_6   = ADG1608_CH_6,
    AXLE_7   = ADG1608_CH_7,
    AXLE_8   = ADG1608_CH_8,
    AXLE_MAX = ADG1608_CH_MAX,
}adg1608_ch1_e;
typedef enum
{
    AXLE_GEAR_1  = ADG1608_CH_1,   //1轴齿轮
    AXLE_MOTOR_1 = ADG1608_CH_2,   //1轴电机
    AXLE_GEAR_2  = ADG1608_CH_3,
    AXLE_MOTOR_2 = ADG1608_CH_4,
    AXLE_GEAR_3  = ADG1608_CH_5,
    AXLE_MOTOR_3 = ADG1608_CH_6,
    AXLE_GEAR_4  = ADG1608_CH_7,
    AXLE_MOTOR_4 = ADG1608_CH_8,
    AXLE_GM_MAX  = ADG1608_CH_MAX,
}adg1608_ch2_e;
typedef struct
{
    adg1608_ch_e  current_ch;
}app_adg1608_env_t;

/* variable */
extern app_adg1608_env_t app_adg1608_env;

/* function */
void app_adg1608_init(void);
void app_adg1608_1_switch(adg1608_ch_e adg1608_ch);
void app_adg1608_2_switch(adg1608_ch_e adg1608_ch);

#endif /*__APP_ADG1608_H__*/
