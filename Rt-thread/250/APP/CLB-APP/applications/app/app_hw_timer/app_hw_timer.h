/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_HW_TIMER_H__
#define __APP_HW_TIMER_H__

/* include */
#include "app_board.h"

/* macro */

/* type declaration */
typedef struct
{
    rt_device_t dev_timer3;
    dev_state_e dev_timer3_state;
    rt_device_t dev_timer4;
    dev_state_e dev_timer4_state;
}app_hw_timer_env_t;

/* variable */
extern app_hw_timer_env_t app_hw_timer_env;

/* function */
void app_hw_timer_init(void);
void app_hw_timer4_start(void);
void app_hw_timer4_stop(void);

#endif /*__APP_HW_TIMER_H__*/
