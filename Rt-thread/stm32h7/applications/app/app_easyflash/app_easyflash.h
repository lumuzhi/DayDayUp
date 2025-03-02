/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_EASYFLASH_H__
#define __APP_EASYFLASH_H__

/* include */
#include "app_board.h"

/* macro */

/* type declaration */
typedef struct
{
    dev_state_e dev_state;

    rt_thread_t log_thread;
    rt_uint8_t  clean_flag;
}app_easyflash_env_t;

/* variable */
extern app_easyflash_env_t app_easyflash_env;

/* function */
void app_easyflash_init(void);
void ef_log_clean_thread(void);

#endif /*__APP_EASYFLASH_H__*/



