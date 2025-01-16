/**
 * @file
 * @details
 * @author
 * @date    LED灯
 * @version
**/

/* include */
#include "app_led.h"
#include "app_log.h"
#include "app_can.h"

/* macro */
#define APP_LED_TIME         5000  //5s

/* type declaration */
typedef struct
{
    led_mode_e mode;
}led_out_info_t;
typedef struct
{
    rt_timer_t     timer;
    led_out_info_t led_out_info[LED_MAX];
    rt_uint8_t     led_out[LED_MAX];
}app_led_local_t;

/* variable declaration */
app_led_local_t app_led_local;
#define local app_led_local

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_led_scan(void)
{
    static uint16_t norm_cnts[LED_MAX] = {0}, fast_cnts[LED_MAX] = {0};
    for(rt_uint8_t i = 0; i < LED_MAX; i++)
    {
        if(local.led_out_info[i].mode == NORM_FLICKER)
        {
            norm_cnts[i]++;
            if(!(norm_cnts[i] % 15)) {
                local.led_out[i] = ~local.led_out[i];
            }
        }
        else if(local.led_out_info[i].mode == FAST_FLICKER)
        {
            fast_cnts[i]++;
            if(!(fast_cnts[i] % 4)) {
                local.led_out[i] = ~local.led_out[i];
            }
        }
        else if(local.led_out_info[i].mode == ALWAY_BRIGHT) {
            local.led_out[i] = 1;
        }
        else if(local.led_out_info[i].mode == ALWAY_QUENCH) {
            local.led_out[i] = 0;
        }
        else {
            local.led_out[i] = 0;
        }
    }
    for(rt_uint8_t i = 0; i < LED_MAX; i++)
    {
        switch(i)
        {
            case LED_1:
                (local.led_out[i] == 0) ? rt_pin_write(APP_LED_1_PIN, PIN_HIGH) : rt_pin_write(APP_LED_1_PIN, PIN_LOW);
                break;
            case LED_2:
                (local.led_out[i] == 0) ? rt_pin_write(APP_LED_2_PIN, PIN_HIGH) : rt_pin_write(APP_LED_2_PIN, PIN_LOW);
                break;
            default:
                break;
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_led_set_mode(app_led_e app_led, led_mode_e mode)
{
    switch(app_led)
    {
        case LED_1:
            local.led_out_info[LED_1].mode = mode;
            break;
        case LED_2:
            local.led_out_info[LED_2].mode = mode;
            break;
        default:
            break;
    }
}

/**
 * @brief
 * @param
 * @return
 * @note   软件定时器回调函数
**/
static void app_led_timer(void* parameter)
{
    if(app_can_env.can_size)
    {
        app_can_env.can_size = 0;
        app_led_set_mode(CAN_LED, FAST_FLICKER);
    }
    else {
        app_led_set_mode(CAN_LED, ALWAY_QUENCH);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_led_init(void)
{
    local.timer = rt_timer_create("led_timer", app_led_timer, RT_NULL,
            APP_LED_TIME, RT_TIMER_FLAG_SOFT_TIMER | RT_TIMER_FLAG_PERIODIC);
    if(local.timer != RT_NULL) {
        rt_timer_start(local.timer);
    }
    else {
        app_log_msg(LOG_LVL_ERROR, true, "led soft time failed");
    }
    rt_pin_mode(APP_LED_1_PIN, PIN_MODE_OUTPUT);  //输出模式
    rt_pin_write(APP_LED_1_PIN, PIN_HIGH);
    rt_pin_mode(APP_LED_2_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(APP_LED_2_PIN, PIN_HIGH);

    for(uint8_t i = 0; i < LED_MAX; i++)
    {
        local.led_out_info[i].mode = ALWAY_QUENCH;
        local.led_out[i] = 0;
    }
    app_led_set_mode(SYS_LED, NORM_FLICKER);
    app_led_set_mode(CAN_LED, ALWAY_QUENCH);
}

