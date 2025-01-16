/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_IWDG_H__
#define __APP_IWDG_H__

/* include */
#include "app_board.h"

/* macro */

/* type declaration */
typedef struct
{
    dev_state_e dev_state;
    rt_device_t wdg_dev;
}app_iwdg_env_t;

/* variable */
extern app_iwdg_env_t app_iwdg_env;

/* function */
void app_iwdg_init(rt_uint32_t timeout);
void app_feed_dogs(void);

#endif /*__APP_IWDG_H__*/
