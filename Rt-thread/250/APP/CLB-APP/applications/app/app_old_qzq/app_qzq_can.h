/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_QZQ_CAN_H__
#define __APP_QZQ_CAN_H__

/* include */
#include "app_board.h"
#include "app_diag.h"

/* macro */
#define CAN_QZQ_MAX_DLEN     8
#define CAN_QZQ_PRO_LENS     12

/* type declaration */
typedef enum
{
    QZQ_SINGLE_FRAME, //单帧
    QZQ_BEGIN_FRAME,  //多帧开始
    QZQ_MIDDLE_FRAME, //多帧中间
    QZQ_FINISH_FRAME, //多帧结束
}frame_qzq_type_e;
enum frame_type
{
    SINGLE,
    MULTI_BEGIN,
    MULTI_MID,
    MULTI_END
};
typedef union
{
    struct
    {
        rt_uint32_t souc_id  :6;
        rt_uint32_t type     :2;
        rt_uint32_t tag_id   :6;
        rt_uint32_t ack      :2;
        rt_uint32_t cnt      :6;
        rt_uint32_t reserve  :4;
        rt_uint32_t priority :3;
        rt_uint32_t nc       :3;
    } bits;
    rt_uint32_t word;
}can_qzq_id_u;
typedef struct
{
    uint8_t head[2];     //55AA
    uint8_t lens[2];     //CAN_QZQ_PRO_LENS + data lens
    uint8_t cmd;         //命令
    uint8_t taget;
    uint8_t souce;
    uint8_t reserved[4]; //预留
    uint8_t data[];      //包含1位校验位（和校验）
}qzq_send_t;
typedef enum
{
    RADIO_Q_BOARD = 0x00,
    QZQ1_Q_BOARD  = 0x01,
    QZQ2_Q_BOARD  = 0x02,
    QZQ3_Q_BOARD  = 0x03,
    QZQ4_Q_BOARD  = 0x04,
    CLB_Q_BOARD   = 0x20
}can_qzq_id_e;
typedef struct
{
    rt_sem_t    recv_sem;
    dev_state_e dev_state;
    bool        com_state;    //通信状态 true=升级前置器
    uint8_t     cur_updata_qzq;
}app_qzq_can_env_t;

typedef struct can_deal
{
     uint16_t count;
     uint8_t *buffer;

}can_deal_t, * can_deal_p;

typedef struct
{
    uint8_t qzq_id[QZQ_TOTAL];
    can_deal_t proximitor[QZQ_TOTAL];
}qzqid_deal_t,* qzqid_deal_p;//用于大于4的前置器ID修改ID
enum message_type
{
    SYSTEM_RESET_CMD = 0x00,        //复位重启
    UPDATE_PRE_BOARD_CMD = 0x01,    //升级前置处理器
    SYS_DATA_CMD = 0x05,            //系统数据
    GET_ID_CMD = 0x06,
    SET_ID_CMD = 0X7 ,              //设置前置器地址（预留）
};
/* variable */
app_qzq_can_env_t app_qzq_can_env;

/* function */
void app_qzq_can_init(void);
void app_update_qzq_data(uint8_t *data, uint16_t size);
void app_update_qzq_data_deal(uint8_t *data, uint16_t size);
void app_qzq_can1_recv(struct rt_can_msg rxmsg);
void app_qzq_can2_recv(struct rt_can_msg rxmsg);
void app_qzq_can1_recv_test(struct rt_can_msg rxmsg);

#endif /*__APP_QZQ_CAN_H__*/



