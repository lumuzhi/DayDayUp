/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "can2_process.h"
#include "app_log.h"
#include "app_update.h"
#include "head_protocol.h"
#include "paras_protocol.h"

/* macro */

/* type declaration */
typedef struct
{
    struct rt_can_msg rxmsg;
}can2_process_local_t;

/* variable declaration */
can2_process_env_t can2_process_env;
#define env can2_process_env
can2_process_local_t can2_process_local;
#define local can2_process_local

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void can2_process_data(uint8_t *data, uint16_t size)
{
    if(data == NULL) {
        return ;
    }
//    if(size == sizeof(paras_info_t))
//    {
//        ;
//    }
//    else
//    {
//        rt_kprintf("can2 read data:\n");
//        for(rt_uint16_t i = 0; i < size; i++)
//        {
//            rt_kprintf("0x%02X ", data[i]);
//        }
//        rt_kprintf("\n");
//    }
}















