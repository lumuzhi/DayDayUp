/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __CAN2_PROCESS_H__
#define __CAN2_PROCESS_H__

/* include */
#include "app_board.h"

/* macro */

/* type declaration */
typedef struct
{
    uint16_t zkb_a_paras_cnts;
    uint16_t zkb_b_paras_cnts;
}can2_process_env_t;

/* variable */
extern can2_process_env_t can2_process_env;

/* function */
void can2_process_data(uint8_t *data, uint16_t size);

#endif /*__CAN2_PROCESS_H__*/



