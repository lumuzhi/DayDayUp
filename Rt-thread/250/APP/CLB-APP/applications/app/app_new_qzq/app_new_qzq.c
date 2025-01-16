/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-04-02     Administrator       the first version
 */
#include "app_new_qzq.h"
#include "app_can.h"
#include "app_can1.h"
#include "app_can2.h"
#include "app_log.h"
#include "app_scast.h"
#include "app_qzq.h"




/* type declaration */
typedef struct
{
    uint16_t count;
    uint8_t *data;
}qzq_buff_t , *qzq_buff_p;

typedef struct
{
    qzq_buff_t qzq_buff[QZQ_MAX];
}app_new_qzq_local_t;
/* variable declaration */
app_new_qzq_local_t app_new_qzq_local;
#define local app_new_qzq_local
app_new_qzq_env_t app_new_qzq_env;
#define env app_new_qzq_env


#define QZQ_CAN1_RECV_MAX_SIZE    256
#define QZQ_CAN2_RECV_MAX_SIZE    256

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_update_qzq_data_for_new(uint8_t *data, uint16_t size)
{
    if((data == RT_NULL) || (size == 0) || \
            env.dev_state == DEV_INIT_FAILED)
    {
        return ;
    }
    if(*(uint16_t *)data == ntoh16(0xAF01))
    {
        size -= 2;
        env.com_state = true;
        app_can1_send(QZQ1_BOARD, &data[2], size);
        app_can2_send(QZQ1_BOARD, &data[2], size);
        return ;
    }
    else if(*(uint16_t *)data == ntoh16(0xAF02))
    {
        size -= 2;
        env.com_state = true;
        app_can1_send(QZQ2_BOARD, &data[2], size);
        app_can2_send(QZQ2_BOARD, &data[2], size);
        return ;
    }
    else if(*(uint16_t *)data == ntoh16(0xAF03))
    {
        size -= 2;
        env.com_state = true;
        app_can1_send(QZQ3_BOARD, &data[2], size);
        app_can2_send(QZQ3_BOARD, &data[2], size);
        return ;
    }
    else if(*(uint16_t *)data == ntoh16(0xAF04))
    {
        size -= 2;
        env.com_state = true;
        app_can1_send(QZQ4_BOARD, &data[2], size);
        app_can2_send(QZQ4_BOARD, &data[2], size);
        return ;
    }
    else {
        return ;
    }
}
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_qzq_can1_recv_for_new(struct rt_can_msg rxmsg)
{
    can_id_u can_id;
    can_id.word = rxmsg.id;
    if(can_id.bits.target == CLB_BOARD)
    {
        qzq_buff_t *cur_qzq;
        if(can_id.bits.source < 0 || can_id.bits.source > 4)
            return ;

        if(can_id.bits.source == 2)
        {
            rt_kprintf("qzq2222222222222222222qzq\n");
            rt_kprintf("head = 0x%x type = %d len = %d\n",*(uint16_t *)rxmsg.data, can_id.bits.type, rxmsg.len);
            for(uint8_t k = 0; k < 8; k++)
            {
                rt_kprintf("%x ", rxmsg.data[k]);
            }
            rt_kprintf("\n");
        }
        cur_qzq = &local.qzq_buff[can_id.bits.source - 1];

        switch(can_id.bits.type)
        {
        case SINGLE_FRAME:
            cur_qzq->count = 0;
            rt_memset(&cur_qzq->data[cur_qzq->count], 0, QZQ_CAN1_RECV_MAX_SIZE);
            rt_memmove(&cur_qzq->data[0], rxmsg.data, rxmsg.len);
            cur_qzq->count = rxmsg.len;
            if(*(uint16_t *)&cur_qzq->data[0] == ntoh16(0xAFCB))
                app_tcps_send_data(&cur_qzq->data[0], cur_qzq->count);
            else{
                rt_kprintf("33333333333333\n");
            }
            for(uint8_t k = 0; k < 8; k++)
            {
                rt_kprintf("%x ", cur_qzq->data[k]);
            }
            rt_kprintf("\n");
            break;
        case BEGIN_FRAME:
            cur_qzq->count = 0;
            rt_memset(&cur_qzq->data[cur_qzq->count], 0, QZQ_CAN1_RECV_MAX_SIZE);
            rt_memcpy(&cur_qzq->data[cur_qzq->count], rxmsg.data, rxmsg.len);
            cur_qzq->count = rxmsg.len;
            break;
        case MIDDLE_FRAME:
            if((cur_qzq->count + rxmsg.len) <= QZQ_CAN1_RECV_MAX_SIZE)
            {
                rt_memcpy(&cur_qzq->data[cur_qzq->count], rxmsg.data, rxmsg.len);
                cur_qzq->count += rxmsg.len;
            }
            break;
        case FINISH_FRAME:
            if((cur_qzq->count + rxmsg.len) <= QZQ_CAN1_RECV_MAX_SIZE)
            {
                rt_memcpy(&cur_qzq->data[cur_qzq->count], rxmsg.data, rxmsg.len);
                cur_qzq->count += rxmsg.len;
                if(*(uint16_t *)cur_qzq->data == ntoh16(0xAFCB))
                {
                    app_tcps_send_data(cur_qzq->data, cur_qzq->count);
                }
                else if(*(uint16_t *)cur_qzq->data == ntoh16(0xF304))
                {
                    app_qzq_data_deal(cur_qzq->data, cur_qzq->count);
                }
                cur_qzq->count = 0;
            }
            break;
        default:
            break;
        }
    }
}
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_new_qzq_init()
{
    env.com_state = false;

    for(int i = 0; i < QZQ_MAX; i++)
    {
        local.qzq_buff[i].count = 0;
        local.qzq_buff[i].data = rt_malloc(QZQ_CAN1_RECV_MAX_SIZE);
        if(local.qzq_buff[i].data == RT_NULL)
        {
            rt_kprintf("qzq buff malloc failed \n");
        }
    }

}
