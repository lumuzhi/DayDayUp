/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_rtc.h"
#include "app_mcast.h"
#include "app_log.h"

/* macro */
#define APP_RTC_REDUNDANCY        (3 * 60)    //时间冗余度3分钟

/* type declaration */

/* variable declaration */

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_rtc_get(local_time_t *local_time)
{
    time_t timer;
    struct tm *time_p;
    time(&timer);
    time_p = gmtime(&timer);
    local_time->year = time_p->tm_year + 1900;
    local_time->mon  = time_p->tm_mon + 1;
    local_time->day  = time_p->tm_mday;
    local_time->hour = time_p->tm_hour;
    local_time->min  = time_p->tm_min;
    local_time->sec  = time_p->tm_sec;
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_rtc_set(local_time_t *timer)
{
    time_t local_time, set_timer;
    struct tm time_p;
    if((timer->year < 1900) || (timer->mon < 1) || (timer->mon > 12) || (timer->day < 1) || \
            (timer->day > 31) || (timer->hour > 23) || (timer->min > 59) || (timer->sec > 59))
    {
        return;
    }
    time(&local_time);
    time_p.tm_year = timer->year - 1900;
    time_p.tm_mon  = timer->mon - 1;
    time_p.tm_mday = timer->day;
    time_p.tm_hour = timer->hour;
    time_p.tm_min  = timer->min;
    time_p.tm_sec  = timer->sec;
    set_timer = mktime(&time_p);
    uint8_t set_state = 0;
    if(local_time > set_timer)
    {
        if((local_time - set_timer) > APP_RTC_REDUNDANCY) {
            set_state = 1;
        }
    }
    if(set_timer > local_time)
    {
        if((set_timer - local_time) > APP_RTC_REDUNDANCY) {
            set_state = 1;
        }
    }
    if(set_state)
    {
        char data_info[100];
        local_time_t time_info;
        app_rtc_get(&time_info);
        memset(data_info, 0, sizeof(data_info));
        sprintf(data_info, "current time:%04d-%02d-%02d-%02d:%02d:%02d", time_info.year, \
                time_info.mon, time_info.day, time_info.hour, time_info.min, time_info.sec);
        app_log_msg(LOG_LVL_INFO, true, data_info);
        memset(data_info, 0, sizeof(data_info));
        sprintf(data_info, "set time:%04d-%02d-%02d-%02d:%02d:%02d", timer->year, \
                timer->mon, timer->day, timer->hour, timer->min, timer->sec);
        app_log_msg(LOG_LVL_INFO, true, data_info);
        if(set_date(timer->year, timer->mon, timer->day) != RT_EOK) {
            app_log_msg(LOG_LVL_ERROR, true, "set date failed");
        }
        if(set_time(timer->hour, timer->min, timer->sec) != RT_EOK) {
            app_log_msg(LOG_LVL_ERROR, true, "set time failed");
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_rtc_update(uint16_t count)
{
    if(!(count % 100))  //每10秒更新一次
    {
#define mvb_info app_mcast_env.txb_info.trust_mvb
        if(mvb_info->com_state)
        {
            ccu_public_t * public_p = (ccu_public_t *)mvb_info->data;
            if(public_p->valid_flag.bits.time_valid)  //时间有效
            {
                local_time_t set_time;
                set_time.year = 2000 + public_p->year;
                set_time.mon = public_p->mon;
                set_time.day = public_p->day;
                set_time.hour = public_p->hour;
                set_time.min = public_p->min;
                set_time.sec = public_p->sec;
                app_rtc_set(&set_time);
            }
        }
#undef mvb_info
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_rtc_init(void)
{
    ;
}





