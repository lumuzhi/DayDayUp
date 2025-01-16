/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_hw_timer.h"
#include "app_led.h"
#include "bsp_fsmc.h"
#include "app_ad7606.h"
#include "app_iwdg.h"
#include "app_log.h"

/* macro */
#define HWTIMER_TIMER3_DEVICE_NAME    "timer3"
#define HWTIMER_TIMER4_DEVICE_NAME    "timer4"

/* type declaration */

/* variable */
app_hw_timer_env_t app_hw_timer_env;
#define env app_hw_timer_env


/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
static rt_err_t app_hwtimer3_timeout_cb(rt_device_t dev, rt_size_t size)
{
    app_led_scan();
//    app_feed_dogs();
    return 0;
}

/**
 * @brief
 * @param  sec-秒    usec-微秒(us)
 * @return
 * @note
**/
static void app_hwtimer3_init(rt_uint32_t sec, rt_uint32_t usec)
{
    rt_err_t ret = RT_EOK;
    rt_hwtimer_mode_t mode = HWTIMER_MODE_PERIOD;
    rt_uint32_t freq = 10000;
    rt_hwtimerval_t timeout_s;

    env.dev_timer3_state = DEV_INIT_NULL;
    env.dev_timer3 = rt_device_find(HWTIMER_TIMER3_DEVICE_NAME);
    if(env.dev_timer3 == RT_NULL)
    {
        env.dev_timer3_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "no find timer3 device");
        return ;
    }
    ret = rt_device_open(env.dev_timer3, RT_DEVICE_OFLAG_RDWR);
    if(ret != RT_EOK)
    {
        env.dev_timer3_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "open timer3 device failed");
        return ;
    }
    rt_device_set_rx_indicate(env.dev_timer3, app_hwtimer3_timeout_cb);
    rt_device_control(env.dev_timer3, HWTIMER_CTRL_FREQ_SET, &freq);
    ret = rt_device_control(env.dev_timer3, HWTIMER_CTRL_MODE_SET, &mode);
    if(ret != RT_EOK)
    {
        env.dev_timer3_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "set timer3 mode failed");
        return ;
    }
    timeout_s.sec = sec;
    timeout_s.usec = usec;
    if(rt_device_write(env.dev_timer3, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        env.dev_timer3_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "set timer3 timeout value failed");
        return ;
    }
    env.dev_timer3_state = DEV_INIT_SUCCESS;
    app_log_msg(LOG_LVL_INFO, true, "app timer3 init success");
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static rt_err_t hw_timer4_timeout_cb(rt_device_t dev, rt_size_t size)
{
    rt_base_t level = rt_hw_interrupt_disable();
    if(app_ad7606_env.ad7606_state == DEV_INIT_SUCCESS) {
        app_ad7606_read_adc();
    }
    rt_hw_interrupt_enable(level);
    return 0;
}

/**
 * @brief
 * @param  sec-秒    usec-微秒(us)
 * @return
 * @note
**/
void app_hw_timer4_start(void)
{
    rt_err_t ret = RT_EOK;
    rt_hwtimer_mode_t mode = HWTIMER_MODE_PERIOD;
    rt_uint32_t freq = 1000000;
    rt_hwtimerval_t timeout_s;

    env.dev_timer4_state = DEV_INIT_NULL;
    env.dev_timer4 = rt_device_find(HWTIMER_TIMER4_DEVICE_NAME);
    if(env.dev_timer4 == RT_NULL)
    {
        env.dev_timer4_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "no find timer4 device");
        return ;
    }
    ret = rt_device_open(env.dev_timer4, RT_DEVICE_OFLAG_RDWR);
    if(ret != RT_EOK)
    {
        env.dev_timer4_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "open timer4 device failed");
        return ;
    }
    rt_device_set_rx_indicate(env.dev_timer4, hw_timer4_timeout_cb);
    rt_device_control(env.dev_timer4, HWTIMER_CTRL_FREQ_SET, &freq);
    ret = rt_device_control(env.dev_timer4, HWTIMER_CTRL_MODE_SET, &mode);
    if(ret != RT_EOK)
    {
        env.dev_timer4_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "set timer4 mode failed");
        return ;
    }
    timeout_s.sec = 0;
    timeout_s.usec = 244;   //0, 244  4K采样率
    if(rt_device_write(env.dev_timer4, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        env.dev_timer4_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "set timer4 timeout value failed");
        return ;
    }
    env.dev_timer4_state = DEV_INIT_SUCCESS;
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_hw_timer4_stop(void)
{
    if((env.dev_timer4 != RT_NULL) && (env.dev_timer4_state == DEV_INIT_SUCCESS))
    {
        rt_device_close(env.dev_timer4);
        env.dev_timer4_state = DEV_INIT_NULL;
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_hw_timer_init(void)
{
    app_hwtimer3_init(0, 10000);
    app_hw_timer4_start();
    if(env.dev_timer4_state == DEV_INIT_SUCCESS) {
        app_log_msg(LOG_LVL_INFO, true, "app timer4 init success");
    }
}

