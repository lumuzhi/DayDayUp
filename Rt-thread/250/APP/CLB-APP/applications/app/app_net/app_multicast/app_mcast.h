/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_MCAST_H__
#define __APP_MCAST_H__

/* include */
#include "app_board.h"
#include "txb_protocol.h"
#include "trdp_protocol.h"

/* macro */
#define APP_MULT_RECV_THREAD_PRO          APP_BOARD_MULT_RX_THREAD_PRO
#define APP_MULT_RECV_STACK               APP_BOARD_MULT_RX_STACK

/* type declaration */
//typedef struct
//{
//    uint8_t    com_state;  //通信状态　1-正常 0-异常
//    uint16_t   recv_count; //接收计数
//    uint8_t    data_state; //数据有效　1-有效 0-异常
//    uint8_t    train_num;
//    uint8_t    data[sizeof(port_offset_info_t) * MVB_SLINK_MAX_PORT];
//}mcast_txb_mvb_msg_t;
typedef struct
{
    uint8_t    com_state;  //通信状态　1-正常 0-异常
    uint16_t   recv_count; //接收计数
    uint8_t    data_state; //数据有效　1-有效 0-异常
    uint8_t    train_num;
    uint8_t    data[sizeof(ccu_public_t)];
}mcast_txb_public_msg_t;
typedef struct
{
    mcast_txb_public_msg_t mvb_info[TRAIN_MAX][TRDP_DEV_MAX];
    //MVB信任
    mcast_txb_public_msg_t *trust_mvb;
}mcast_txb_info_t;
typedef struct
{
    dev_state_e        dev_state;
    int                sock_handle;
    struct sockaddr_in sock_addr;
    rt_thread_t        recv_thread;
    mcast_txb_info_t   txb_info;
}app_mcast_env_t;

/* variable */
extern app_mcast_env_t app_mcast_env;

/* function */
void app_mcast_init(void);
void app_mcast_thread(void);
void app_mcast_send_data(void *data, rt_uint16_t size);
void app_mcast_send_raw_data(void *data, rt_uint16_t size);

#endif /*__APP_MCAST_H__*/
