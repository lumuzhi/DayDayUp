/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_SCAST_H__
#define __APP_SCAST_H__

/* include */
#include "app_board.h"

/* macro */
#define APP_TCPS_PORT                 8000   //TCP服务端端口号
#define APP_TCPS_RECV_MAX             1536
#define APP_TCPS_CONNECT_MAX          1      //1个连接,TCPS多连接目前还有点问题
#define APP_UDP_PORT                  9000

#define APP_TCPS_THREAD_PRO           APP_BOARD_TCPS_THREAD_PRO
#define APP_TCPS_STACK                APP_BOARD_TCPS_STACK

#define APP_UDP_THREAD_PRO            APP_BOARD_UDP_THREAD_PRO
#define APP_UDP_STACK                 APP_BOARD_UDP_STACK

/* type declaration */
typedef struct
{
    int                connect;
    fd_set             readset;
    fd_set             readset_client;
    struct sockaddr_in client_addr;
    uint8_t            *recv_data;
}tcps_info_t;
typedef struct
{
    dev_state_e        tcps_dev_state;
    rt_thread_t        tcps_thread;
    tcps_info_t        tcps_info[APP_TCPS_CONNECT_MAX];
    rt_thread_t        tcpc_thread;
    dev_state_e        udp_dev_state;
    int                udp_sock;
    rt_thread_t        udp_thread;
}app_scast_env_t;

/* variable */
extern app_scast_env_t app_scast_env;

/* function */
void app_scast_init(void);
void app_scast_thread(void);
void app_tcps_send_data(uint8_t *data, uint16_t size);
int app_udp_send_data(uint8_t *data, uint16_t size, struct sockaddr_in *sockaddr);

#endif /*__APP_SCAST_H__*/
