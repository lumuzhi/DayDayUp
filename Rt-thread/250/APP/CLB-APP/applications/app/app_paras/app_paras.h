/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_PARAS_H__
#define __APP_PARAS_H__

/* include */
#include "app_board.h"

/* macro */
#define APP_PARAS_THREAD_PRO      APP_BOARD_PARAS_THREAD_PRO
#define APP_PARAS_STACK           APP_BOARD_PARAS_STACK

/* type declaration */
typedef enum
{
    PARAS_INIT_NULL,
    PARAS_INIT_SUCCESS,
    PARAS_INIT_FAIL,
    PARAS_INIT_DEFAULT,    //初始化默认值
    PARAS_CONFIG_DOING,    //配置中
    PARAS_CONFIG_READY,    //配置就绪
}paras_state_e;
typedef enum
{
    DEV_MASTER,   //主机
    DEV_SLAVE,    //从机
}device_t;
typedef struct
{
    uint8_t  group_num;        //编组
    uint8_t  train_num;
    device_t device;
    uint8_t  local_ip[4];
    uint8_t  netmask[4];
    uint8_t  gateway[4];
    uint8_t  mcast_ip[4];      //组播
    uint8_t  mcast_port[2];
    uint8_t  mcast_raw_ip[4];      //组播，用于原始数据发送
    uint8_t  mcast_raw_port[2];
}sys_paras_t;
typedef struct
{
    uint8_t     head[2];
    sys_paras_t paras;
    uint8_t     check_sum;
}app_paras_t;
typedef struct
{
    uint8_t head[2];
    uint8_t train_num;
    uint8_t group_num;
    uint8_t check_sum;
}app_paras_save_t;
typedef struct
{
    rt_thread_t   thread;
    paras_state_e paras_state;
    app_paras_save_t  paras_inform;
}app_paras_env_t;

/* variable */
extern app_paras_t app_paras;
extern app_paras_env_t app_paras_env;

/* function */
void app_paras_init(void);
void app_paras_qzq_deal(uint8_t *data, uint16_t size);
void app_paras_config(uint8_t *data, uint16_t size);
void app_paras_qzq_config(uint8_t *data, uint16_t size);

/**
 * @brief
 * @param
 * @return
 * @note
**/
static inline sys_paras_t *app_paras_get(void)
{
    return &app_paras.paras;
}

#endif /*__APP_PARAS_H__*/
