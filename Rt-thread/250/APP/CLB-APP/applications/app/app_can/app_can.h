/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_CAN_H__
#define __APP_CAN_H__

/* include */
#include "app_board.h"

/* macro */
#define CAN_MAX_DLEN     8

/* type declaration */
typedef enum
{
    SINGLE_FRAME, //单帧
    BEGIN_FRAME,  //多帧开始
    MIDDLE_FRAME, //多帧中间
    FINISH_FRAME, //多帧结束
}frame_type_e;
typedef union
{
    struct
    {
        rt_uint32_t source   :8;
        rt_uint32_t target   :8;
        rt_uint32_t type     :2;
        rt_uint32_t count    :8;
        rt_uint32_t nc       :6;
    } bits;
    rt_uint32_t word;
}can_id_u;
typedef enum
{
    RADIO_BOARD = 0x00,
    QZQ1_BOARD  = 0x01,
    QZQ2_BOARD  = 0x02,
    QZQ3_BOARD  = 0x03,
    QZQ4_BOARD  = 0x04,
    CLB_BOARD   = 0x20
}can_id_e;
typedef struct
{
    uint16_t can_size;   //LED灯使用
}app_can_env_t;

/* variable */
extern app_can_env_t app_can_env;

/* function */
void app_can_init(void);
void app_can_thread(void);
void app_can_notify_qzq_gather_temp(uint8_t flag);

#endif /*__APP_CAN_H__*/



