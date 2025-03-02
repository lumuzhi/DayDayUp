/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_ADC_H__
#define __APP_ADC_H__

/* include */
#include "app_board.h"
/* macro */
/* macro */
#define APP_ADC_THREAD_PRO         19
#define APP_ADC_STACK              2048
#define APP_ADC_OSR                20   //采样率20Hz


/* type declaration */
typedef struct
{
    dev_state_e     dev_state;
//    rt_adc_device_t adc_dev;
    rt_thread_t     thread;
}app_adc_env_t;

/* variable */
extern app_adc_env_t app_adc_env;

/* function */
void app_adc_init(void);
void app_adc_thread(void);

#endif /*__APP_ADC_H__*/
