/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __QZQ_PROTOCOL_H__
#define __QZQ_PROTOCOL_H__

/* include */
#include "app_board.h"

/* macro */
#define TEMP_OSR                10   //采样率10Hz

/* type declaration */
typedef enum
{
    ADG1609_CH_1,
    ADG1609_CH_2,
    ADG1609_CH_3,
    ADG1609_CH_4,

    ADG1609_CH_MAX,
}adg1609_ch_e;
typedef enum
{
    TEMP_ENV,
    TEMP_PT1,
    TEMP_PT2,
    TEMP_PT3,
    TEMP_PT4,

    TEMP_MAX,
}temp_ch_e;
typedef enum
{
    TEMP_INIT_NULL,
    TEMP_INFO_ERROR,
    TEMP_INFO_OK,
    TEMP_DEV_ERROR,
}qzq_temp_state_e;
typedef struct
{
    uint8_t adc_data[2];
}qzq_adc_t;
typedef struct
{
    qzq_temp_state_e state;
    int8_t           temp_data[TEMP_OSR];
}qzq_temp_t;
typedef struct
{
    uint8_t    id;
    qzq_adc_t  power_12;                 //放大了100倍
    qzq_adc_t  power_5;                  //放大了100倍
    qzq_adc_t  vibrate[ADG1609_CH_MAX];  //放大了100倍
    qzq_temp_t temp[TEMP_MAX];
}qzq_msg_t;
typedef struct
{
    uint8_t head[2];
    uint8_t size[2];   //除head、size、check外的字节数
    uint8_t check;
    uint8_t data[sizeof(qzq_msg_t)];
}qzq_info_t;


#endif /*__QZQ_PROTOCOL_H__*/





