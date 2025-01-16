/**
 * @file
 * @details  这个文件主要用于在新版应用，使用旧版的底包，升级应用程序，流程：发送重启命令让前置器重启进入底包，
 *           在底包中，用新版CAN驱动将就底包的CAN通信格式，发送升级准备命令，发送升级文件，升级完成。如果使用，
 *           新版的底包这些文件都不要，使用新版的升级程序就行。
 * @author
 * @date
 * @version
**/

/* include */
#include "app_can1.h"
#include "app_can2.h"
#include "app_can.h"
#include "app_log.h"
#include "app_scast.h"
#include "app_qzq_can.h"
#include "app_qzq.h"

/* macro */
#define QZQ_CAN1_RECV_BUFF_SIZE    256
#define QZQ_CAN2_RECV_BUFF_SIZE    256

#define QZQ_SYSTEM_RESET_CMD       0
#define QZQ_SYSTEM_RESET_LEN       12
#define QZQ_BOARD_NUMB          0x20

/* type declaration */
typedef struct
{
    uint8_t *can1_recv_buff;   //CAN1接收到数据组包buff
    uint8_t *can2_recv_buff;
    qzqid_deal_t can1_qzq_deal;
    qzqid_deal_t can2_qzq_deal;
}app_qzq_can_local_t;

/* variable declaration */
app_qzq_can_local_t app_qzq_can_local;
#define local app_qzq_can_local
app_qzq_can_env_t app_qzq_can_env;
#define env app_qzq_can_env

/* function declaration */

/**
 * @brief
 * @param
 * @return
 * @note   这个函数用于使用旧版的前置器底包，新版的应用，发送前置器重启命令：AF 11 01 00  ->11 和 01 根据前置器编号变尾数，00是重启，回复01是重启成功
**/
void app_update_qzq_data(uint8_t *data, uint16_t size)
{
//    if(*(uint16_t *)data == ntoh16(0xAFCB)) return ;
    env.com_state = false;

    if((data == RT_NULL) || (size == 0) || \
            env.dev_state == DEV_INIT_FAILED)
    {
        return ;
    }
    if(*(uint16_t *)data == ntoh16(0xAF01))
    {
        size -= 2;
        env.com_state = true;
        env.cur_updata_qzq = 1;
        app_can1_send(QZQ1_Q_BOARD, &data[2], size);
        app_can2_send(QZQ1_Q_BOARD, &data[2], size);
        return ;
    }
    else if(*(uint16_t *)data == ntoh16(0xAF02))
    {
        size -= 2;
        env.com_state = true;
        env.cur_updata_qzq = 2;
        app_can1_send(QZQ2_Q_BOARD, &data[2], size);
        app_can2_send(QZQ2_Q_BOARD, &data[2], size);
        return ;
    }
    else if(*(uint16_t *)data == ntoh16(0xAF03))
    {
        size -= 2;
        env.com_state = true;
        env.cur_updata_qzq = 3;
        app_can1_send(QZQ3_Q_BOARD, &data[2], size);
        app_can2_send(QZQ3_Q_BOARD, &data[2], size);
        return ;
    }
    else if(*(uint16_t *)data == ntoh16(0xAF04))
    {
        size -= 2;
        env.com_state = true;
        env.cur_updata_qzq = 4;
        app_can1_send(QZQ4_Q_BOARD, &data[2], size);
        app_can2_send(QZQ4_Q_BOARD, &data[2], size);
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
void app_update_qzq_data_deal(uint8_t *data, uint16_t size)
{
    if((data == RT_NULL) || (size == 0)) {
        return ;
    }
    if((*(uint16_t *)data == ntoh16(0xAF11)) || \
            (*(uint16_t *)data == ntoh16(0xAF12)) || \
            (*(uint16_t *)data == ntoh16(0xAF13)) || \
            (*(uint16_t *)data == ntoh16(0xAF14)) || \
            (*(uint16_t *)data == ntoh16(0x55AA) && data[2] != 0x7c))
    {
        app_tcps_send_data(data, size);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_qzq_can1_recv(struct rt_can_msg rxmsg)
{
    can_id_u can_id;
    can_id.word = rxmsg.id;
    if(can_id.bits.target == 0x20)
    {
        can_deal_t* pro_p;
        int i = 0;

        for (i = 0; i < QZQ_TOTAL; i++)
        {
            if (local.can1_qzq_deal.qzq_id[i] == can_id.bits.source)
            {
                pro_p = &local.can1_qzq_deal.proximitor[i];
                break;
            }
        }
        if (QZQ_TOTAL == i)//没有查找到对应的前置器id
        {
            for (i = 0; i < QZQ_TOTAL; i++)
            {
                if (0 == local.can1_qzq_deal.qzq_id[i])
                {
                    local.can1_qzq_deal.qzq_id[i] = can_id.bits.source;
                    pro_p = &local.can1_qzq_deal.proximitor[i];
                    break;
                }
            }
        }
        switch(can_id.bits.type)
        {
        case QZQ_SINGLE_FRAME:
            pro_p->count = 0;
            memset(&pro_p->buffer[pro_p->count], 0, QZQ_CAN1_RECV_BUFF_SIZE);
//            pro_p->count = rxmsg.len;
            rt_memmove(&pro_p->buffer[0], rxmsg.data, rxmsg.len);
            pro_p->count = rxmsg.len;
            if(*(uint16_t *)&pro_p->buffer[0] == ntoh16(0xAFCB)){
                app_tcps_send_data(pro_p->buffer, pro_p->count);
            }
            break;
        case QZQ_BEGIN_FRAME:
            pro_p->count = 0;
            memset(&pro_p->buffer[pro_p->count], 0, QZQ_CAN1_RECV_BUFF_SIZE);
            rt_memcpy(&pro_p->buffer[pro_p->count], rxmsg.data, rxmsg.len);
            pro_p->count = rxmsg.len;
            break;
        case QZQ_MIDDLE_FRAME:
            if((pro_p->count + rxmsg.len) <= QZQ_CAN1_RECV_BUFF_SIZE)
            {
                rt_memcpy(&pro_p->buffer[pro_p->count], rxmsg.data, rxmsg.len);
                pro_p->count += rxmsg.len;
            }
            break;
        case QZQ_FINISH_FRAME:
            if((pro_p->count + rxmsg.len) <= QZQ_CAN1_RECV_BUFF_SIZE)
            {
                rt_memcpy(&pro_p->buffer[pro_p->count], rxmsg.data, rxmsg.len);
                local.can1_qzq_deal.qzq_id[i] = 0;
                pro_p->count += rxmsg.len;
                if(*(uint16_t *)pro_p->buffer == ntoh16(0xAFCB)) {
                    app_tcps_send_data(pro_p->buffer, pro_p->count);
                }
//                if(*(uint16_t *)pro_p->buffer == ntoh16(0xF304)) {
//                    app_qzq_data_deal(pro_p->buffer, pro_p->count);
//                }
                pro_p->count = 0;
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
void app_qzq_can2_recv(struct rt_can_msg rxmsg)
{
    can_id_u can_id;
    can_id.word = rxmsg.id;
    if(can_id.bits.target == 0x20)
    {
        can_deal_t* pro_p;
        int i = 0;

        for (i = 0; i < QZQ_TOTAL; i++)
        {
            if (local.can2_qzq_deal.qzq_id[i] == can_id.bits.source)
            {
                pro_p = &local.can2_qzq_deal.proximitor[i];
                break;
            }
        }
        if (QZQ_TOTAL == i)//没有查找到对应的前置器id
        {
            for (i = 0; i < QZQ_TOTAL; i++)
            {
                if (0 == local.can2_qzq_deal.qzq_id[i])
                {
                    local.can2_qzq_deal.qzq_id[i] = can_id.bits.source;
                    pro_p = &local.can2_qzq_deal.proximitor[i];
                    break;
                }
            }
        }
        switch(can_id.bits.type)
        {
        case QZQ_SINGLE_FRAME:
            pro_p->count = 0;
            memset(&pro_p->buffer[pro_p->count], 0, QZQ_CAN2_RECV_BUFF_SIZE);
//            pro_p->count = rxmsg.len;
            rt_memmove(&pro_p->buffer[0], rxmsg.data, rxmsg.len);
            pro_p->count = rxmsg.len;
            if(*(uint16_t *)&pro_p->buffer[0] == ntoh16(0xAFCB)){
                app_tcps_send_data(pro_p->buffer, pro_p->count);
            }
            break;
        case QZQ_BEGIN_FRAME:
            pro_p->count = 0;
            memset(&pro_p->buffer[pro_p->count], 0, QZQ_CAN2_RECV_BUFF_SIZE);
            rt_memcpy(&pro_p->buffer[pro_p->count], rxmsg.data, rxmsg.len);
            pro_p->count = rxmsg.len;
            break;
        case QZQ_MIDDLE_FRAME:
            if((pro_p->count + rxmsg.len) <= QZQ_CAN2_RECV_BUFF_SIZE)
            {
                rt_memcpy(&pro_p->buffer[pro_p->count], rxmsg.data, rxmsg.len);
                pro_p->count += rxmsg.len;
            }
            break;
        case QZQ_FINISH_FRAME:
            if((pro_p->count + rxmsg.len) <= QZQ_CAN2_RECV_BUFF_SIZE)
            {
                rt_memcpy(&pro_p->buffer[pro_p->count], rxmsg.data, rxmsg.len);
                local.can2_qzq_deal.qzq_id[i] = 0;
                pro_p->count += rxmsg.len;
                if(*(uint16_t *)pro_p->buffer == ntoh16(0xAFCB)) {
                    app_tcps_send_data(pro_p->buffer, pro_p->count);
                }
//                if(*(uint16_t *)pro_p->buffer == ntoh16(0xF304)) {
//                    app_qzq_data_deal(pro_p->buffer, pro_p->count);
//                }
                pro_p->count = 0;
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
void app_qzq_can_init(void)
{
    env.recv_sem = rt_sem_create("can_qzq_sem", 0, RT_IPC_FLAG_PRIO);
    if(env.recv_sem == RT_NULL)
    {
        env.dev_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "can qzq device set recv sem failed");
        return ;
    }
    local.can1_recv_buff = rt_malloc(QZQ_CAN1_RECV_BUFF_SIZE);
    if(local.can1_recv_buff == NULL)
    {
        env.dev_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "app qzq can1 malloc recv buff error");
        return ;
    }
    local.can2_recv_buff = rt_malloc(QZQ_CAN2_RECV_BUFF_SIZE);
    if(local.can2_recv_buff == NULL)
    {
        env.dev_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "app qzq can2 malloc recv buff error");
        return ;
    }
    env.com_state = false;
    env.cur_updata_qzq = 0;
    env.dev_state = DEV_INIT_SUCCESS;
    for(int i=0; i<QZQ_TOTAL; i++)
    {
        local.can1_qzq_deal.qzq_id[i] = 0;
        local.can1_qzq_deal.proximitor[i].count = 0;
        local.can1_qzq_deal.proximitor[i].buffer = rt_malloc(256);//仅用于前置器id非1-4时接收前置器ID用
        if (local.can1_qzq_deal.proximitor[i].buffer == RT_NULL)
        {
            rt_kprintf("proximitor.buffer malloc failed\n");
        }
        local.can2_qzq_deal.qzq_id[i] = 0;
        local.can2_qzq_deal.proximitor[i].count = 0;
        local.can2_qzq_deal.proximitor[i].buffer = rt_malloc(256);//仅用于前置器id非1-4时接收前置器ID用
        if (local.can2_qzq_deal.proximitor[i].buffer == RT_NULL)
        {
            rt_kprintf("proximitor.buffer malloc failed\n");
        }
    }
    app_log_msg(LOG_LVL_INFO, true, "app can qzq init success");
}










