/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __PARAS_INFO_PROTOCOL_H__
#define __PARAS_INFO_PROTOCOL_H__

/* include */
#include "app_board.h"

/* macro */

/* type declaration */
typedef struct
{
    uint8_t sn[2];
    uint8_t id;
    uint8_t state;
}qzq_paras_msg_t;
typedef struct
{
    uint8_t head[2];
    uint8_t size[2];   //除head、size、check外的字节数
    uint8_t check;
    uint8_t data[sizeof(qzq_paras_msg_t)];
}qzq_paras_info_t;
typedef struct
{
    uint8_t train_num;
    uint8_t gand_up;
    uint8_t state;
}clb_paras_msg_t;
typedef struct
{
    uint8_t head[2];
    uint8_t size[2];   //除head、size、check外的字节数
    uint8_t check;
    uint8_t data[sizeof(clb_paras_msg_t)];
}clb_paras_info_t;

/* variable declaration */

/* function declaration */

#endif /*__PARAS_INFO_PROTOCOL_H__*/




