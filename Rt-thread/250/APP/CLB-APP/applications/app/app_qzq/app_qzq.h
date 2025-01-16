/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_QZQ_H__
#define __APP_QZQ_H__

/* include */
#include "app_board.h"
#include "clb_protocol.h"
#include "qzq_protocol.h"

/* macro */
#define APP_QZQ_THREAD_PRO          APP_BOARD_QZQ_THREAD_PRO
#define APP_QZQ_STACK               APP_BOARD_QZQ_STACK

/* type declaration */
typedef enum
{
    TEMP_NORMAL,   //温度正常
    TEMP_WARN,     //温度预警
    TEMP_ALARM,    //温度报警
}temp_state_e;
typedef struct
{
    uint16_t recv_count;
    uint8_t  data[sizeof(qzq_msg_t)];
}can_qzq_info_t;
typedef struct
{
    temp_state_e temp_state;
    int8_t       temp_data;
}qzq_temp_diag_t;
typedef struct
{
    uint8_t         com_state;
    uint8_t         vib_state[ADG1609_CH_MAX];   //前置器采集的振动数据得到振动状态，1-正常，0-异常
    qzq_temp_diag_t temp_diag[TEMP_MAX];
    uint8_t         cgq_state[ADG1609_CH_MAX];   //上报传感器的综合状态，1-正常，0-异常
}qzq_data_t;
typedef struct
{
    rt_thread_t    thread;
    can_qzq_info_t can_qzq_info[QZQ_MAX];
    qzq_data_t     qzq_data[QZQ_MAX];

    rt_uint16_t    life_cnt;
}app_qzq_env_t;

/* variable */
extern app_qzq_env_t app_qzq_env;

/* function */
void app_qzq_init(void);
void app_qzq_thread(void);
void app_qzq_data_deal(uint8_t *data, uint16_t size);

#endif /*__APP_QZQ_H__*/
