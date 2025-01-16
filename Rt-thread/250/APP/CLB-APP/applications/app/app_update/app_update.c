/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_update.h"
#include "app_can.h"
#include "app_qzq_can.h"
#include "app_can1.h"
#include "app_can2.h"
#include "app_log.h"
#include "bsp_hw_rtc.h"
#include "update_protocol.h"
#include "head_protocol.h"
#include "easyflash.h"
#include "app_scast.h"
#include "app_new_qzq.h"
#include "app_hw_timer.h"
/* macro */
#define APP_UPDATE_RTC_BKP_DR      RTC_BKP_DR2   //RTC_BKP_DR1在rtthread的rtc中已经使用
#define APP_UPDATE_BUFF_MAX        1536

/* type declaration */
typedef struct
{
    uint32_t head;          //头信息
    uint32_t update_flag;   //0-不升级  1-升级
    uint32_t update_size;   //升级包大小
    uint32_t update_addr;   //升级地址
    uint32_t check;
}update_data_infor_t;
typedef struct
{
    update_data_infor_t update_data_infor;
    uint8_t             update_buff[APP_UPDATE_BUFF_MAX];
    uint32_t            update_size;
    size_t              cur_size;
    uint32_t            pkg_num;
}app_update_local_t;

/* variable declaration */
app_update_local_t app_update_local;
#define local app_update_local
app_update_env_t app_update_env;
#define env app_update_env

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note   升级准备 AF CB 00 01 00 02 00 04 81
**/
static void app_update_ready(uint8_t *data, uint16_t size)
{
    char update_info[256];
    pupdata_protocol_t updata = (pupdata_protocol_t)data;
    pupdata_protocol_t reply_updata = (pupdata_protocol_t)local.update_buff;

    *(uint16_t *)reply_updata->head = *(uint16_t *)updata->head;
    reply_updata->command = READY_CMD;
    reply_updata->operate = REPLY_OPE;
    *(uint16_t *)reply_updata->lens = ntoh16(1);

    if((updata->operate == WRITE_OPE) && (ntoh16(*(uint16_t *)updata->lens) == 0x04) && (ntoh32(*(uint32_t *)updata->data) > 0x00))
    {
//        app_hw_timer4_stop();
        env.update_state = UPDATE_READY;
        local.update_size = ntoh32(*(uint32_t *)updata->data);
        int update_size = (int)local.update_size;
        int counts = sprintf(update_info, "app update size = %d", update_size);
        update_info[counts] = '\0';
        app_log_msg(LOG_LVL_INFO, true, update_info);

        reply_updata->data[0] = APP_UPDATE_OPE_SUCCESS;
        reply_updata->data[1] = CheckSum(local.update_buff, sizeof(updata_protocol_t));
        app_tcps_send_data(local.update_buff, sizeof(updata_protocol_t) + 2);
        env.update_state = UPDATE_START;
    }
    else
    {
        reply_updata->data[0] = APP_UPDATE_OPE_FAILED;
        reply_updata->data[1] = CheckSum(local.update_buff, sizeof(updata_protocol_t));
        app_tcps_send_data(local.update_buff, sizeof(updata_protocol_t) + 2);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note   固件传输 AF CB 01 01 00 0A 11 22 33 44 55 66 77 88 99 00 83
**/
static void app_update_trans(uint8_t *data, uint16_t size)
{
    static EfErrCode result;
    pupdata_protocol_t updata = (pupdata_protocol_t)data;
    pupdata_protocol_t reply_updata = (pupdata_protocol_t)local.update_buff;

    *(uint16_t *)reply_updata->head = *(uint16_t *)updata->head;
    reply_updata->command = TRANS_CMD;
    reply_updata->operate = REPLY_OPE;
    *(uint16_t *)reply_updata->lens = ntoh16(3);

    if(env.update_state == UPDATE_START)
    {
        result = ef_erase_bak_app(local.update_size);
        local.cur_size = 0;
        local.pkg_num = 1;
        env.update_state = UPDATE_DOING;
    }
    if((updata->operate == WRITE_OPE) && (env.update_state == UPDATE_DOING) && (ntoh16(*(uint16_t *)updata->data) == local.pkg_num) && \
            (result == EF_NO_ERR))
    {
        uint16_t fw_size = ntoh16(*(uint16_t *)updata->lens) - 2; //数据包总长度-包序号长度
        rt_kprintf("pkg_id = %d---recv data =%d\n", ntoh16(*(uint16_t *)updata->data), fw_size);
        ef_write_data_to_bak(&updata->data[2], fw_size, &local.cur_size, local.update_size);
        *(uint16_t *)reply_updata->data = ntoh16(local.pkg_num);
        reply_updata->data[2] = APP_UPDATE_OPE_SUCCESS;
        reply_updata->data[3] = CheckSum(local.update_buff, sizeof(updata_protocol_t) + 3);
        app_tcps_send_data(local.update_buff, sizeof(updata_protocol_t) + 4);
        local.pkg_num++;
    }
    else
    {
        *(uint16_t *)reply_updata->data = ntoh16(local.pkg_num);
        reply_updata->data[2] = APP_UPDATE_OPE_FAILED;
        reply_updata->data[3] = CheckSum(local.update_buff, sizeof(updata_protocol_t) + 3);
        app_tcps_send_data(local.update_buff, sizeof(updata_protocol_t) + 4);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note   固件传输完成
**/
static void app_update_finish(uint8_t *data, uint16_t size)
{
    pupdata_protocol_t updata = (pupdata_protocol_t)data;
    pupdata_protocol_t reply_updata = (pupdata_protocol_t)local.update_buff;

    *(uint16_t *)reply_updata->head = *(uint16_t *)updata->head;
    reply_updata->command = FINISH_CMD;
    reply_updata->operate = REPLY_OPE;
    *(uint16_t *)reply_updata->lens = ntoh16(1);

    if((updata->operate == WRITE_OPE) && (ntoh16(*(uint16_t *)updata->lens) == 0x00))
    {
        local.update_data_infor.head = APP_UPDATE_HEAD;
        local.update_data_infor.update_flag = 1;
        local.update_data_infor.update_size = local.update_size;
        local.update_data_infor.update_addr = 0x08020000;
        local.update_data_infor.check = CheckTotal(&local.update_data_infor.head, 4);
        bsp_hwrtc_bkup_write(APP_UPDATE_RTC_BKP_DR, (uint32_t *)&local.update_data_infor, sizeof(update_data_infor_t) / sizeof(uint32_t));
        local.update_size = 0;
        local.cur_size = 0;
        rt_kprintf("app update flag = %d, size = 0x%08X, addr = 0x%08X !\n", local.update_data_infor.update_flag, \
                local.update_data_infor.update_size, local.update_data_infor.update_addr);
        app_log_msg(LOG_LVL_INFO, true, "app update success reset mcu");
        reply_updata->data[0] = APP_UPDATE_OPE_SUCCESS;
        reply_updata->data[1] = CheckSum(local.update_buff, sizeof(updata_protocol_t) + 1);
        app_tcps_send_data(local.update_buff, sizeof(updata_protocol_t) + 2);
        rt_thread_delay(2000);
        rt_hw_cpu_reset();
    }
    else
    {
        reply_updata->data[0] = APP_UPDATE_OPE_FAILED;
        reply_updata->data[1] = CheckSum(local.update_buff, sizeof(updata_protocol_t));
        app_tcps_send_data(local.update_buff, sizeof(updata_protocol_t) + 2);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_update_read_data(uint8_t *data, uint16_t size)
{
    app_update_qzq_data(data, size);

    if(*(uint16_t *)data == ntoh16(0xAFFA))
    {
        uint8_t send_data[2] = {0xFA, 0xAF};  //前置器升级成功后，发送重启命令，让处理板恢复正常的通信模式
        rt_kprintf(" update success: send 0xFA, 0xAF\n");
        app_tcps_send_data(send_data, 2);
        app_qzq_can_env.com_state = false;
        rt_thread_delay(2000);
        rt_hw_cpu_reset();
    }
    uint8_t cal_crc = 0;
    if((data == RT_NULL) || (size == 0) || (size < sizeof(updata_protocol_t))) {
        return ;
    }
    if(*(uint16_t *)data == ntoh16(APP_UPDATE_HEAD))
    {
        goto update;
    }
    else{
        return ;
    }
#if 0
    else if(*(uint16_t *)data == ntoh16(HEAD_CAN_QZQ1_UPDATE_INFO))
    {
        size -= 2;
        app_can1_send(QZQ1_BOARD, &data[2], size);
        app_can2_send(QZQ1_BOARD, &data[2], size);
        return ;
    }
    else if(*(uint16_t *)data == ntoh16(HEAD_CAN_QZQ2_UPDATE_INFO))
    {
        size -= 2;
        app_can1_send(QZQ2_BOARD, &data[2], size);
        app_can2_send(QZQ2_BOARD, &data[2], size);
        return ;
    }
    else if(*(uint16_t *)data == ntoh16(HEAD_CAN_QZQ3_UPDATE_INFO))
    {
        size -= 2;
        app_can1_send(QZQ3_BOARD, &data[2], size);
        app_can2_send(QZQ3_BOARD, &data[2], size);
        return ;
    }
    else if(*(uint16_t *)data == ntoh16(HEAD_CAN_QZQ4_UPDATE_INFO))
    {
        size -= 2;
        app_can1_send(QZQ4_BOARD, &data[2], size);
        app_can2_send(QZQ4_BOARD, &data[2], size);
        return ;
    }
    else {
        return ;
    }
#endif
    update:
    cal_crc = CheckSum(data, size - 1);
    if(data[size - 1] != cal_crc)
    {
        rt_kprintf("recv crc = 0x%02X, calculate crc = 0x%02X ! \n", data[size - 1], cal_crc);
        return ;
    }
    pupdata_protocol_t updata = (pupdata_protocol_t)data;
    if((ntoh16(*(uint16_t *)updata->lens) + sizeof(updata_protocol_t) + 1/*校验位*/) != size)
    {
        rt_kprintf("update read lens error ! \n");
        return ;
    }
    switch (updata->command)
    {
        case READY_CMD:  //升级准备
            app_update_ready(data, size);
            break;
        case TRANS_CMD:  //固件传输
            app_update_trans(data, size);
            break;
        case FINISH_CMD: //固件传输完成
            app_update_finish(data, size);
            break;
        default:
            break;
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_update_data_deal(uint8_t *data, uint16_t size)
{
    if((data == RT_NULL) || (size == 0)) {
        return ;
    }
    if((*(uint16_t *)data == ntoh16(HEAD_CAN_QZQ1_UPDATE_INFO)) || \
            (*(uint16_t *)data == ntoh16(HEAD_CAN_QZQ2_UPDATE_INFO)) || \
            (*(uint16_t *)data == ntoh16(HEAD_CAN_QZQ3_UPDATE_INFO)) || \
            (*(uint16_t *)data == ntoh16(HEAD_CAN_QZQ4_UPDATE_INFO)) || \
            (*(uint16_t *)data == ntoh16(APP_UPDATE_HEAD)))
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
void app_update_init(void)
{
    env.update_state = UPDATE_NULL;
    if((sizeof(update_data_infor_t) / sizeof(uint32_t)) <= APP_RTCBKUP_LENS_MAX)
    {
        local.update_data_infor.head = 0;
        local.update_data_infor.update_flag = 0;
        local.update_data_infor.update_size = 0;
        local.update_data_infor.update_addr = 0;
        local.update_data_infor.check = 0;
        bsp_hwrtc_bkup_write(APP_UPDATE_RTC_BKP_DR, (uint32_t *)&local.update_data_infor,
                sizeof(update_data_infor_t) / sizeof(uint32_t));
        env.update_state = UPDATE_INITOK;
        app_log_msg(LOG_LVL_INFO, true, "app update init success");
    }
    else {
        app_log_msg(LOG_LVL_ERROR, true, "app update data greater than rtcbkup max");
    }
}



