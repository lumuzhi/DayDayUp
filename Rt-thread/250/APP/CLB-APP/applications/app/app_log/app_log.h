/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_LOG_H__
#define __APP_LOG_H__

/* include */
#include "app_board.h"
#include <elog.h>

/* macro */

/* type declaration */
typedef enum
{
    LOG_LVL_ASSERT = ELOG_LVL_ASSERT,
    LOG_LVL_ERROR  = ELOG_LVL_ERROR,
    LOG_LVL_WARN   = ELOG_LVL_WARN,
    LOG_LVL_INFO   = ELOG_LVL_INFO,
    LOG_LVL_DEBUG  = ELOG_LVL_DEBUG,
}log_lvl_e;
typedef enum
{
    LOG_SAVE_OPEN,
    LOG_SAVE_CLOSE,
}log_save_e;
typedef struct
{
    dev_state_e  dev_state;
    log_save_e   log_save;
}app_log_env_t;

/* variable */
extern app_log_env_t app_log_env;

/* function */
void app_log_init(void);
void app_log_read_data(uint8_t *data, uint16_t size);
void app_log_msg(log_lvl_e log_lvl, bool state, char *data);

#endif /* __APP_LOG_H__ */

