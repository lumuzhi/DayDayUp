/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_PERIOD_H__
#define __APP_PERIOD_H__

/* include */
#include "app_board.h"
#include "clb_protocol.h"

/* macro */
#define APP_PERIOD_THREAD_PRO      APP_BOARD_PERIOD_THREAD_PRO
#define APP_PERIOD_STACK           APP_BOARD_PERIOD_STACK
#define APP_PERIOD_TEMP_OFFSET     APP_BOARD_TEMP_OFFSET

/* type declaration */
typedef struct
{
    uint16_t wheel;
    uint16_t speed;
    uint16_t now_id;
    uint16_t next_id;
}clb_info_t;
typedef struct
{
    rt_thread_t thread;
    clb_info_t  clb_info;
    uint8_t     clb_data[sizeof(clb_port_info_t)];

    rt_uint16_t life_cnt;
}app_period_env_t;

/* variable */
extern app_period_env_t app_period_env;

/* function */
void app_period_init(void);
void app_period_thread(void);
void app_period_clb_diag_default(uint8_t *data, uint16_t size);
void app_period_clb_diag_default_qzq(uint8_t *data, uint16_t size, uint8_t qzq_num);

#endif /*__APP_PERIOD_H__*/



