/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_period.h"
#include "app_mcast.h"
#include "app_scast.h"
#include "app_log.h"
#include "app_paras.h"
#include "app_diag.h"
#include "app_qzq.h"
#include "head_protocol.h"
#include "ptu_protocol.h"

/* macro */

/* type declaration */

/* variable */
app_period_env_t app_period_env;
#define env app_period_env

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
#if 1
static void app_txb_info_update(uint16_t count)
{
    char log_data[256] = { 0 };
     sys_paras_t *paras = app_paras_get();
    static uint8_t  not_trust_cnt = 0;
    static uint8_t trust_cnt[TRAIN_MAX][TRDP_DEV_MAX] = {0};
    static uint16_t trdp_cnt[TRAIN_MAX][TRDP_DEV_MAX] = {0};
    static uint16_t reset_cnt[TRAIN_MAX][TRDP_DEV_MAX] = {0};
#define txb_info app_mcast_env.txb_info
    if(txb_info.trust_mvb->com_state)
    {
        ccu_public_t * public_p = (ccu_public_t *)txb_info.trust_mvb->data;
        uint16_t wheel = env.clb_info.wheel;
        switch(paras->train_num)
        {
            case 1:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_1[0], public_p->wheel_1[1]);
                }
                break;
            case 2:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_2[0], public_p->wheel_2[1]);
                }
                break;
            case 3:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_3[0], public_p->wheel_3[1]);
                }
                break;
            case 4:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_4[0], public_p->wheel_4[1]);
                }
                break;
            case 5:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_5[0], public_p->wheel_5[1]);
                }
                break;
            case 6:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_6[0], public_p->wheel_6[1]);
                }
                break;
            default:
                break;
        }
        if(env.clb_info.wheel != wheel)
        {
            memset(log_data, 0, sizeof(log_data));
            sprintf(log_data, "recv new wheel %d, old wheel %d", wheel, env.clb_info.wheel);
            app_log_msg(LOG_LVL_INFO, true, log_data);
            env.clb_info.wheel = wheel;
        }
        //协议中 (1 -- 0.01km/h) 已在通信板转换
        env.clb_info.speed = DATA16(public_p->speed[0], public_p->speed[1]);
        env.clb_info.speed /= 100;
        env.clb_info.now_id = DATA16(public_p->curr_id[0], public_p->curr_id[1]);
        env.clb_info.next_id = DATA16(public_p->next_id[0], public_p->next_id[1]);
    }
    else
    {
        if(env.clb_info.speed != 0)
        {
            memset(log_data, 0, sizeof(log_data));
            sprintf(log_data, "txb com state error, set speed %d -> 0", env.clb_info.speed);
            app_log_msg(LOG_LVL_INFO, true, log_data);
            //因为通信中断不知道现在车速是多少，考虑到诊断需要速度，未知速度容易误报警，其实可以将转速板的速度用起来，暂时先这样
            env.clb_info.speed = 0;
        }
    }

    if(!(count % 10)) {
        for(train_e i = TRAIN_HEAD; i < TRAIN_MAX; i++) {
            for(trdp_dev_e j = TRDP_JLB; j < TRDP_DEV_MAX; j++) {
                if(trdp_cnt[i][j] != txb_info.mvb_info[i][j].recv_count) {
                    if(trust_cnt[i][j] >= 5) {
                        if(txb_info.mvb_info[i][j].com_state != 1) {
                            txb_info.mvb_info[i][j].com_state = 1;
                            sprintf(log_data, "train %d mcast recv %d trdp data get right ps: 0 is head, 1 is tail", i, j);
                            app_log_msg(LOG_LVL_INFO, true, log_data);
                        }
                    } else{
                        trust_cnt[i][j]++;
                    }
                    trdp_cnt[i][j] = txb_info.mvb_info[i][j].recv_count;
                    reset_cnt[i][j] = 0;
                }
                else {
                    reset_cnt[i][j]++;
                    if(reset_cnt[i][j] == 5) {
                        trust_cnt[i][j] = 0;
                        trdp_cnt[i][j] = 0;
                        txb_info.mvb_info[i][j].recv_count = 0;
                        txb_info.mvb_info[i][j].com_state = 0;
                        sprintf(log_data, "train %d mcast recv %d trdp data failed ps: 0 is head, 1 is tail", i, j);
                        app_log_msg(LOG_LVL_INFO, true, log_data);
                    }
                }
            }
        }
        uint8_t i , j, k = 0;
        for(i = 0; i < TRAIN_MAX; i++) {
            for(j = 0; j < TRDP_DEV_MAX; j++) {
                if(txb_info.mvb_info[i][j].com_state && txb_info.mvb_info[i][j].data_state) {
                    txb_info.trust_mvb = &txb_info.mvb_info[i][j];
                    rt_kprintf("current trust public info is train_%d %d ps: 0 is head, 1 is tail\n", i, j);
                    not_trust_cnt = 0;
                    k = 1;
                    break;
                }
            }
            if(k) break;
        }

        if(i >= TRAIN_MAX) {
            if(!(not_trust_cnt % 10)) {
                app_log_msg(LOG_LVL_INFO, true, "all trdp data recv failed trdp trust info default");
                txb_info.trust_mvb = &txb_info.mvb_info[TRAIN_HEAD][TRDP_JLB];
            }
            not_trust_cnt++;
        }
    }

    if(!(count % 50)) {
        sprintf(log_data, "mcast recv trdp data head count %d-%d, tail count %d-%d", trdp_cnt[TRAIN_HEAD][TRDP_JLB], \
                trdp_cnt[TRAIN_HEAD][TRDP_TXB], trdp_cnt[TRAIN_TAIL][TRDP_JLB], trdp_cnt[TRAIN_TAIL][TRDP_TXB]);
        app_log_msg(LOG_LVL_INFO, true, log_data);
    }
#undef txb_info
}
#else
static void app_txb_info_update(uint16_t count)
{
    char log_data[100];
    sys_paras_t *paras = app_paras_get();
    static uint8_t head_trust_cnt = 0, tail_trust_cnt = 0;
    static uint16_t head_mvb_count = 0, head_reset_cnts = 0, \
                    tail_mvb_count = 0, tail_reset_cnts = 0;

#define txb_info app_mcast_env.txb_info

    if(txb_info.trust_mvb->com_state)
    {
        ccu_public_t * public_p = (ccu_public_t *)txb_info.trust_mvb->data;
        uint16_t wheel = env.clb_info.wheel;
        switch(paras->train_num)
        {
            case 1:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_1[0], public_p->wheel_1[1]);
                }
                break;
            case 2:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_2[0], public_p->wheel_2[1]);
                }
                break;
            case 3:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_3[0], public_p->wheel_3[1]);
                }
                break;
            case 4:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_4[0], public_p->wheel_4[1]);
                }
                break;
            case 5:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_5[0], public_p->wheel_5[1]);
                }
                break;
            case 6:
                if(public_p->valid_flag.bits.wheel_valid) {
                    wheel = DATA16(public_p->wheel_6[0], public_p->wheel_6[1]);
                }
                break;
            default:
                break;
        }
        if(env.clb_info.wheel != wheel)
        {
            memset(log_data, 0, sizeof(log_data));
            sprintf(log_data, "recv new wheel %d, old wheel %d", wheel, env.clb_info.wheel);
            app_log_msg(LOG_LVL_INFO, true, log_data);
            env.clb_info.wheel = wheel;
        }
        //协议中 (1 -- 0.01km/h) 已在通信板转换
        env.clb_info.speed = DATA16(public_p->speed[0], public_p->speed[1]);
        env.clb_info.speed /= 100;
        env.clb_info.now_id = DATA16(public_p->curr_id[0], public_p->curr_id[1]);
        env.clb_info.next_id = DATA16(public_p->next_id[0], public_p->next_id[1]);
    }
    else
    {
        if(env.clb_info.speed != 0)
        {
            memset(log_data, 0, sizeof(log_data));
            sprintf(log_data, "txb com state error, set speed %d -> 0", env.clb_info.speed);
            app_log_msg(LOG_LVL_INFO, true, log_data);
            //因为通信中断不知道现在车速是多少，考虑到诊断需要速度，未知速度容易误报警，其实可以将转速板的速度用起来，暂时先这样
            env.clb_info.speed = 0;
        }
    }


    if(!(count % 10))   //1s
    {
        //头车
        if(head_mvb_count != txb_info.mvb_info[TRAIN_HEAD].recv_count)
        {
            if(head_trust_cnt >= 5)
            {
                if(txb_info.mvb_info[TRAIN_HEAD].com_state != 1) {
                    app_log_msg(LOG_LVL_INFO, true, "train head mcast recv txb mvb data get right");
                }
                txb_info.mvb_info[TRAIN_HEAD].com_state = 1;
            }
            else {
                head_trust_cnt++;
            }
            head_mvb_count = txb_info.mvb_info[TRAIN_HEAD].recv_count;
            head_reset_cnts = 0;
        }
        else
        {
            head_reset_cnts++;
            if(head_reset_cnts == 5)  //在5秒内记录板没有收到头车MVB信息，则判定MVB数据异常
            {
                head_trust_cnt = 0;
                txb_info.mvb_info[TRAIN_HEAD].com_state = 0;
                app_log_msg(LOG_LVL_INFO, true, "train head mcast recv txb mvb data failed");
            }
        }
        //尾车
        if(tail_mvb_count != txb_info.mvb_info[TRAIN_TAIL].recv_count)
        {
            if(tail_trust_cnt >= 5)
            {
                if(txb_info.mvb_info[TRAIN_TAIL].com_state != 1) {
                    app_log_msg(LOG_LVL_INFO, true, "train tail mcast recv txb mvb data get right");
                }
                txb_info.mvb_info[TRAIN_TAIL].com_state = 1;
            }
            else {
                tail_trust_cnt++;
            }
            tail_mvb_count = txb_info.mvb_info[TRAIN_TAIL].recv_count;
            tail_reset_cnts = 0;
        }
        else
        {
            tail_reset_cnts++;
            if(tail_reset_cnts == 5)  //在5秒内记录板没有收到尾车MVB信息，则判定MVB数据异常
            {
                tail_trust_cnt = 0;
                txb_info.mvb_info[TRAIN_TAIL].com_state = 0;
                app_log_msg(LOG_LVL_INFO, true, "train tail mcast recv txb mvb data failed");
            }
        }

        //信任 add 连接有效不代表数据有效
        static uint8_t trust_cnt = 0;
        if(txb_info.mvb_info[TRAIN_HEAD].com_state && txb_info.mvb_info[TRAIN_HEAD].data_state) {
            trust_cnt = 0;
            txb_info.trust_mvb = &txb_info.mvb_info[TRAIN_HEAD];
        }
        else if(txb_info.mvb_info[TRAIN_TAIL].com_state && txb_info.mvb_info[TRAIN_TAIL].data_state) {
            trust_cnt = 0;
            txb_info.trust_mvb = &txb_info.mvb_info[TRAIN_TAIL];
        }
        else {
            if(!(trust_cnt % 10)) {
                app_log_msg(LOG_LVL_INFO, true, "all mvb data recv failed txb trust info default");
            }
            if(txb_info.mvb_info[TRAIN_HEAD].com_state) {
                txb_info.trust_mvb = &txb_info.mvb_info[TRAIN_HEAD];
            }
            else if(txb_info.mvb_info[TRAIN_TAIL].com_state) {
                txb_info.trust_mvb = &txb_info.mvb_info[TRAIN_TAIL];
            }
            else {
                txb_info.trust_mvb = &txb_info.mvb_info[TRAIN_HEAD];
            }
            trust_cnt++;
        }
    }


    if(!(count % 50))  //每5秒记录一次接收到的消息计数
    {
        sprintf(log_data, "mcast recv txb mvb data head count %d, tail %d count %d", txb_info.mvb_info[TRAIN_HEAD].recv_count,
                txb_info.mvb_info[TRAIN_TAIL].train_num, txb_info.mvb_info[TRAIN_TAIL].recv_count);
        app_log_msg(LOG_LVL_INFO, true, log_data);
    }

#undef txb_info
}
#endif
/**
 * @brief
 * @param
 * @return
 * @note
**/
static uint8_t app_get_head_tail(void)
{
    uint8_t state = 0;
    sys_paras_t *paras = app_paras_get();
    if((paras->group_num > 0) && (paras->group_num <= GROUP_NUM_6) && \
            (paras->train_num > 0) && (paras->train_num <= GROUP_NUM_6))
    {
        if(((paras->group_num == GROUP_NUM_4) && (paras->train_num == GROUP_NUM_4)) || \
                ((paras->group_num == GROUP_NUM_6) && (paras->train_num == GROUP_NUM_6)) || \
                (paras->train_num == 1))
        {
            state = 1;
        }
    }
    return state;
}

/**
 * @brief
 * @param
 * @return
 * @note   诊断结果设置为默认值
**/
void app_period_clb_diag_default(uint8_t *data, uint16_t size)
{
    if((data == RT_NULL) || (size != sizeof(clb_port_info_t))) {
        return;
    }
    clb_port_info_t *clb_info = (clb_port_info_t *)data;
    clb_port_msg_t *clb_msg = (clb_port_msg_t *)clb_info->data;
    uint8_t clean_alarm = 0xC0;
    //X车1-8位轴箱轴承及踏面二级、一级报警，预警
    for (int i=0; i<INDEX_MAX; i++)
    {
        clb_msg->zxzc_alarm[i].byte &= clean_alarm;
    }
    //X车1-4轴电机、齿轮箱
    for (int i=0; i<= INDEX_4; i++)
    {
        //电机轴承二级、一级报警，预警
        clb_msg->dj_alarm[i].byte &= clean_alarm;
        //小齿轮箱轴承及齿轮二级、一级报警，预警
        clb_msg->clx_alarm[i].byte &= clean_alarm;
    }
}
void app_period_clb_diag_default_qzq(uint8_t *data, uint16_t size, uint8_t qzq_num)
{
    if((data == RT_NULL) || (size != sizeof(clb_port_info_t))) {
        return;
    }
    clb_port_info_t *clb_info = (clb_port_info_t *)data;
    clb_port_msg_t *clb_msg = (clb_port_msg_t *)clb_info->data;
    uint8_t clean_alarm = 0xC0;
    sys_paras_t *paras = app_paras_get();
    switch (qzq_num) {
        case QZQ_1:
            if((paras->train_num == 1) || (paras->train_num == 6)) {
                for (int i = INDEX_1; i <= INDEX_4; i++) {
                    clb_msg->zxzc_alarm[i].byte &= clean_alarm;
                }
            } else {
                for (int i = INDEX_1; i <= INDEX_2; i++) {
                    clb_msg->zxzc_alarm[i].byte &= clean_alarm;
                }
                for (int i = INDEX_1; i <= INDEX_1; i++) {
                    //电机轴承二级、一级报警，预警
                    clb_msg->dj_alarm[i].byte &= clean_alarm;
                    //小齿轮箱轴承及齿轮二级、一级报警，预警
                    clb_msg->clx_alarm[i].byte &= clean_alarm;
                }
            }
            break;
        case QZQ_2:
            if((paras->train_num == 1) || (paras->train_num == 6)) {
                for (int i = INDEX_5; i < INDEX_MAX; i++) {
                    clb_msg->zxzc_alarm[i].byte &= clean_alarm;
                }
            } else {
                for (int i = INDEX_3; i <= INDEX_4; i++) {
                    clb_msg->zxzc_alarm[i].byte &= clean_alarm;
                }
                for (int i = INDEX_2; i <= INDEX_2; i++) {
                    //电机轴承二级、一级报警，预警
                    clb_msg->dj_alarm[i].byte &= clean_alarm;
                    //小齿轮箱轴承及齿轮二级、一级报警，预警
                    clb_msg->clx_alarm[i].byte &= clean_alarm;
                }
            }
            break;
        case QZQ_3:
            for (int i = INDEX_5; i <= INDEX_6; i++) {
                clb_msg->zxzc_alarm[i].byte &= clean_alarm;
            }
            for (int i = INDEX_3; i <= INDEX_3; i++) {
                //电机轴承二级、一级报警，预警
                clb_msg->dj_alarm[i].byte &= clean_alarm;
                //小齿轮箱轴承及齿轮二级、一级报警，预警
                clb_msg->clx_alarm[i].byte &= clean_alarm;
            }
            break;
        case QZQ_4:
            for (int i = INDEX_7; i <= INDEX_8; i++) {
                clb_msg->zxzc_alarm[i].byte &= clean_alarm;
            }
            for (int i = INDEX_4; i <= INDEX_4; i++) {
                //电机轴承二级、一级报警，预警
                clb_msg->dj_alarm[i].byte &= clean_alarm;
                //小齿轮箱轴承及齿轮二级、一级报警，预警
                clb_msg->clx_alarm[i].byte &= clean_alarm;
            }
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
static void app_period_clb_port_info(uint16_t count)
{
    sys_paras_t *paras = app_paras_get();
    clb_port_info_t *clb_info = (clb_port_info_t *)env.clb_data;
    clb_port_msg_t *clb_msg = (clb_port_msg_t *)clb_info->data;

    if(!(count % 10))   //1s
    {
        *(uint16_t *)clb_info->head = HEAD_MCAST_CLB_PORT_INFO;
        clb_info->size[0] = DATA16_H(sizeof(clb_port_info_t) - 5);
        clb_info->size[1] = DATA16_L(sizeof(clb_port_info_t) - 5);
        clb_msg->train_num = paras->train_num;
        clb_msg->version[0] = 1;
        clb_msg->version[1] = 0;
        uint8_t connect_cnt = 0;
        for(uint8_t i = 0; i < APP_TCPS_CONNECT_MAX; i++)
        {
            if(app_scast_env.tcps_info[i].connect == -1) {
                connect_cnt++;
            }
        }
        //1、判断TCPS有没有连接，如果有连接说明此时在进行设备的维护，发送默认的诊断结果
        //2、判断AD7606有没有初始化成功。诊断有没有初始化成功。
        if((connect_cnt == APP_TCPS_CONNECT_MAX) && (app_ad7606_env.ad7606_state == DEV_INIT_SUCCESS) && \
                (app_diag_env.dev_state == DEV_INIT_SUCCESS))
        {
            clb_msg->diag_state = 1;
        }
        else
        {
            if(clb_msg->diag_state != 0) {
                app_log_msg(LOG_LVL_INFO, true, "app diag init state error set diag default");
            }
            clb_msg->diag_state = 0;
            app_period_clb_diag_default(env.clb_data, sizeof(clb_port_info_t));
        }
        clb_msg->temp_state = 1;
        for(uint8_t i = 0; i < QZQ_MAX; i++)
        {
            qzq_data_t *qzq_data = (qzq_data_t *)&app_qzq_env.qzq_data[i];
            if((paras->train_num > 0) && (paras->train_num <= GROUP_NUM_6))
            {
                /* ***&&&***状态部分（这部分代码考虑到后期好定位写的很冗余，查看可以结合MVB通信协议很容易定位）***&&&*** */
                if(i == QZQ_1)
                {
                    //X车1号前置处理器故障（不区分4编组或者6编组，头尾车或者中间车）
                    clb_msg->qzq_fault.bits.qzq_1 = app_qzq_env.qzq_data[i].com_state ? 0 : 1;
                    //X车1位轴箱传感器故障（不区分4编组或者6编组，头尾车或者中间车）
                    clb_msg->zxzc_sensor.bits.num_1 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
                    //X车1位轴箱轴承状态清零
                    clb_msg->zxzc_alarm[AXLE_1].byte = 0;
                    //X车1位轴箱温度报警
                    clb_msg->zxzc_alarm[AXLE_1].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_ALARM) ? 1 : 0;
                    //X车1位轴箱温度预警
                    clb_msg->zxzc_alarm[AXLE_1].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_WARN) ? 1 : 0;
                    if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_1])
                    {
                        //X车1位 踏面 二级、一级报警、预警
                        if(app_diag_env.diag_msg.tread[AXLE_1] == DIAG_ALARM_2){
                            clb_msg->zxzc_alarm[AXLE_1].bits.tm_2_alarm = 1;
                        }
                        else if(app_diag_env.diag_msg.tread[AXLE_1] == DIAG_ALARM_1){
                            clb_msg->zxzc_alarm[AXLE_1].bits.tm_1_alarm = 1;
                        }
                        else if(app_diag_env.diag_msg.tread[AXLE_1] == DIAG_WARN){
                            clb_msg->zxzc_alarm[AXLE_1].bits.tm_warning = 1;
                        }
                        //X车1位 轴箱轴承 二级、一级报警、预警
                        if(app_diag_env.diag_msg.axle_bearing[AXLE_1] == DIAG_ALARM_2){
                            clb_msg->zxzc_alarm[AXLE_1].bits.zc_2_alarm = 1;
                        }
                        else if(app_diag_env.diag_msg.axle_bearing[AXLE_1] == DIAG_ALARM_1){
                            clb_msg->zxzc_alarm[AXLE_1].bits.zc_1_alarm = 1;
                        }
                        else if(app_diag_env.diag_msg.axle_bearing[AXLE_1] == DIAG_WARN){
                            clb_msg->zxzc_alarm[AXLE_1].bits.zc_warning = 1;
                        }
                     }

                    //X车2位轴箱传感器故障（不区分4编组或者6编组，头尾车或者中间车）
                    clb_msg->zxzc_sensor.bits.num_2 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
                    //X车2位轴箱轴承状态清零
                    clb_msg->zxzc_alarm[AXLE_2].byte = 0;
                    //X车2位轴箱温度预警报警
                    clb_msg->zxzc_alarm[AXLE_2].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_ALARM) ? 1 : 0;
                    clb_msg->zxzc_alarm[AXLE_2].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_WARN) ? 1 : 0;
                    if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_2])
                    {
                        //X车2位 踏面 二级、一级报警、预警
                        if(app_diag_env.diag_msg.tread[AXLE_2] == DIAG_ALARM_2){
                            clb_msg->zxzc_alarm[AXLE_2].bits.tm_2_alarm = 1;
                        }
                        else if(app_diag_env.diag_msg.tread[AXLE_2] == DIAG_ALARM_1){
                            clb_msg->zxzc_alarm[AXLE_2].bits.tm_1_alarm = 1;
                        }
                        else if(app_diag_env.diag_msg.tread[AXLE_2] == DIAG_WARN){
                            clb_msg->zxzc_alarm[AXLE_2].bits.tm_warning = 1;
                        }
                        //X车2位 轴承 二级、一级报警、预警
                        if(app_diag_env.diag_msg.axle_bearing[AXLE_2] == DIAG_ALARM_2){
                            clb_msg->zxzc_alarm[AXLE_2].bits.zc_2_alarm = 1;
                        }
                        else if(app_diag_env.diag_msg.axle_bearing[AXLE_2] == DIAG_ALARM_1){
                            clb_msg->zxzc_alarm[AXLE_2].bits.zc_1_alarm = 1;
                        }
                        else if(app_diag_env.diag_msg.axle_bearing[AXLE_2] == DIAG_WARN){
                            clb_msg->zxzc_alarm[AXLE_2].bits.zc_warning = 1;
                        }
                    }

                    if(app_get_head_tail())
                    {
                        //X车3位轴箱传感器故障（4编组或者6编组头尾车，在前置器1的3位）
                        clb_msg->zxzc_sensor.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                        //X车3位轴箱轴承状态清零
                        clb_msg->zxzc_alarm[INDEX_3].byte = 0;
                        //X车3位轴箱温度预警、报警
                        clb_msg->zxzc_alarm[INDEX_3].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->zxzc_alarm[INDEX_3].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_3])
                        {
                            //X车3位 踏面 二级、一级报警、预警
                            if(app_diag_env.diag_msg.tread[AXLE_3] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_3].bits.tm_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_3] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_3].bits.tm_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_3] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_3].bits.tm_warning = 1;
                            }
                            //X车3位 轴箱轴承 二级、一级报警，预警
                            if(app_diag_env.diag_msg.axle_bearing[AXLE_3] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_3].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_3] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_3].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_3] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_3].bits.zc_warning = 1;
                            }
                        }

                        //X车4位轴箱传感器故障（4编组或者6编组头尾车，在前置器1的4位）
                        clb_msg->zxzc_sensor.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                        //X车4位轴箱轴承状态清零
                        clb_msg->zxzc_alarm[AXLE_4].byte = 0;
                        //X车4位轴箱温度报警、预警
                        clb_msg->zxzc_alarm[AXLE_4].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->zxzc_alarm[AXLE_4].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_4])
                        {
                            //X车4位踏面二级、一级报警，预警
                            if(app_diag_env.diag_msg.tread[AXLE_4] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[AXLE_4].bits.tm_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_4] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[AXLE_4].bits.tm_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_4] == DIAG_WARN){
                                clb_msg->zxzc_alarm[AXLE_4].bits.tm_warning = 1;
                            }
                            //X车4位轴箱轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.axle_bearing[AXLE_4] == DIAG_ALARM_2)
                            {
                                clb_msg->zxzc_alarm[AXLE_4].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_4] == DIAG_ALARM_1)
                            {
                                clb_msg->zxzc_alarm[AXLE_4].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_4] == DIAG_WARN)
                            {
                                clb_msg->zxzc_alarm[AXLE_4].bits.zc_warning = 1;
                            }
                        }
                    }
                    else
                    {
                        //X车1轴电机传感器故障（4编组或者6编组，中间车在前置器1的4位）
                        clb_msg->dj_sensor.bits.num_1 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                        //X车1位电机状态清零
                        clb_msg->dj_alarm[INDEX_1].byte = 0;
                        //X车1轴电机温度报警、预警
                        clb_msg->dj_alarm[INDEX_1].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->dj_alarm[INDEX_1].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_4])
                        {
                            //X车1轴电机轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_1] == DIAG_ALARM_2){
                                clb_msg->dj_alarm[INDEX_1].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_1] == DIAG_ALARM_1){
                                clb_msg->dj_alarm[INDEX_1].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_1] == DIAG_WARN){
                                clb_msg->dj_alarm[INDEX_1].bits.zc_warning = 1;
                            }
                        }
                        //X车1轴齿轮箱传感器故障（4编组或者6编组，中间车在前置器1的3位）
                        clb_msg->clx_sensor.bits.num_1 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                        //X车1位齿轮箱状态清零
                        clb_msg->clx_alarm[INDEX_1].byte = 0;
                        //X车1轴齿轮温度报警、预警
                        clb_msg->clx_alarm[INDEX_1].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->clx_alarm[INDEX_1].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_3])
                        {
                            //X车1轴小齿轮箱轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_1] == DIAG_ALARM_2){
                                clb_msg->clx_alarm[INDEX_1].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_1] == DIAG_ALARM_1){
                                clb_msg->clx_alarm[INDEX_1].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_1] == DIAG_WARN){
                                clb_msg->clx_alarm[INDEX_1].bits.zc_warning = 1;
                            }
                            //X车1轴小齿轮箱齿轮二级、一级报警，预警
                            if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_1] == DIAG_ALARM_2){
                                clb_msg->clx_alarm[INDEX_1].bits.cl_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_1] == DIAG_ALARM_1){
                                clb_msg->clx_alarm[INDEX_1].bits.cl_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_1] == DIAG_WARN){
                                clb_msg->clx_alarm[INDEX_1].bits.cl_warning = 1;
                            }
                        }
                    }
                }
                else if(i == QZQ_2)
                {
                    //X车2号前置处理器故障（不区分4编组或者6编组，头尾车或者中间）
                    clb_msg->qzq_fault.bits.qzq_2 = app_qzq_env.qzq_data[i].com_state ? 0 : 1;
                    if(app_get_head_tail())
                    {
                        //X车5位轴箱传感器故障（4编组或者6编组头尾车，在前置器2的1位）
                        clb_msg->zxzc_sensor.bits.num_5 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
                        //X车5位轴箱报警状态清零
                        clb_msg->zxzc_alarm[INDEX_5].byte = 0;
                        //X车5位轴箱温度报警、预警
                        clb_msg->zxzc_alarm[INDEX_5].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->zxzc_alarm[INDEX_5].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_1])
                        {
                            //X车5位踏面二级、一级报警，预警
                            if(app_diag_env.diag_msg.tread[AXLE_5] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_5].bits.tm_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_5] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_5].bits.tm_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_5] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_5].bits.tm_warning = 1;
                            }
                            //X车5位轴箱轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.axle_bearing[AXLE_5] == DIAG_ALARM_2)
                            {
                                clb_msg->zxzc_alarm[INDEX_5].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_5] == DIAG_ALARM_1)
                            {
                                clb_msg->zxzc_alarm[INDEX_5].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_5] == DIAG_WARN)
                            {
                                clb_msg->zxzc_alarm[INDEX_5].bits.zc_warning = 1;
                            }

                        }

                        //X车6位轴箱传感器故障（4编组或者6编组头尾车，在前置器2的2位）
                        clb_msg->zxzc_sensor.bits.num_6 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
                        //X车6位轴箱报警状态清零
                        clb_msg->zxzc_alarm[INDEX_6].byte = 0;
                        //X车6位轴箱温度报警
                        clb_msg->zxzc_alarm[INDEX_6].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->zxzc_alarm[INDEX_6].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_2])
                        {
                            //X车6位踏面二级、一级报警，预警
                            if(app_diag_env.diag_msg.tread[AXLE_6] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_6].bits.tm_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_6] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_6].bits.tm_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_6] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_6].bits.tm_warning = 1;
                            }
                            //X车6位轴箱轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.axle_bearing[AXLE_6] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_6].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_6] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_6].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_6] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_6].bits.zc_warning = 1;
                            }
                        }
                        if(paras->group_num == GROUP_NUM_4)
                        {
                            //X车3轴电机传感器故障（4编组头尾车，在前置器2的4位）
                            clb_msg->dj_sensor.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                            //X车3轴电机报警状态清零
                            clb_msg->dj_alarm[INDEX_3].byte = 0;
                            //X车3轴电机温度报警
                            clb_msg->dj_alarm[INDEX_3].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_ALARM) ? 1 : 0;
                            clb_msg->dj_alarm[INDEX_3].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_WARN) ? 1 : 0;
                            if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_3])
                            {
                                //X车3位电机轴承二级、一级报警，预警
                                if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_3] == DIAG_ALARM_2){
                                    clb_msg->dj_alarm[INDEX_3].bits.zc_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_3] == DIAG_ALARM_1){
                                    clb_msg->dj_alarm[INDEX_3].bits.zc_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_3] == DIAG_WARN){
                                    clb_msg->dj_alarm[INDEX_3].bits.zc_warning = 1;
                                }
                            }

                            //X车3轴齿轮传感器故障（4编组头尾车，在前置器2的3位）
                            clb_msg->clx_sensor.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                            //X车3轴齿轮报警状态清零
                            clb_msg->clx_alarm[INDEX_4].byte = 0;
                            //X车3轴齿轮温度报警、预警
                            clb_msg->clx_alarm[INDEX_4].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_ALARM) ? 1 : 0;
                            clb_msg->clx_alarm[INDEX_4].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_WARN) ? 1 : 0;
                            if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_4])
                            {
                                //X车3位齿轮箱轴承二级、一级报警，预警
                                if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_4] == DIAG_ALARM_2){
                                    clb_msg->clx_alarm[INDEX_4].bits.zc_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_4] == DIAG_ALARM_1){
                                    clb_msg->clx_alarm[INDEX_4].bits.zc_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_4] == DIAG_WARN){
                                    clb_msg->clx_alarm[INDEX_4].bits.zc_warning = 1;
                                }
                                //X车3位齿轮箱齿轮二级、一级报警，预警
                                if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_4] == DIAG_ALARM_2){
                                    clb_msg->clx_alarm[INDEX_4].bits.cl_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_4] == DIAG_ALARM_1){
                                    clb_msg->clx_alarm[INDEX_4].bits.cl_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_4] == DIAG_WARN){
                                    clb_msg->clx_alarm[INDEX_4].bits.cl_warning = 1;
                                }
                            }
                        }
                        else if(paras->group_num == GROUP_NUM_6)
                        {
                            //X车7位轴箱传感器故障（6编组头尾车，在前置器2的3位）
                            clb_msg->zxzc_sensor.bits.num_7 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                            //X车7位轴箱报警状态清零
                            clb_msg->zxzc_alarm[INDEX_7].byte = 0;
                            //X车7位轴箱温度报警、预警
                            clb_msg->zxzc_alarm[INDEX_7].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_ALARM) ? 1 : 0;
                            clb_msg->zxzc_alarm[INDEX_7].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_WARN) ? 1 : 0;
                            if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_3])
                            {
                                //X车7位踏面二级、一级报警，预警
                                if(app_diag_env.diag_msg.tread[AXLE_7] == DIAG_ALARM_2){
                                    clb_msg->zxzc_alarm[INDEX_7].bits.tm_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.tread[AXLE_7] == DIAG_ALARM_1){
                                    clb_msg->zxzc_alarm[INDEX_7].bits.tm_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.tread[AXLE_7] == DIAG_WARN){
                                    clb_msg->zxzc_alarm[INDEX_7].bits.tm_warning = 1;
                                }
                                //X车7位轴箱轴承二级、一级报警，预警
                                if(app_diag_env.diag_msg.axle_bearing[AXLE_7] == DIAG_ALARM_2){
                                    clb_msg->zxzc_alarm[INDEX_7].bits.zc_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.axle_bearing[AXLE_7] == DIAG_ALARM_1){
                                    clb_msg->zxzc_alarm[INDEX_7].bits.zc_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.axle_bearing[AXLE_7] == DIAG_WARN){
                                    clb_msg->zxzc_alarm[INDEX_7].bits.zc_warning = 1;
                                }
                            }

                            //X车8位轴箱传感器故障（6编组头尾车，在前置器2的4位）
                            clb_msg->zxzc_sensor.bits.num_8 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                            //X车8位轴箱报警状态清零
                            clb_msg->zxzc_alarm[INDEX_8].byte = 0;
                            //X车8位轴箱温度报警
                            clb_msg->zxzc_alarm[INDEX_8].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_ALARM) ? 1 : 0;
                            //X车8位轴箱温度预警
                            clb_msg->zxzc_alarm[INDEX_8].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_WARN) ? 1 : 0;
                            if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_4])
                            {
                                //X车8位踏面二级、一级报警，预警
                                if(app_diag_env.diag_msg.tread[AXLE_8] == DIAG_ALARM_2){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.tm_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.tread[AXLE_8] == DIAG_ALARM_1){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.tm_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.tread[AXLE_8] == DIAG_WARN){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.tm_warning = 1;
                                }
                                //X车8位轴箱轴承二级、一级报警，预警
                                if(app_diag_env.diag_msg.axle_bearing[AXLE_8] == DIAG_ALARM_2){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.zc_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.axle_bearing[AXLE_8] == DIAG_ALARM_1){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.zc_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.axle_bearing[AXLE_8] == DIAG_WARN){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.zc_warning = 1;
                                }
                            }
                        }
                    }
                    else
                    {
                        //X车3位轴箱传感器故障（4编组或者6编组，中间车在前置器2的1位）
                        clb_msg->zxzc_sensor.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
                        //X车3位轴箱报警状态清零
                        clb_msg->zxzc_alarm[INDEX_3].byte = 0;
                        //X车3位轴箱温度报警、预警
                        clb_msg->zxzc_alarm[INDEX_3].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->zxzc_alarm[INDEX_3].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_1])
                        {
                            //X车3位 踏面 二级、一级报警、预警
                            if(app_diag_env.diag_msg.tread[AXLE_3] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_3].bits.tm_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_3] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_3].bits.tm_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_3] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_3].bits.tm_warning = 1;
                            }
                            //X车3位 轴箱轴承 二级、一级报警，预警
                            if(app_diag_env.diag_msg.axle_bearing[AXLE_3] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_3].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_3] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_3].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_3] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_3].bits.zc_warning = 1;
                            }
                        }

                        //X车4位轴箱传感器故障（4编组或者6编组，中间车在前置器2的2位）
                        clb_msg->zxzc_sensor.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
                        //X车4位轴箱报警状态清零
                        clb_msg->zxzc_alarm[INDEX_4].byte = 0;
                        //X车4位轴箱温度报警、预警
                        clb_msg->zxzc_alarm[INDEX_4].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->zxzc_alarm[INDEX_4].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_2])
                        {
                            //X车4位踏面二级、一级报警，预警
                            if(app_diag_env.diag_msg.tread[AXLE_4] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_4].bits.tm_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_4] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_4].bits.tm_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_4] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_4].bits.tm_warning = 1;
                            }
                            //X车4位轴箱轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.axle_bearing[AXLE_4] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_4].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_4] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_4].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_4] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_4].bits.zc_warning = 1;
                            }
                        }
                        //X车2轴电机传感器故障（4编组或者6编组中间车在前置器2的4位）
                        clb_msg->dj_sensor.bits.num_2 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                        //X车2轴电机报警状态清零
                        clb_msg->dj_alarm[INDEX_2].byte = 0;
                        //X车2轴电机温度报警、预警
                        clb_msg->dj_alarm[INDEX_2].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->dj_alarm[INDEX_2].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_4])
                        {
                            //X车2轴电机轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_2] == DIAG_ALARM_2){
                                clb_msg->dj_alarm[INDEX_2].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_2] == DIAG_ALARM_1){
                                clb_msg->dj_alarm[INDEX_2].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_2] == DIAG_WARN){
                                clb_msg->dj_alarm[INDEX_2].bits.zc_warning = 1;
                            }
                        }
                        //X车2轴齿轮传感器故障（4编组或者6编组中间车在前置器2的3位）
                        clb_msg->clx_sensor.bits.num_2 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                        //X车2轴齿轮温度报警
                        clb_msg->clx_alarm[INDEX_2].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_ALARM) ? 1 : 0;
                        //X车2轴齿轮温度预警
                        clb_msg->clx_alarm[INDEX_2].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_3])
                        {
                            //X车2轴小齿轮箱轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_2] == DIAG_ALARM_2){
                                clb_msg->clx_alarm[INDEX_2].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_2] == DIAG_ALARM_1){
                                clb_msg->clx_alarm[INDEX_2].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_2] == DIAG_WARN){
                                clb_msg->clx_alarm[INDEX_2].bits.zc_warning = 1;
                            }
                            //X车2轴小齿轮箱齿轮二级、一级报警，预警
                            if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_2] == DIAG_ALARM_2){
                                clb_msg->clx_alarm[INDEX_2].bits.cl_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_2] == DIAG_ALARM_1){
                                clb_msg->clx_alarm[INDEX_2].bits.cl_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_2] == DIAG_WARN){
                                clb_msg->clx_alarm[INDEX_2].bits.cl_warning = 1;
                            }
                        }
                    }
                }
                else if(i == QZQ_3)
                {
                    if(app_get_head_tail())
                    {
                        if(paras->group_num == GROUP_NUM_4)
                        {
                            //X车3号前置处理器故障（4编组或者6编组，只用4编组的头尾车有）
                            clb_msg->qzq_fault.bits.qzq_3 = app_qzq_env.qzq_data[i].com_state ? 0 : 1;
                            //X车7位轴箱传感器故障（4编组头尾车，在前置器3的1位）
                            clb_msg->zxzc_sensor.bits.num_7 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
                            //X车7位轴箱报警状态清零
                            clb_msg->zxzc_alarm[INDEX_7].byte = 0;
                            //X车7位轴箱温度报警、预警
                            clb_msg->zxzc_alarm[INDEX_7].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_ALARM) ? 1 : 0;
                            clb_msg->zxzc_alarm[INDEX_7].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_WARN) ? 1 : 0;
                            if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_1])
                            {
                                //X车7位踏面二级、一级报警，预警
                                 if(app_diag_env.diag_msg.tread[AXLE_7] == DIAG_ALARM_2){
                                     clb_msg->zxzc_alarm[INDEX_7].bits.tm_2_alarm = 1;
                                 }
                                 else if(app_diag_env.diag_msg.tread[AXLE_7] == DIAG_ALARM_1){
                                     clb_msg->zxzc_alarm[INDEX_7].bits.tm_1_alarm = 1;
                                 }
                                 else if(app_diag_env.diag_msg.tread[AXLE_7] == DIAG_WARN){
                                     clb_msg->zxzc_alarm[INDEX_7].bits.tm_warning = 1;
                                 }
                                 //X车7位轴箱轴承二级、一级报警，预警
                                 if(app_diag_env.diag_msg.axle_bearing[AXLE_7] == DIAG_ALARM_2){
                                     clb_msg->zxzc_alarm[INDEX_7].bits.zc_2_alarm = 1;
                                 }
                                 else if(app_diag_env.diag_msg.axle_bearing[AXLE_7] == DIAG_ALARM_1){
                                     clb_msg->zxzc_alarm[INDEX_7].bits.zc_1_alarm = 1;
                                 }
                                 else if(app_diag_env.diag_msg.axle_bearing[AXLE_7] == DIAG_WARN){
                                     clb_msg->zxzc_alarm[INDEX_7].bits.zc_warning = 1;
                                 }
                            }
                            //X车8位轴箱传感器故障（4编组头尾车，在前置器3的2位）
                            clb_msg->zxzc_sensor.bits.num_8 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
                            //X车8位轴箱报警状态清零
                            clb_msg->zxzc_alarm[INDEX_8].byte = 0;
                            //X车8位轴箱温度报警、报警
                            clb_msg->zxzc_alarm[INDEX_8].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_ALARM) ? 1 : 0;
                            clb_msg->zxzc_alarm[INDEX_8].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_WARN) ? 1 : 0;
                            if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_2])
                            {
                                //X车8位踏面二级、一级报警，预警
                                if(app_diag_env.diag_msg.tread[AXLE_8] == DIAG_ALARM_2){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.tm_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.tread[AXLE_8] == DIAG_ALARM_1){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.tm_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.tread[AXLE_8] == DIAG_WARN){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.tm_warning = 1;
                                }
                                //X车8位轴箱轴承二级、一级报警，预警
                                if(app_diag_env.diag_msg.axle_bearing[AXLE_8] == DIAG_ALARM_2){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.zc_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.axle_bearing[AXLE_8] == DIAG_ALARM_1){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.zc_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.axle_bearing[AXLE_8] == DIAG_WARN){
                                    clb_msg->zxzc_alarm[INDEX_8].bits.zc_warning = 1;
                                }
                            }

                            //X车4轴电机传感器故障（4编组头尾车，在前置器3的4位）
                            clb_msg->dj_sensor.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                            //X车4轴电机报警状态清零
                            clb_msg->dj_alarm[INDEX_4].byte = 0;
                            //X车4轴电机温度报警、预警
                            clb_msg->dj_alarm[INDEX_4].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_ALARM) ? 1 : 0;
                            clb_msg->dj_alarm[INDEX_4].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_WARN) ? 1 : 0;
                            if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_4])
                            {
                                //X车4位电机轴承二级、一级报警，预警
                                if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_4] == DIAG_ALARM_2){
                                    clb_msg->dj_alarm[INDEX_4].bits.zc_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_4] == DIAG_ALARM_1){
                                    clb_msg->dj_alarm[INDEX_4].bits.zc_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_4] == DIAG_WARN){
                                    clb_msg->dj_alarm[INDEX_4].bits.zc_warning = 1;
                                }
                            }

                            //X车4轴齿轮传感器故障（4编组头尾车，在前置器3的3位）
                            clb_msg->clx_sensor.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                            //X车4轴齿轮箱报警状态清零
                            clb_msg->clx_alarm[INDEX_4].byte = 0;
                            //X车4轴齿轮温度报警、预警
                            clb_msg->clx_alarm[INDEX_4].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_ALARM) ? 1 : 0;
                            clb_msg->clx_alarm[INDEX_4].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_WARN) ? 1 : 0;
                            if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_3])
                            {
                                //X车4轴小齿轮箱轴承二级、一级报警，预警
                                if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_4] == DIAG_ALARM_2){
                                    clb_msg->clx_alarm[INDEX_4].bits.zc_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_4] == DIAG_ALARM_1){
                                    clb_msg->clx_alarm[INDEX_4].bits.zc_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_4] == DIAG_WARN){
                                    clb_msg->clx_alarm[INDEX_4].bits.zc_warning = 1;
                                }
                                //X车4轴小齿轮箱齿轮二级、一级报警，预警
                                if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_4] == DIAG_ALARM_2){
                                    clb_msg->clx_alarm[INDEX_4].bits.cl_2_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_4] == DIAG_ALARM_1){
                                    clb_msg->clx_alarm[INDEX_4].bits.cl_1_alarm = 1;
                                }
                                else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_4] == DIAG_WARN){
                                    clb_msg->clx_alarm[INDEX_4].bits.cl_warning = 1;
                                }
                            }
                        }
                    }
                    else
                    {
                        //X车3号前置处理器故障（4编组或者6编组中间车都有3号前置器）
                        clb_msg->qzq_fault.bits.qzq_3 = app_qzq_env.qzq_data[i].com_state ? 0 : 1;
                        //X车5位轴箱传感器故障（4编组或者6编组中间车在前置器3的1位）
                        clb_msg->zxzc_sensor.bits.num_5 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
                        //X车5位轴箱报警位清零
                        clb_msg->zxzc_alarm[INDEX_5].byte = 0;
                        //X车5位轴箱温度报警、预警
                        clb_msg->zxzc_alarm[INDEX_5].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->zxzc_alarm[INDEX_5].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_1])
                        {
                            //X车5位踏面二级、一级报警，预警
                            if(app_diag_env.diag_msg.tread[AXLE_5] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_5].bits.tm_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_5] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_5].bits.tm_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_5] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_5].bits.tm_warning = 1;
                            }
                            //X车5位轴箱轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.axle_bearing[AXLE_5] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_5].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_5] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_5].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_5] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_5].bits.zc_warning = 1;
                            }
                        }

                        //X车6位轴箱传感器故障（4编组或者6编组中间车在前置器3的2位）
                        clb_msg->zxzc_sensor.bits.num_6 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
                        //X车6位轴箱报警位清零
                        clb_msg->zxzc_alarm[INDEX_6].byte = 0;
                        //X车6位轴箱温度报警、预警
                        clb_msg->zxzc_alarm[INDEX_6].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->zxzc_alarm[INDEX_6].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_2])
                        {
                            //X车6位踏面二级、一级报警，预警
                            if(app_diag_env.diag_msg.tread[AXLE_6] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_6].bits.tm_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_6] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_6].bits.tm_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_6] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_6].bits.tm_warning = 1;
                            }
                            //X车6位轴箱轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.axle_bearing[AXLE_6] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_6].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_6] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_6].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_6] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_6].bits.zc_warning = 1;
                            }
                        }

                        //X车3轴电机传感器故障（4编组或者6编组中间车在前置器3的4位）
                        clb_msg->dj_sensor.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                        //X车3轴电机报警位清零
                        clb_msg->dj_alarm[INDEX_3].byte = 0;
                        //X车3轴电机温度报警、预警
                        clb_msg->dj_alarm[INDEX_3].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->dj_alarm[INDEX_3].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_4])
                        {
                            if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_3] == DIAG_ALARM_2){
                                clb_msg->dj_alarm[INDEX_3].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_3] == DIAG_ALARM_1){
                                clb_msg->dj_alarm[INDEX_3].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_3] == DIAG_WARN){
                                clb_msg->dj_alarm[INDEX_3].bits.zc_warning = 1;
                            }
                        }
                        //X车3轴齿轮传感器故障（4编组或者6编组中间车在前置器3的3位）
                        clb_msg->clx_sensor.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                        //X车3轴齿轮箱报警位清零
                        clb_msg->clx_alarm[INDEX_3].byte = 0;
                        //X车3轴齿轮温度报警、预警
                        clb_msg->clx_alarm[INDEX_3].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->clx_alarm[INDEX_3].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_3])
                        {
                            //X车3轴小齿轮箱轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_3] == DIAG_ALARM_2){
                                clb_msg->clx_alarm[INDEX_3].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_3] == DIAG_ALARM_1){
                                clb_msg->clx_alarm[INDEX_3].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_3] == DIAG_WARN){
                                clb_msg->clx_alarm[INDEX_3].bits.zc_warning = 1;
                            }
                            //X车3轴小齿轮箱齿轮二级、一级报警，预警
                            if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_3] == DIAG_ALARM_2){
                                clb_msg->clx_alarm[INDEX_3].bits.cl_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_3] == DIAG_ALARM_1){
                                clb_msg->clx_alarm[INDEX_3].bits.cl_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_3] == DIAG_WARN){
                                clb_msg->clx_alarm[INDEX_3].bits.cl_warning = 1;
                            }
                        }
                    }
                }
                else if(i == QZQ_4)
                {
                    if(!app_get_head_tail()) //中间车
                    {
                        //X车4号前置处理器故障（4编组或者6编组，只用中间车有4号前置处理器）
                        clb_msg->qzq_fault.bits.qzq_4 = app_qzq_env.qzq_data[i].com_state ? 0 : 1;
                        //X车7位轴箱传感器故障（4编组或者6编组，中间车在前置器4的1位）
                        clb_msg->zxzc_sensor.bits.num_7 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
                        //X车7位轴箱报警位清零
                        clb_msg->zxzc_alarm[INDEX_7].byte = 0;
                        //X车7位轴箱温度报警、预警
                        clb_msg->zxzc_alarm[INDEX_7].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->zxzc_alarm[INDEX_7].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT1].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_1])
                        {
                            //X车7位踏面二级、一级报警，预警
                             if(app_diag_env.diag_msg.tread[AXLE_7] == DIAG_ALARM_2){
                                 clb_msg->zxzc_alarm[INDEX_7].bits.tm_2_alarm = 1;
                             }
                             else if(app_diag_env.diag_msg.tread[AXLE_7] == DIAG_ALARM_1){
                                 clb_msg->zxzc_alarm[INDEX_7].bits.tm_1_alarm = 1;
                             }
                             else if(app_diag_env.diag_msg.tread[AXLE_7] == DIAG_WARN){
                                 clb_msg->zxzc_alarm[INDEX_7].bits.tm_warning = 1;
                             }
                             //X车7位轴箱轴承二级、一级报警，预警
                             if(app_diag_env.diag_msg.axle_bearing[AXLE_7] == DIAG_ALARM_2){
                                 clb_msg->zxzc_alarm[INDEX_7].bits.zc_2_alarm = 1;
                             }
                             else if(app_diag_env.diag_msg.axle_bearing[AXLE_7] == DIAG_ALARM_1){
                                 clb_msg->zxzc_alarm[INDEX_7].bits.zc_1_alarm = 1;
                             }
                             else if(app_diag_env.diag_msg.axle_bearing[AXLE_7] == DIAG_WARN){
                                 clb_msg->zxzc_alarm[INDEX_7].bits.zc_warning = 1;
                             }
                        }

                        //X车8位轴箱传感器故障（4编组或者6编组，中间车在前置器4的2位）
                        clb_msg->zxzc_sensor.bits.num_8 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
                        //X车8位轴箱报警位清零
                        clb_msg->zxzc_alarm[INDEX_8].byte = 0;
                        //X车8位轴箱温度报警、预警
                        clb_msg->zxzc_alarm[INDEX_8].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->zxzc_alarm[INDEX_8].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT2].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_2])
                        {
                            //X车8位踏面二级、一级报警，预警
                            if(app_diag_env.diag_msg.tread[AXLE_8] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_8].bits.tm_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_8] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_8].bits.tm_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.tread[AXLE_8] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_8].bits.tm_warning = 1;
                            }
                            //X车8位轴箱轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.axle_bearing[AXLE_8] == DIAG_ALARM_2){
                                clb_msg->zxzc_alarm[INDEX_8].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_8] == DIAG_ALARM_1){
                                clb_msg->zxzc_alarm[INDEX_8].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.axle_bearing[AXLE_8] == DIAG_WARN){
                                clb_msg->zxzc_alarm[INDEX_8].bits.zc_warning = 1;
                            }
                        }
                        //X车4轴电机传感器故障（4编组或者6编组，中间车在前置器4的4位）
                        clb_msg->dj_sensor.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                        //X车4轴电机报警状态清零
                        clb_msg->dj_alarm[INDEX_4].byte = 0;
                        //X车4轴电机温度报警、预警
                        clb_msg->dj_alarm[INDEX_4].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_ALARM) ? 1 : 0;
                        clb_msg->dj_alarm[INDEX_4].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT4].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_4])
                        {
                            //X车4位电机轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_4] == DIAG_ALARM_2){
                                clb_msg->dj_alarm[INDEX_4].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_4] == DIAG_ALARM_1){
                                clb_msg->dj_alarm[INDEX_4].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.motor_bearing[AXLE_MOTOR_4] == DIAG_WARN){
                                clb_msg->dj_alarm[INDEX_4].bits.zc_warning = 1;
                            }
                        }

                        //X车4轴齿轮传感器故障（4编组或者6编组，中间车在前置器4的3位）
                        clb_msg->clx_sensor.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                        //X车4轴齿轮箱报警状态清零
                        clb_msg->clx_alarm[INDEX_4].byte = 0;
                        //X车4轴齿轮温度报警
                        clb_msg->clx_alarm[INDEX_4].bits.temp_alarm = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_ALARM) ? 1 : 0;
                        //X车4轴齿轮温度预警
                        clb_msg->clx_alarm[INDEX_4].bits.temp_warning = (qzq_data->temp_diag[TEMP_PT3].temp_state == TEMP_WARN) ? 1 : 0;
                        if(clb_msg->diag_state && app_qzq_env.qzq_data[i].com_state && app_qzq_env.qzq_data[i].vib_state[ADG1609_CH_3])
                        {
                            //X车4轴小齿轮箱轴承二级、一级报警，预警
                            if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_4] == DIAG_ALARM_2){
                                clb_msg->clx_alarm[INDEX_4].bits.zc_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_4] == DIAG_ALARM_1){
                                clb_msg->clx_alarm[INDEX_4].bits.zc_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_bearing[AXLE_GEAR_4] == DIAG_WARN){
                                clb_msg->clx_alarm[INDEX_4].bits.zc_warning = 1;
                            }
                            //X车4轴小齿轮箱齿轮二级、一级报警，预警
                            if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_4] == DIAG_ALARM_2){
                                clb_msg->clx_alarm[INDEX_4].bits.cl_2_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_4] == DIAG_ALARM_1){
                                clb_msg->clx_alarm[INDEX_4].bits.cl_1_alarm = 1;
                            }
                            else if(app_diag_env.diag_msg.box_gear[AXLE_GEAR_4] == DIAG_WARN){
                                clb_msg->clx_alarm[INDEX_4].bits.cl_warning = 1;
                            }
                        }
                    }
                }
                /* ***&&&***温度部分（这部分代码考虑到后期好定位写的很冗余，查看可以结合MVB通信协议很容易定位）***&&&*** */
                //4编组或者6编组，头尾车3位轴箱温度和4位轴箱温度在前置器1的3、4位，中间车在前置器2的1、2位
                if(i == QZQ_1)
                {
                    //X车1位轴箱温度
                    clb_msg->zx_temp[INDEX_1] = qzq_data->temp_diag[TEMP_PT1].temp_data;  //这个对于关系需要查看接线图
                    //X车2位轴箱温度
                    clb_msg->zx_temp[INDEX_2] = qzq_data->temp_diag[TEMP_PT2].temp_data;
                    if (app_get_head_tail())
                    {
                        //X车3位轴箱温度
                        clb_msg->zx_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                        //X车4位轴箱温度
                        clb_msg->zx_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT4].temp_data;
                    }
                    else {
                        //X车1位齿轮箱箱温度
                        clb_msg->clx_temp[INDEX_1] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                        //X车1位电机温度
                        clb_msg->dj_temp[INDEX_1] = qzq_data->temp_diag[TEMP_PT4].temp_data;
                    }
                }
                else if(i == QZQ_2)
                {
                    if(app_get_head_tail())//主机车厢
                    {
                        //X车5位轴箱温度
                        clb_msg->zx_temp[INDEX_5] = qzq_data->temp_diag[TEMP_PT1].temp_data;
                        //X车6位轴箱温度
                        clb_msg->zx_temp[INDEX_6] = qzq_data->temp_diag[TEMP_PT2].temp_data;
                        //根据编组分类
                        if(paras->group_num == GROUP_NUM_6)//6编组方式
                        {
                            //X车7位轴箱温度
                            clb_msg->zx_temp[INDEX_7] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                            //X车8位轴箱温度
                            clb_msg->zx_temp[INDEX_8] = qzq_data->temp_diag[TEMP_PT4].temp_data;
                        }
                        else if(paras->group_num == GROUP_NUM_4)//4编组方式
                        {
                            //X车3位齿轮箱温度
                            clb_msg->clx_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                            //X车3位电机温度
                            clb_msg->dj_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT4].temp_data;
                        }
                    }
                    else {
                        //X车3位轴箱温度
                        clb_msg->zx_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT1].temp_data;
                        //X车4位轴箱温度
                        clb_msg->zx_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT2].temp_data;
                        //X车2位齿轮箱温度
                        clb_msg->clx_temp[INDEX_2] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                        //X车2位电机温度
                        clb_msg->dj_temp[INDEX_2] = qzq_data->temp_diag[TEMP_PT4].temp_data;
                    }
                }
                else if(i == QZQ_3)
                {
                    //主机只有4编组才有前置器3
                    if(app_get_head_tail() && paras->group_num==GROUP_NUM_4)//主机车厢
                    {
                        //X车7位轴箱温度
                        clb_msg->zx_temp[INDEX_7] = qzq_data->temp_diag[TEMP_PT1].temp_data;
                        //X车8位轴箱温度
                        clb_msg->zx_temp[INDEX_8] = qzq_data->temp_diag[TEMP_PT2].temp_data;
                        //X车4位齿轮箱温度
                        clb_msg->clx_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                        //X车4位电机温度
                        clb_msg->dj_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT4].temp_data;
                    }
                    else if (!app_get_head_tail())//非主机
                    {
                        //X车5位轴箱温度
                        clb_msg->zx_temp[INDEX_5] = qzq_data->temp_diag[TEMP_PT1].temp_data;
                        //X车6位轴箱温度
                        clb_msg->zx_temp[INDEX_6] = qzq_data->temp_diag[TEMP_PT2].temp_data;
                        //X车3位齿轮箱温度
                        clb_msg->clx_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                        //X车3位电机温度
                        clb_msg->dj_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT4].temp_data;
                    }
                }
                else if(i == QZQ_4)//只有从机有4号前置器
                {
                    if(!app_get_head_tail())
                    {
                        //X车7位轴箱温度
                        clb_msg->zx_temp[INDEX_7] = qzq_data->temp_diag[TEMP_PT1].temp_data;
                        //X车8位轴箱温度
                        clb_msg->zx_temp[INDEX_8] = qzq_data->temp_diag[TEMP_PT2].temp_data;
                        //X车4位齿轮箱温度
                        clb_msg->clx_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                        //X车4位电机温度
                        clb_msg->dj_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT4].temp_data;
                    }
                }
                //环境温度
                clb_msg->env_temp[i] = qzq_data->temp_diag[TEMP_ENV].temp_data;
            }
        }

        // 算法屏蔽
        for(uint8_t j = 0; j < 8; j++) {
            clb_msg->zxzc_alarm[j].byte &= 0xc0;
        }
        for(uint8_t j = 0; j < 4; j++) {
            clb_msg->dj_alarm[j].byte &= 0xc0;
            clb_msg->clx_alarm[j].byte &= 0xc0;
        }

        clb_info->check = crc8_maxim(clb_info->data, hton16(*(uint16_t *)clb_info->size));

        app_mcast_send_data(env.clb_data, sizeof(clb_port_info_t));
//        printf("app_mcast_send to jlb:\n");
//        for(int i=0; i<sizeof(clb_port_msg_t); i++)
//        {
//            if (i!=0 && i%30==0) printf("\n");
//            if (i!=0 && i%10==0)  printf("  ");
//            printf(" %x",env.clb_data[i+5]);
//        }
//        printf("\n");
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_period_thread_entry(void * paras)
{
    uint16_t count = 0;
    while(1)
    {
        env.life_cnt++;
        count++;
        app_rtc_update(count);
        app_period_clb_port_info(count);
        app_txb_info_update(count);
        rt_thread_mdelay(100);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_period_thread(void)
{
    env.thread = rt_thread_create("app_period", app_period_thread_entry, \
            RT_NULL, APP_PERIOD_STACK, APP_PERIOD_THREAD_PRO, 20);
    if(env.thread != RT_NULL) {
        rt_thread_startup(env.thread);
    }
    else {
        app_log_msg(LOG_LVL_ERROR, true, "app period thread create failed");
    }
}
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_period_init(void)
{
    env.life_cnt = 0;
    env.clb_info.wheel = 805;
    env.clb_info.speed = 0;
    memset(env.clb_data, 0, sizeof(clb_port_info_t));
}






