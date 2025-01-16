/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_RTC_H__
#define __APP_RTC_H__

/* include */
#include "app_board.h"

/* macro */

/* type declaration */
typedef struct
{
    uint16_t year;
    uint16_t mon;
    uint16_t day;
    uint16_t hour;
    uint16_t min;
    uint16_t sec;
}local_time_t;

/* variable */

/* function declaration */
void app_rtc_init(void);
void app_rtc_get(local_time_t *local_time);
void app_rtc_set(local_time_t *timer);
void app_rtc_update(uint16_t count);

#endif /*__APP_RTC_H__*/

