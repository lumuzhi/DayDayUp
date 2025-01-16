/**
 * @file
 * @details
 * @author
 * @date
 * @version
 *      log_a("Hello EasyLogger!");
        log_e("Hello EasyLogger!");
        log_w("Hello EasyLogger!");
        log_i("Hello EasyLogger!");
        log_d("Hello EasyLogger!");
        log_v("Hello EasyLogger!");
**/

/* include */
#include "app_log.h"
#include "app_easyflash.h"
#include "elog_flash.h"
#include "easyflash.h"
#include "app_scast.h"

/* macro */

/* type declaration */
typedef struct
{
    uint8_t rxmsg1;
}app_log_local_t;

/* variable declaration */
app_log_env_t app_log_env;
#define env app_log_env
app_log_local_t app_log_local;
#define local app_log_local

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_log_msg(log_lvl_e log_lvl, bool state, char *data)
{
    if((data != NULL) && (state == true)) {
        rt_kprintf("%s\n", data);
        return ;
    }
    if((data == NULL) || (env.dev_state != DEV_INIT_SUCCESS)) {
        return ;
    }
    for(uint8_t i = 0, count = 0; i < APP_TCPS_CONNECT_MAX; i++)
    {
        if(app_scast_env.tcps_info[i].connect != -1) {
            count++;
        }
        if(i == (APP_TCPS_CONNECT_MAX - 1))
        {
            if(!count) {
                env.log_save = LOG_SAVE_OPEN;
            }
            else if(env.log_save == LOG_SAVE_CLOSE) {
                return ;
            }
        }
    }
    switch(log_lvl)
    {
        case LOG_LVL_ASSERT:
            log_a(data);
            break;
        case LOG_LVL_ERROR:
            log_e(data);
            break;
        case LOG_LVL_WARN:
            log_w(data);
            break;
        case LOG_LVL_INFO:
            log_i(data);
            break;
        case LOG_LVL_DEBUG:
            log_d(data);
            break;
        default:
            break;
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_log_read_data(uint8_t *data, uint16_t size)
{
    if((data == NULL) || (size != 24) || (strncmp((const char *)data, "lhkj device log read clb", 24) != 0))
    {
        return ;
    }
    env.log_save = LOG_SAVE_CLOSE;
    elog_flash_output(0, ef_log_get_used_size());
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_log_init(void)
{
    env.dev_state = DEV_INIT_NULL;
    env.log_save = LOG_SAVE_OPEN;
    if(app_easyflash_env.dev_state == DEV_INIT_SUCCESS)
    {
        elog_init();
        elog_output_lock_enabled(true);
        elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL & ~(ELOG_FMT_T_INFO | ELOG_FMT_P_INFO | ELOG_FMT_TAG));
        elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_ALL & ~(ELOG_FMT_T_INFO | ELOG_FMT_P_INFO | ELOG_FMT_TAG));
        elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TIME);
        elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TIME);
        elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL);
        elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_T_INFO | ELOG_FMT_P_INFO | ELOG_FMT_TAG));
        elog_start();
        elog_flash_init();
        elog_flash_lock_enabled(true);
        env.dev_state = DEV_INIT_SUCCESS;
        app_log_msg(LOG_LVL_INFO, true, "app log init success");
    }
    else
    {
        env.dev_state = DEV_INIT_FAILED;
        rt_kprintf("app log init failed \n");
    }
}



