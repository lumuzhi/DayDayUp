/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_CAN1_H__
#define __APP_CAN1_H__

/* include */
#include "app_board.h"

/* macro */
#define APP_CAN1_RECV_THREAD_PRO      APP_BOARD_CAN1_RX_THREAD_PRO
#define APP_CAN1_RECV_STACK           APP_BOARD_CAN1_RX_STACK
#define APP_CAN1_DEAL_THREAD_PRO      APP_BOARD_CAN1_DEAL_THREAD_PRO
#define APP_CAN1_DEAL_STACK           APP_BOARD_CAN1_DEAL_STACK
#define APP_CAN1_SEND_THREAD_PRO      APP_BOARD_CAN1_TX_THREAD_PRO
#define APP_CAN1_SEND_STACK           APP_BOARD_CAN1_TX_STACK

/* type declaration */
typedef struct
{
    rt_thread_t recv_thread;
    rt_thread_t deal_thread;  //CAN1接收到数据处理线程
    rt_device_t dev_handle;
    dev_state_e dev_state;

    rt_uint16_t life_cnt;
}app_can1_env_t;

/* variable */
extern app_can1_env_t app_can1_env;

/* function */
void app_can1_init(void);
void app_can1_thread(void);
int app_can1_send(uint8_t board_id, uint8_t *data, uint16_t size);

#endif /*__APP_CAN1_H__*/



