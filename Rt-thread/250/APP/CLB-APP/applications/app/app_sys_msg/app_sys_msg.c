/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_sys_msg.h"
#include "app_paras.h"
#include "app_scast.h"
#include "app_mcast.h"
#include "app_qzq.h"

/* macro */

/* type declaration */

/* variable declaration */

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void sys_msg_qzq_info(uint8_t *data, uint16_t size)
{
    if((data == NULL) || (size != 15) || (strncmp((const char *)data, "lhkj qzq info ?", 15) != 0))
    {
        return ;
    }
    rt_kprintf("sys_msg_qzq_info !\n");
    char qzq_info[512];
    memset(qzq_info, 0, sizeof(qzq_info));
    sprintf(qzq_info, "LHKJ QZQ INFO qzq1 %d, qzq2 %d, qzq3 %d, qzq4 %d", app_qzq_env.qzq_data[0].com_state, \
            app_qzq_env.qzq_data[1].com_state, app_qzq_env.qzq_data[2].com_state, app_qzq_env.qzq_data[3].com_state);
    app_mcast_send_data((uint8_t *)qzq_info, strlen(qzq_info));
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void sys_msg_local_info(uint8_t *data, uint16_t size)
{
    sys_paras_t *paras = app_paras_get();

    if((data == NULL) || (size != 18) || (strncmp((const char *)data, "lhkj device info ?", 18) != 0))
    {
        return ;
    }
    char local_info[512];
    memset(local_info, 0, sizeof(local_info));
    sprintf(local_info, "LHKJ DEVICE INFO clb group_num %d, train_num %d, local_ip: %d.%d.%d.%d", paras->group_num, \
            paras->train_num, paras->local_ip[0], paras->local_ip[1], paras->local_ip[2], paras->local_ip[3]);
    app_mcast_send_data((uint8_t *)local_info, strlen(local_info));
}
