/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-04-02     Administrator       the first version
 */
#ifndef _APP_NEW_QZQ_H_
#define _APP_NEW_QZQ_H_

#include "app_board.h"




typedef struct
{
    dev_state_e dev_state;
    bool        com_state;    //通信状态 true=升级前置器
}app_new_qzq_env_t;



extern app_new_qzq_env_t app_new_qzq_env;

/* function */
void app_new_qzq_init(void);
void app_update_qzq_data_for_new(uint8_t *data, uint16_t size);
//void app_update_qzq_data_deal(uint8_t *data, uint16_t size);

void app_qzq_can1_recv_for_new(struct rt_can_msg rxmsg);
void app_qzq_can2_recv_for_new(struct rt_can_msg rxmsg);



#endif /* _APP_NEW_OLD_H_ */
