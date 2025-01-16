/**
 * @file
 * @details
 * @author
 * @date    系统参数保存
 * @version
**/

/* include */
#include "app_paras.h"
#include "easyflash.h"
#include "app_log.h"
#include "app_scast.h"
#include "app_mcast.h"
#include "app_can.h"
#include "app_can1.h"
#include "app_can2.h"
#include "paras_protocol.h"
#include "head_protocol.h"
#include "app_utility.h"
#include "bsp_flash.h"


/* macro */

#define APP_PARAS_CHUNK_HEAD               0x6ACA
#define APP_PARAS_CHUNK_CHECKSUM_CODE      0xC5

/* type declaration */
typedef struct
{
    uint16_t recv_size;
}app_paras_local_t;

/* variable declaration */
app_paras_t app_paras;
app_paras_env_t app_paras_env;
#define env app_paras_env
app_paras_local_t app_paras_local;
#define local app_paras_local
sys_paras_t def_sys_paras =
{
    .group_num      = 6,
    .train_num      = 1,
    .device         = DEV_SLAVE,
    .local_ip       = {192, 168, 1, 13},
    .netmask        = {255, 255, 0, 0},
    .gateway        = {192, 168, 1, 1},
    .mcast_ip       = {239, 255, 10, 1},
    .mcast_port     = {0x23, 0x28},  //9000
    .mcast_raw_ip   = {239, 255, 10, 2},
    .mcast_raw_port = {0x23, 0x29},  //9001
};

/* function declaration */

/**
 * @brief
 * @param
 * @return
 * @note
**/
static uint8_t app_paras_save(void)
{
    rt_base_t level;
    uint8_t result = 0;

    env.paras_inform.head[0] = DATA16_H(APP_PARAS_CHUNK_HEAD);
    env.paras_inform.head[1] = DATA16_L(APP_PARAS_CHUNK_HEAD);
    env.paras_inform.train_num = app_paras.paras.train_num;
    env.paras_inform.group_num = app_paras.paras.group_num;
    env.paras_inform.check_sum = Check_xor_sum(&env.paras_inform, STRUCT_OFFSET(app_paras_save_t, check_sum),
                                         APP_PARAS_CHUNK_CHECKSUM_CODE);
    level = rt_hw_interrupt_disable();
    result = bsp_flash_write((uint8_t *)&env.paras_inform, sizeof(app_paras_save_t));
    rt_hw_interrupt_enable(level);
    rt_kprintf("write parameters result %d (0 is OK)!\n", result);
    return result;
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static uint8_t app_paras_read(void)
{
    rt_base_t level;
    uint8_t result = 0;

    level = rt_hw_interrupt_disable();
    result = bsp_flash_read((uint8_t *)&env.paras_inform, sizeof(app_paras_save_t));
    rt_hw_interrupt_enable(level);

    if (APP_PARAS_CHUNK_HEAD == DATA16(env.paras_inform.head[0], env.paras_inform.head[1])) {
        uint8_t check_sum = Check_xor_sum(&env.paras_inform, STRUCT_OFFSET(app_paras_save_t, check_sum),
                                             APP_PARAS_CHUNK_CHECKSUM_CODE);
        result = (check_sum == env.paras_inform.check_sum) ? 0 : 1;
    }
    else {
        result = 1;
    }
    rt_kprintf("read parameters result %d (0 is OK)!\n", result);
    return result;
}


/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_paras_qzq_config(uint8_t *data, uint16_t size)
{
    if((data == NULL) || (size < 15) || (strncmp((const char *)data, "lhkj qzq config", 15) != 0))
    {
        return ;
    }
    int sn = 0, id = 0;
    if(sscanf((char *)data, "lhkj qzq config sn %d, id %d", &sn, &id) == 2)
    {
        if((id > 0) && (id < 30))
        {
            uint8_t paras_config[sizeof(qzq_paras_info_t)] = { 0 };
            qzq_paras_info_t *paras_info = (qzq_paras_info_t *)paras_config;
            qzq_paras_msg_t *paras_msg = (qzq_paras_msg_t *)paras_info->data;
            *(uint16_t *)paras_info->head = hton16(HEAD_CAN_CLB_QZQ_PARAS);
            paras_info->size[0] = DATA16_H(sizeof(qzq_paras_info_t) - 5);
            paras_info->size[1] = DATA16_L(sizeof(qzq_paras_info_t) - 5);
            paras_msg->sn[0] = DATA16_H((uint16_t)sn);
            paras_msg->sn[1] = DATA16_L((uint16_t)sn);
            paras_msg->id = id;
            paras_msg->state = 0;
            paras_info->check = crc8_maxim(paras_info->data, hton16(*(uint16_t *)paras_info->size));
            app_can1_send(QZQ1_BOARD, paras_config, sizeof(qzq_paras_info_t));
            app_can1_send(QZQ4_BOARD, paras_config, sizeof(qzq_paras_info_t));
            app_can1_send(QZQ2_BOARD, paras_config, sizeof(qzq_paras_info_t));
            app_can1_send(QZQ3_BOARD, paras_config, sizeof(qzq_paras_info_t));
            app_can2_send(QZQ1_BOARD, paras_config, sizeof(qzq_paras_info_t));
            app_can2_send(QZQ4_BOARD, paras_config, sizeof(qzq_paras_info_t));
            app_can2_send(QZQ2_BOARD, paras_config, sizeof(qzq_paras_info_t));
            app_can2_send(QZQ3_BOARD, paras_config, sizeof(qzq_paras_info_t));
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note   lhkj qzq config sn 65536, id 1
**/
void app_paras_qzq_deal(uint8_t *data, uint16_t size)
{
    sys_paras_t *paras = app_paras_get();

    if((data == NULL) || (size != sizeof(qzq_paras_info_t)) || \
            (*(uint16_t *)data != hton16(HEAD_CAN_CLB_QZQ_PARAS)))
    {
        return ;
    }
    qzq_paras_info_t *paras_info = (qzq_paras_info_t *)data;
    qzq_paras_msg_t *paras_msg = (qzq_paras_msg_t *)paras_info->data;
    if(paras_info->check == crc8_maxim(paras_info->data, hton16(*(uint16_t *)paras_info->size)))
    {
        char data_info[100];
        memset(data_info, 0, sizeof(data_info));
        sprintf(data_info, "LHKJ QZQ CONFIG sn %d, id %d, cur_train %d", DATA16(paras_msg->sn[0], paras_msg->sn[1]), paras_msg->id, paras->train_num);
        app_mcast_send_data((uint8_t *)data_info, strlen(data_info));
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_paras_printf(void)
{
#define paras app_paras.paras

    char log_data[250];

    memset(log_data, 0, sizeof(log_data));
    sprintf(log_data, "app sys paras group num %d", paras.group_num);
    app_log_msg(LOG_LVL_INFO, true, log_data);
    memset(log_data, 0, sizeof(log_data));
    sprintf(log_data, "app sys paras train num %d", paras.train_num);
    app_log_msg(LOG_LVL_INFO, true, log_data);
    memset(log_data, 0, sizeof(log_data));
    sprintf(log_data, "app sys paras local_ip %d.%d.%d.%d", paras.local_ip[0],
            paras.local_ip[1], paras.local_ip[2], paras.local_ip[3]);
    app_log_msg(LOG_LVL_INFO, true, log_data);
    memset(log_data, 0, sizeof(log_data));
    sprintf(log_data, "app sys paras netmask %d.%d.%d.%d", paras.netmask[0],
            paras.netmask[1], paras.netmask[2], paras.netmask[3]);
    app_log_msg(LOG_LVL_INFO, true, log_data);
    memset(log_data, 0, sizeof(log_data));
    sprintf(log_data, "app sys paras gateway %d.%d.%d.%d", paras.gateway[0],
            paras.gateway[1], paras.gateway[2], paras.gateway[3]);
    app_log_msg(LOG_LVL_INFO, true, log_data);
    memset(log_data, 0, sizeof(log_data));
    sprintf(log_data, "app sys paras mcast ip %d.%d.%d.%d", paras.mcast_ip[0],
            paras.mcast_ip[1], paras.mcast_ip[2], paras.mcast_ip[3]);
    app_log_msg(LOG_LVL_INFO, true, log_data);
    memset(log_data, 0, sizeof(log_data));
    sprintf(log_data, "app sys paras mcast port %d", DATA16(paras.mcast_port[0], paras.mcast_port[1]));
    app_log_msg(LOG_LVL_INFO, true, log_data);
    memset(log_data, 0, sizeof(log_data));
    sprintf(log_data, "app sys paras tcp server port %d", APP_TCPS_PORT);
    app_log_msg(LOG_LVL_INFO, true, log_data);
    memset(log_data, 0, sizeof(log_data));
    sprintf(log_data, "app sys paras udp port %d", APP_UDP_PORT);
    app_log_msg(LOG_LVL_INFO, true, log_data);

#undef paras
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
//void app_paras_save(void)
//{
//    char paras_msg[512];
//    int size = sprintf(paras_msg, "group_num=%d&train_num=%d", app_paras.paras.group_num, app_paras.paras.train_num);
//    paras_msg[size] = '\0';
//    ef_set_env("paras_info", paras_msg);
//}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_paras_config(uint8_t *data, uint16_t size)
{
    char data_info[100];
    uint8_t change_sign = 0;

    if((data == NULL) || (size != 42) || (strncmp((const char *)data, "lhkj paras config", 17) != 0))
    {
        return ;
    }
    int group_num = 0, train_num = 0;
    if(sscanf((char *)data, "lhkj paras config group_num %d, train_num %d", &group_num, &train_num) == 2)
    {
        if(((group_num == 4) && (train_num > 0) && (train_num <= group_num)) || \
                ((group_num == 6) && (train_num > 0) && (train_num <= group_num)))
        {
            if(app_paras.paras.group_num != group_num)
            {
                change_sign = 1;
                app_paras.paras.group_num = group_num;
                memset(data_info, 0, sizeof(data_info));
                sprintf(data_info, "recv new group_num :%d", app_paras.paras.group_num);
                app_log_msg(LOG_LVL_INFO, true, data_info);
            }
            if(app_paras.paras.train_num != train_num)
            {
                change_sign = 1;
                app_paras.paras.train_num = train_num;
                memset(data_info, 0, sizeof(data_info));
                sprintf(data_info, "recv new train num :%d", app_paras.paras.train_num);
                app_log_msg(LOG_LVL_INFO, true, data_info);
            }
            memset(data_info, 0, sizeof(data_info));
            sprintf(data_info, "LHKJ PARAS CONFIG clb group_num %d, train_num %d", app_paras.paras.group_num, app_paras.paras.train_num);
            app_mcast_send_data((uint8_t *)data_info, strlen(data_info));
        }
    }
    if(change_sign)
    {
        app_paras_save();
        app_log_msg(LOG_LVL_INFO, true, "sys paras update and reboot");
        rt_thread_delay(2000);
        rt_hw_cpu_reset();
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_paras_init(void)
{
    /*  use easyflash to save paras  */
//    env.paras_state = PARAS_INIT_NULL;
//    char paras_msg[512];
//    char *paras_info = ef_get_env("paras_info");
//    int group_num = 0, train_num = 0;
//    rt_kprintf("read paras info: %s\n", paras_info);
//    if(sscanf(paras_info, "group_num=%d&train_num=%d", &group_num, &train_num) == 2)
//    {
//        memcpy(&app_paras.paras, &def_sys_paras, sizeof(sys_paras_t));
//        app_paras.paras.group_num = group_num;
//        app_paras.paras.train_num = train_num;
//        (train_num == group_num) ? (app_paras.paras.device = DEV_MASTER) : (app_paras.paras.device = DEV_SLAVE);
//        app_paras.paras.local_ip[2] = train_num;
//        app_paras.paras.gateway[2] = train_num;
//        env.paras_state = PARAS_INIT_SUCCESS;
//        app_log_msg(LOG_LVL_INFO, true, "sys paras read success");
//    }
//    else
//    {
//        memcpy(&app_paras.paras, &def_sys_paras, sizeof(sys_paras_t));
//        int size = sprintf(paras_msg, "group_num=%d&train_num=%d", def_sys_paras.group_num, def_sys_paras.train_num);
//        paras_msg[size] = '\0';
//        ef_set_env("paras_info", paras_msg);
//        env.paras_state = PARAS_INIT_DEFAULT;
//        app_log_msg(LOG_LVL_ERROR, true, "sys paras read failed");
//    }
//    local.recv_size = 0;
//    app_paras_printf();

    /*  使用板载flash存储参数  */
    env.paras_state = PARAS_INIT_NULL;
    if(!app_paras_read()) {
        memcpy(&app_paras.paras, &def_sys_paras, sizeof(sys_paras_t));
        app_paras.paras.group_num = env.paras_inform.group_num;
        app_paras.paras.train_num = env.paras_inform.train_num;
        (app_paras.paras.train_num == app_paras.paras.group_num) ? (app_paras.paras.device = DEV_MASTER) : (app_paras.paras.device = DEV_SLAVE);
        app_paras.paras.local_ip[2] = env.paras_inform.train_num;
        app_paras.paras.gateway[2] = env.paras_inform.train_num;
        env.paras_state = PARAS_INIT_SUCCESS;
        app_log_msg(LOG_LVL_INFO, true, "sys paras read success");
    }
    else {
        memcpy(&app_paras.paras, &def_sys_paras, sizeof(sys_paras_t));
        app_paras_save();
        env.paras_state = PARAS_INIT_DEFAULT;
        app_log_msg(LOG_LVL_ERROR, true, "sys paras read failed");
    }
    local.recv_size = 0;
    app_paras_printf();
}

