/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_CAN2_H__
#define __APP_CAN2_H__

/* include */
#include "app_board.h"
#include "app_can1.h"

/* macro */
#define CAN_BASE                      20

#define APP_CAN2_RECV_THREAD_PRO      APP_BOARD_CAN2_RX_THREAD_PRO
#define APP_CAN2_RECV_STACK           APP_BOARD_CAN2_RX_STACK
#define APP_CAN2_DEAL_THREAD_PRO      APP_BOARD_CAN2_DEAL_THREAD_PRO
#define APP_CAN2_DEAL_STACK           APP_BOARD_CAN2_DEAL_STACK
#define APP_CAN2_SEND_THREAD_PRO      APP_BOARD_CAN2_TX_THREAD_PRO
#define APP_CAN2_SEND_STACK           APP_BOARD_CAN2_TX_STACK

/* type declaration */
typedef struct
{
    dev_state_e dev_state;
    rt_device_t dev_handle;
    rt_thread_t recv_thread;
    rt_thread_t deal_thread;  //CAN2接收到数据处理线程

    rt_uint16_t life_cnt;
}app_can2_env_t;

/* variable */
extern app_can2_env_t app_can2_env;

/* function */
void app_can2_init(void);
void app_can2_thread(void);
int app_can2_send(uint8_t board_id, uint8_t *data, uint16_t size);

#endif /*__APP_CAN2_H__*/



