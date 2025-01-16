/**
 * @file
 * @details this is watchdog
 * @author
 * @date
 * @version
**/

/* include */
#include "app_iwdg.h"
#include "app_log.h"

/* macro */
#define WDT_DEVICE_NAME    "wdt"

/* variable declaration */
app_iwdg_env_t app_iwdg_env;
#define env app_iwdg_env

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_feed_dogs(void)
{
    if(env.dev_state == DEV_INIT_SUCCESS) {
        rt_device_control(app_iwdg_env.wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, RT_NULL);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_iwdg_init(rt_uint32_t timeout)
{
    env.dev_state = DEV_INIT_NULL;
    if((timeout == 0) || (timeout > 32))
    {
        app_log_msg(LOG_LVL_ERROR, true, "iwdg timeout set error, use default values timeout = 12");
        timeout = 12;
    }
    app_iwdg_env.wdg_dev = rt_device_find(WDT_DEVICE_NAME);
    if(app_iwdg_env.wdg_dev == RT_NULL)
    {
        env.dev_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "no find iwdg device");
        return ;
    }
    rt_device_init(app_iwdg_env.wdg_dev);
    rt_device_control(app_iwdg_env.wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    if(rt_device_control(app_iwdg_env.wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL) != RT_EOK)
    {
        env.dev_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "iwdg start failed");
        return ;
    }
    env.dev_state = DEV_INIT_SUCCESS;
    app_log_msg(LOG_LVL_INFO, true, "app iwdg init success");
}

