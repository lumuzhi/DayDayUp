/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_NET_H__
#define __APP_NET_H__

/* include */
#include "app_board.h"

/* macro */

/* type declaration */
typedef enum
{
    NET_STATE_OK,
    NET_STATE_FAIL,
}net_state_e;
typedef struct
{
    uint16_t net_size; //网络LED灯使用
}app_net_env_t;

/* variable */
extern app_net_env_t app_net_env;

/* function declaration */
void app_net_init(void);
void app_net_thread(void);

#endif /*__APP_NET_H__*/

