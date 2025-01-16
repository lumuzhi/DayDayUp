/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_qzq.h"
#include "app_paras.h"
#include "app_mcast.h"
#include "app_rtc.h"
#include "head_protocol.h"
#include "app_period.h"
#include "app_log.h"
#include "ptu_protocol.h"
#include "clb_protocol.h"

/* macro */
#define APP_QZQ_VIB_MIN       0.3
#define APP_QZQ_VIB_MAX       1.1
#define APP_QZQ_TEMP_WARN     80
#define APP_QZQ_TEMP_ALARM    90
#define APP_QZQ_TEMP_WIN      10   //该值要大于2，诊断策略的计数
#define MVB_DEFAULT_TEMP      0x81   //默认无效值

/* type declaration */
typedef struct
{
    int8_t temp_data[APP_QZQ_TEMP_WIN];
}temp_win_t;  //温度窗口
typedef struct
{
    temp_win_t temp_win[TEMP_MAX];
}temp_win_info_t;
typedef struct
{
    int8_t temp_data[TEMP_OSR * 2];
}temp_slip_t;  //温度滑移
typedef struct
{
    temp_slip_t temp_slip[TEMP_MAX];
}temp_slip_info_t;
typedef struct
{
    temp_win_info_t  win_info[QZQ_MAX];
    temp_slip_info_t slip_orig_info[QZQ_MAX];  //原始
    temp_slip_info_t slip_sort_info[QZQ_MAX];  //排序
    uint8_t          *temp_send;

    int8_t          pre_tmp[QZQ_MAX][TEMP_MAX];
}app_qzq_local_t;

/* variable declaration */
app_qzq_env_t app_qzq_env;
#define env app_qzq_env
app_qzq_local_t app_qzq_local;
#define local app_qzq_local

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note   插入排序
**/
void app_qzq_data_sort(int8_t *a, int8_t n)
{
    for(int8_t i = 0; i < n - 1; i++)
    {
        int end = i, temp = a[end + 1];

        while(end >= 0)
        {
            if(temp < a[end])
            {
                a[end + 1] = a[end];
                end--;
            }
            else {
                break;
            }
        }
        a[end + 1] = temp;
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_qzq_data_deal(uint8_t *data, uint16_t size)
{
    if((data == RT_NULL) || (size != sizeof(qzq_info_t)) || \
            (*(uint16_t *)data != HEAD_CAN_CLB_QZQ_INFO))
    {
        return ;
    }
    qzq_info_t *qzq_info = (qzq_info_t *)data;
    qzq_msg_t *qzq_msg = (qzq_msg_t *)qzq_info->data;
    if(qzq_info->check == crc8_maxim(qzq_info->data, hton16(*(uint16_t *)qzq_info->size)))
    {
        if((qzq_msg->id > 0) && (qzq_msg->id <= QZQ_MAX))
        {
            env.can_qzq_info[qzq_msg->id - 1].recv_count += 1;
            rt_base_t level = rt_hw_interrupt_disable();
            rt_memcpy(env.can_qzq_info[qzq_msg->id - 1].data, qzq_msg, sizeof(qzq_msg_t));
            rt_hw_interrupt_enable(level);
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_qzq_com_state_check(uint16_t count)
{
    char log_data[256] = { 0 };
    sys_paras_t *paras = app_paras_get();
    static uint8_t trust_cnt[QZQ_MAX] = { 0 };
    static uint16_t qzq_count[QZQ_MAX] = { 0 }, reset_cnts[QZQ_MAX] = { 0 };
    if(!(count % 2))  //200ms
    {
        for(uint8_t i = QZQ_1; i < QZQ_MAX; i++)
        {
            if(qzq_count[i] != env.can_qzq_info[i].recv_count)
            {
                if(trust_cnt[i] >= 1)  //前1包数据不信任
                {
                    if(env.qzq_data[i].com_state != 1)
                    {
                        rt_memset(log_data, 0, sizeof(log_data));
                        sprintf(log_data, "train %d can recv qzq %d  data get right", paras->train_num, i + 1);
                        app_log_msg(LOG_LVL_INFO, true, log_data);
                    }
                    env.qzq_data[i].com_state = 1;
                }
                else {
                    trust_cnt[i]++;
                }
                qzq_count[i] = env.can_qzq_info[i].recv_count;
                reset_cnts[i] = 0;
            }
            else
            {
                reset_cnts[i]++;
                if(reset_cnts[i] == 300)  //在15秒内处理板板没有收到前置器信息，则判定前置器信息数据异常
                {
                    trust_cnt[i] = 0;
                    env.qzq_data[i].com_state = 0;
                    rt_memset(log_data, 0, sizeof(log_data));
                    sprintf(log_data, "train %d can recv qzq %d data failed", paras->train_num, i + 1);
                    app_log_msg(LOG_LVL_INFO, true, log_data);
                    //在前置器通信中断的情况下，将传感器振动状态设置为1正常，已经报了前置器故障
                    for(uint8_t j = ADG1609_CH_1; j < ADG1609_CH_MAX; j++)
                    {
                        if(env.qzq_data[i].vib_state[j] != 1)
                        {
                            rt_memset(log_data, 0, sizeof(log_data));
                            sprintf(log_data, "qzq %d com state failed set vibrate ch %d state 0", i + 1, j);
                            app_log_msg(LOG_LVL_INFO, true, log_data);
                        }
                        env.qzq_data[i].vib_state[j] = 1;
                        if(env.qzq_data[i].cgq_state[j] != 1)
                        {
                            rt_memset(log_data, 0, sizeof(log_data));
                            sprintf(log_data, "qzq %d com state failed set cgq ch %d state 0", i + 1, j);
                            app_log_msg(LOG_LVL_INFO, true, log_data);
                        }
                        env.qzq_data[i].cgq_state[j] = 1;
                    }
                    /* 在前置器通信中断的情况下，因为已经报了前置器故障，所以将温度状态设置为正常，温度设置为-127  */
                    for(uint8_t j = TEMP_ENV; j < TEMP_MAX; j++)
                    {
                        if(env.qzq_data[i].temp_diag[j].temp_state != TEMP_NORMAL)
                        {
                            rt_memset(log_data, 0, sizeof(log_data));
                            sprintf(log_data, "qzq %d com state failed set ch %d temp state TEMP_NORMAL", i + 1, j);
                            app_log_msg(LOG_LVL_INFO, true, log_data);
                        }
                        env.qzq_data[i].temp_diag[j].temp_state = TEMP_NORMAL;
                        env.qzq_data[i].temp_diag[j].temp_data = MVB_DEFAULT_TEMP;
                    }
                    //通信中断，传感器振动值无法判断，设置诊断结果为默认值
                    app_period_clb_diag_default_qzq(app_period_env.clb_data, sizeof(clb_port_info_t), i);
                }
            }
        }
    }
    if(!(count % 100))  //每10秒记录一次接收到的消息计数
    {
        sprintf(log_data, "can recv qzq data count %d-%d-%d-%d", qzq_count[0], \
                qzq_count[1], qzq_count[2], qzq_count[3]);
        app_log_msg(LOG_LVL_INFO, true, log_data);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_qzq_power_record(uint16_t count)
{
    char log_data[256] = { 0 };
    if(!(count % 50))  //每5秒记录一次前置器电源信息
    {
        for(uint8_t i = QZQ_1; i < QZQ_MAX; i++)
        {
            if(env.qzq_data[i].com_state)
            {
                qzq_msg_t *qzq_msg = (qzq_msg_t *)env.can_qzq_info[i].data;
                uint16_t power_12 = DATA16(qzq_msg->power_12.adc_data[0], qzq_msg->power_12.adc_data[1]);
                uint16_t power_5 = DATA16(qzq_msg->power_5.adc_data[0], qzq_msg->power_5.adc_data[1]);
                rt_memset(log_data, 0, sizeof(log_data));
                sprintf(log_data, "can recv qzq %d power msg %d.%d-%d.%d", i + 1, power_12 / 100, \
                        power_12 % 100, power_5 / 100, power_5 % 100);
                app_log_msg(LOG_LVL_INFO, true, log_data);
            }
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_qzq_vib_state_check(uint16_t count)
{
    static uint8_t vib_error_cnt[QZQ_MAX][ADG1609_CH_MAX] = {0};
    char log_data[256] = { 0 };
    float vib_val = 0;
    if(!(count % 2))  //200ms
    {
        for(uint8_t i = QZQ_1; i < QZQ_MAX; i++)
        {
            if(env.qzq_data[i].com_state)
            {
                qzq_msg_t *qzq_msg = (qzq_msg_t *)env.can_qzq_info[i].data;
                for(uint8_t j = ADG1609_CH_1; j < ADG1609_CH_MAX; j++)
                {
                    //因为在前置器中采集的振动值已经进行了求均值等操作，这里直接使用
                    vib_val = (float)DATA16(qzq_msg->vibrate[j].adc_data[0], qzq_msg->vibrate[j].adc_data[1]) / 100;
                    if((vib_val < APP_QZQ_VIB_MIN) || (vib_val > APP_QZQ_VIB_MAX))
                    {
                        // 偏置电压连续20次不正常才上报
                        vib_error_cnt[i][j]++;
                        if(!(vib_error_cnt[i][j] % 20)) {
                            if(env.qzq_data[i].vib_state[j] != 0)
                            {
                                rt_memset(log_data, 0, sizeof(log_data));
                                sprintf(log_data, "qzq %d vibrate ch %d data failed", i + 1, j);
                                app_log_msg(LOG_LVL_INFO, true, log_data);
                            }
                            env.qzq_data[i].vib_state[j] = 0;   // 振动状态
                        }
                    }
                    else
                    {
                        vib_error_cnt[i][j] = 0;
                        if(env.qzq_data[i].vib_state[j] != 1)
                        {
                            rt_memset(log_data, 0, sizeof(log_data));
                            sprintf(log_data, "qzq %d vibrate ch %d data get right", i + 1, j);
                            app_log_msg(LOG_LVL_INFO, true, log_data);
                        }
                        env.qzq_data[i].vib_state[j] = 1;
                    }
//                    rt_kprintf("qzq1 ch%d vib_state: %d\n", j, env.qzq_data[i].vib_state[j]);
                }
            }
            else {
                //在通信状态判断的函数中，已经处理了通信中断的情况，因为判断通信中断后能够及时将报警信息更新
            }
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note   该函数很重要涉及到稳定报警相关，修改一定要注意
**/
static void app_qzq_temp_deal(uint16_t count)
{
    char log_data[256] = { 0 };
    sys_paras_t *paras = app_paras_get();
    uint8_t temp_warn_value = APP_QZQ_TEMP_WARN;
    uint8_t temp_alarm_value = APP_QZQ_TEMP_ALARM;
    if(!(count % 10))  //1s
    {
        for(uint8_t i = QZQ_1; i < QZQ_MAX; i++)
        {
            if(env.qzq_data[i].com_state)
            {
                for(uint8_t j = 0; j < TEMP_MAX; j++)
                {
                    qzq_msg_t *qzq_msg = (qzq_msg_t *)env.can_qzq_info[i].data;
                    rt_base_t level = rt_hw_interrupt_disable();
                    //前置器获取到的温度，拷贝到温度处理的原始温度
                    rt_memcpy(local.slip_orig_info[i].temp_slip[j].temp_data, qzq_msg->temp[j].temp_data, TEMP_OSR);

                    //2秒的原始温度，拷贝到排序的温度空间
                    rt_memcpy(local.slip_sort_info[i].temp_slip[j].temp_data, local.slip_orig_info[i].temp_slip[j].temp_data, TEMP_OSR * 2);
                    //原始温度滑移1秒
                    rt_memcpy(&local.slip_orig_info[i].temp_slip[j].temp_data[TEMP_OSR], local.slip_orig_info[i].temp_slip[j].temp_data, TEMP_OSR);

                    rt_hw_interrupt_enable(level);

                    if(qzq_msg->temp[j].state == TEMP_INFO_OK)
                    {
                        app_qzq_data_sort(local.slip_sort_info[i].temp_slip[j].temp_data, TEMP_OSR * 2);

                        //取中间往右5个，其它不要
                        for(int16_t k = 0, mean_val = 0; k < 5; k++)
                        {
                            mean_val += local.slip_sort_info[i].temp_slip[j].temp_data[10 + k];
                            if(k == 4)
                            {
                                int8_t temp_val = (int8_t)(mean_val / 5);
                                if((temp_val < -55) || (temp_val > 125))  //DS18B20的温度范围是-55 - +125，超出该范围数据肯定有问题
                                {
                                    rt_memset(log_data, 0, sizeof(log_data));
                                    sprintf(log_data, "qzq %d temp ch %d val %d data out of range", i + 1, j, temp_val);
                                    app_log_msg(LOG_LVL_INFO, true, log_data);
                                    //温度数据异常，不更新数据，沿用上一次的数据
                                    local.win_info[i].temp_win[j].temp_data[0] = local.pre_tmp[i][j];
                                }
                                else
                                {
                                    if (temp_val == 85) {
                                        //85是18b20的默认温度值，有可能是传感器出现问题是反馈的温度，在此判断
                                        if ((local.win_info[i].temp_win[j].temp_data[0] - 85) < -10
                                                || (local.win_info[i].temp_win[j].temp_data[0] - 85) > 10) {
                                            local.win_info[i].temp_win[j].temp_data[0] =  MVB_DEFAULT_TEMP;
                                        }
                                        else {
                                            local.win_info[i].temp_win[j].temp_data[0] = temp_val;
                                            local.pre_tmp[i][j] = temp_val;
                                        }
                                    }
                                    else {
                                        //温度数据正常，更新数据
                                        local.win_info[i].temp_win[j].temp_data[0] = temp_val;
                                        local.pre_tmp[i][j] = temp_val;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        //传感器没接或者是其他原因，温度数据设置为-128
                        local.win_info[i].temp_win[j].temp_data[0] = MVB_DEFAULT_TEMP;
                    }
                }
                //温度诊断的窗口大小内，所有值都大于预警报警后，得到温度的诊断结果，温度值使用最后一次得到的结果
                for(uint8_t j = 0; j < TEMP_MAX; j++)
                {
                    int8_t alarm_temp = 0, warn_temp = 0, normal_temp = 0;
                    for(uint8_t k = 0, warn_cnts = 0, alarm_cnts = 0; k < APP_QZQ_TEMP_WIN; k++)
                    {
                        //最终的温度先设置初值
                        if(k == 0)
                        {
                            alarm_temp = local.win_info[i].temp_win[j].temp_data[k];
                            warn_temp = local.win_info[i].temp_win[j].temp_data[k];
                            normal_temp = local.win_info[i].temp_win[j].temp_data[k];
                        }
                        //报警温度取报警条件下的最小值
                        // 济南8 电机 齿轮箱温度判断更改为 95， 105
                        if(paras->train_num > 1 && paras->train_num < 6) {  // 从机
                            if((j == TEMP_PT3) || (j == TEMP_PT4)) {
                                temp_warn_value = 95;
                                temp_alarm_value = 105;
                            }
                            else {
                                temp_warn_value = 80;
                                temp_alarm_value = 90;
                            }
                        }
                        else {
                            temp_warn_value = 80;
                            temp_alarm_value = 90;
                        }
                        if(local.win_info[i].temp_win[j].temp_data[k] >= temp_alarm_value)
                        {
                            alarm_temp = MIN(local.win_info[i].temp_win[j].temp_data[k], alarm_temp);
                            alarm_cnts++;
                            warn_cnts++;
                        }
                        else if(local.win_info[i].temp_win[j].temp_data[k] >= temp_warn_value)
                        {
                            warn_temp = MIN(local.win_info[i].temp_win[j].temp_data[k], warn_temp);
                            warn_cnts++;
                        }
                        else {
                            normal_temp = MIN(local.win_info[i].temp_win[j].temp_data[k], normal_temp);
                        }

                        //最后一次得到诊断结果
                        if(k == (APP_QZQ_TEMP_WIN -1))
                        {
                            if(alarm_cnts == APP_QZQ_TEMP_WIN)  //窗口大小内全部满足报警
                            {
                                if(env.qzq_data[i].temp_diag[j].temp_state != TEMP_ALARM)
                                {
                                    rt_memset(log_data, 0, sizeof(log_data));
                                    sprintf(log_data, "qzq %d temp ch %d val %d state TEMP_ALARM", i + 1, j, alarm_temp);
                                    app_log_msg(LOG_LVL_INFO, true, log_data);
                                }
                                env.qzq_data[i].temp_diag[j].temp_state = TEMP_ALARM;
                                env.qzq_data[i].temp_diag[j].temp_data = alarm_temp;
                            }
                            else if(warn_cnts == APP_QZQ_TEMP_WIN)  //窗口大小内全部满足预警
                            {
                                if(env.qzq_data[i].temp_diag[j].temp_state != TEMP_WARN)
                                {
                                    rt_memset(log_data, 0, sizeof(log_data));
                                    sprintf(log_data, "qzq %d temp ch %d val %d state TEMP_WARN", i + 1, j, warn_temp);
                                    app_log_msg(LOG_LVL_INFO, true, log_data);
                                }
                                env.qzq_data[i].temp_diag[j].temp_state = TEMP_WARN;
                                env.qzq_data[i].temp_diag[j].temp_data = (warn_temp >= temp_alarm_value) ? (temp_alarm_value - 1) : warn_temp;
                            }
                            else  //窗口大小内全部温度正常
                            {
                                if(env.qzq_data[i].temp_diag[j].temp_state != TEMP_NORMAL)
                                {
                                    rt_memset(log_data, 0, sizeof(log_data));
                                    sprintf(log_data, "qzq %d temp ch %d val %d state TEMP_NORMAL", i + 1, j, normal_temp);
                                    app_log_msg(LOG_LVL_INFO, true, log_data);
                                }
                                env.qzq_data[i].temp_diag[j].temp_state = TEMP_NORMAL;
                                //因为是要APP_QZQ_TEMP_WIN次都满足，有可能没到报警的次数，实际温度已经大于80，此时上传的温度为80-1
                                env.qzq_data[i].temp_diag[j].temp_data = (normal_temp >= temp_warn_value) ? (temp_warn_value - 1) : normal_temp;
                            }
                        }
                    }
                    if(APP_QZQ_TEMP_WIN > 2)
                    {
                        //移动窗口
                        for(uint8_t z = APP_QZQ_TEMP_WIN - 2; z >= 0; z--)
                        {
                            local.win_info[i].temp_win[j].temp_data[z + 1] = local.win_info[i].temp_win[j].temp_data[z];
                            if(z == 0) {
                                break;
                            }
                        }
                    }
                }
                rt_kprintf("QZQ_%d  temp-state: ENV(%d)  %d  %d  %d  %d  vib:%d-%d-%d-%d\n",i+1, \
                        env.qzq_data[i].temp_diag[0].temp_data, env.qzq_data[i].temp_diag[1].temp_data,\
                        env.qzq_data[i].temp_diag[2].temp_data, env.qzq_data[i].temp_diag[3].temp_data,\
                        env.qzq_data[i].temp_diag[4].temp_data,
                        env.qzq_data[i].vib_state[0],env.qzq_data[i].vib_state[1],
                        env.qzq_data[i].vib_state[2],env.qzq_data[i].vib_state[3]);
            }
            else {
                //在通信状态判断的函数中，已经处理了通信中断的情况，因为判断通信中断后能够及时将报警信息更新
            }
        }
        //数据发送，在记录板进行保存
//        if(local.temp_send != RT_NULL)
//        {
//            static rt_uint16_t pack_cnt = 0;
//            clb_temp_save_info_t *save_info = (clb_temp_save_info_t *)local.temp_send;
//            //传输头
//            *(uint16_t *)save_info->head = HEAD_MCAST_CLB_TEMP_RAW_DATA_SAVE;
//            save_info->size[0] = DATA16_H(sizeof(clb_temp_save_info_t) - 5);
//            save_info->size[1] = DATA16_L(sizeof(clb_temp_save_info_t) - 5);
//            local_time_t time;
//            app_rtc_get(&time);
//            //存储协议头
//            clb_save_head_t *save_head = (clb_save_head_t *)save_info->data;
//            save_head->head_h = 0x66;
//            save_head->head_l = 0xbb;
//            uint16_t save_len = sizeof(clb_save_head_t) + QZQ_MAX * sizeof(clb_save_temp_t);
//            save_head->data_len[0] = DATA16_H(save_len);
//            save_head->data_len[1] = DATA16_L(save_len);
//            save_head->type = QZQ_TEMP;
//            save_head->year = time.year - 2000;
//            save_head->month = time.mon;
//            save_head->day = time.day;
//            save_head->hour = time.hour;
//            save_head->min = time.min;
//            save_head->sec = time.sec;
//            save_head->pack_count_h = DATA16_H(pack_cnt);
//            save_head->pack_count_l = DATA16_L(pack_cnt);
//            pack_cnt++;
//            if((paras->train_num == 1) || (paras->train_num == paras->group_num)) {
//                save_head->host_slave_flag = 1;
//            }
//            else {
//                save_head->host_slave_flag = 0;
//            }
//            save_head->train_id = paras->train_num;
//            //前置器
//            for(uint8_t i = 0; i < QZQ_MAX; i++)
//            {
//                clb_save_temp_t *save_temp = (clb_save_temp_t *)&save_info->data[sizeof(clb_save_head_t) + i * sizeof(clb_save_temp_t)];
//                qzq_msg_t *qzq_msg = (qzq_msg_t *)env.can_qzq_info[i].data;
//                for(uint8_t j = 0; j < TEMP_MAX; j++)
//                {
//                    qzq_temp_t *qzq_temp = (qzq_temp_t *)&qzq_msg->temp[j];
//                    switch(j)
//                    {
//                        case 0:   //TEMP_ENV
//                            //因为存储协议温度数据是uint16类型，前置器上传的温度是int8_t，温度范围是-55 - +125，int8_t完全够用，为了兼容
//                            for(uint8_t k = 0; k < TEMP_OSR; k++)
//                            {
//                                save_temp->env_data[k * 2] = 0;
//                                save_temp->env_data[k * 2 + 1] = (qzq_temp->state == TEMP_INFO_OK) ? qzq_temp->temp_data[k] : MVB_DEFAULT_TEMP;
//                            }
//                            break;
//                        case 1:   //TEMP_PT1
//                            for(uint8_t k = 0; k < TEMP_OSR; k++)
//                            {
//                                save_temp->pt1_data[k * 2] = 0;
//                                save_temp->pt1_data[k * 2 + 1] = (qzq_temp->state == TEMP_INFO_OK) ? qzq_temp->temp_data[k] : MVB_DEFAULT_TEMP;
//                            }
//                            break;
//                        case 2:   //TEMP_PT2
//                            for(uint8_t k = 0; k < TEMP_OSR; k++)
//                            {
//                                save_temp->pt2_data[k * 2] = 0;
//                                save_temp->pt2_data[k * 2 + 1] = (qzq_temp->state == TEMP_INFO_OK) ? qzq_temp->temp_data[k] : MVB_DEFAULT_TEMP;
//                            }
//                            break;
//                        case 3:   //TEMP_PT3
//                            for(uint8_t k = 0; k < TEMP_OSR; k++)
//                            {
//                                save_temp->pt3_data[k * 2] = 0;
//                                save_temp->pt3_data[k * 2 + 1] = (qzq_temp->state == TEMP_INFO_OK) ? qzq_temp->temp_data[k] : MVB_DEFAULT_TEMP;
//                            }
//                            break;
//                        case 4:   //TEMP_PT4
//                            for(uint8_t k = 0; k < TEMP_OSR; k++)
//                            {
//                                save_temp->pt4_data[k * 2] = 0;
//                                save_temp->pt4_data[k * 2 + 1] = (qzq_temp->state == TEMP_INFO_OK) ? qzq_temp->temp_data[k] : MVB_DEFAULT_TEMP;
//                            }
//                            break;
//                        default:
//                            break;
//                    }
//                }
//            }
//            save_info->check = crc8_maxim(save_info->data, hton16(*(uint16_t *)save_info->size));
//            app_mcast_send_data(local.temp_send, sizeof(clb_temp_save_info_t));
//        }
    }
}
/**
 * @brief  传感器状态检测函数
 * @param
 * @return
 * @note   检测并生成向列控上传的传感器状态，温度与振动只要有一个正常，就按照正常上报
**/
static void app_qzq_cgq_state_check(uint16_t count)
{
    char log_data[256] = { 0 };
    static uint8_t cgq_state[QZQ_MAX][ADG1609_CH_MAX] = { 0 };
    if(!(count % 2))  //200ms
    {
        for(uint8_t i = QZQ_1; i < QZQ_MAX; i++)
        {
            if(env.qzq_data[i].com_state)
            {
                qzq_msg_t *qzq_msg = (qzq_msg_t *)env.can_qzq_info[i].data;
                for(uint8_t j = ADG1609_CH_1, k = TEMP_PT1; j < ADG1609_CH_MAX; j++, k++)
                {
                    if (env.qzq_data[i].vib_state[j] || (TEMP_INFO_OK == qzq_msg->temp[k].state))
                    {
                        if(!cgq_state[i][j])  //日志记录温度和振动状态
                        {
                            cgq_state[i][j] = 1;
                            rt_memset(log_data, 0, sizeof(log_data));
                            sprintf(log_data, "qzq %d cgq ch %d data state get right：temp：%d, vib:%d", i + 1, j, \
                                    qzq_msg->temp[k].state, env.qzq_data[i].vib_state[j]);
                            app_log_msg(LOG_LVL_ERROR, true, log_data);
                        }
                        env.qzq_data[i].cgq_state[j] = 1; //温度和振动，只要有一个状态OK，就反馈正常
                    }
                    else
					{
                        env.qzq_data[i].cgq_state[j] = 0; //温度和振动状态都故障，反馈故障
                        if(cgq_state[i][j])  //日志记录温度和振动状态
                        {
                            cgq_state[i][j] = 0;
                            rt_memset(log_data, 0, sizeof(log_data));
                            sprintf(log_data, "qzq %d cgq ch %d data state failed：temp：%d, vib:%d", i + 1, j, \
                                    qzq_msg->temp[k].state, env.qzq_data[i].vib_state[j]);
                            app_log_msg(LOG_LVL_ERROR, true, log_data);
                        }
                    }
                }
            }
            else {
                //在通信状态判断的函数中，已经处理了通信中断的情况，因为判断通信中断后能够及时将报警信息更新
            }
        }
    }
}
/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_qzq_thread_entry(void * paras)
{
    uint16_t count = 0;
    while(1)
    {
        env.life_cnt++;
        count++;
        app_qzq_com_state_check(count);
        app_qzq_power_record(count);
        app_qzq_vib_state_check(count);
        app_qzq_temp_deal(count);
        app_qzq_cgq_state_check(count);
        rt_thread_delay(100);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_qzq_thread(void)
{
    env.thread = rt_thread_create("app_qzq", app_qzq_thread_entry, \
            RT_NULL, APP_QZQ_STACK, APP_QZQ_THREAD_PRO, 20);
    if(env.thread != RT_NULL) {
        rt_thread_startup(env.thread);
    }
    else {
        app_log_msg(LOG_LVL_ERROR, true, "app qzq thread create failed");
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_qzq_init(void)
{
    env.life_cnt = 0;
    for(uint8_t i = QZQ_1; i < QZQ_MAX; i++)
    {
        rt_memset(&env.can_qzq_info[i], 0, sizeof(can_qzq_info_t));
        rt_memset(&env.qzq_data[i], 0, sizeof(qzq_data_t));
        rt_memset(&local.win_info[i], 0, sizeof(temp_win_info_t));
        rt_memset(&local.slip_orig_info[i], 0, sizeof(temp_slip_info_t));
        rt_memset(&local.slip_sort_info[i], 0, sizeof(temp_slip_info_t));

        rt_memset(&env.qzq_data[i].vib_state, 1, ADG1609_CH_MAX);
        rt_memset(&env.qzq_data[i].cgq_state, 1, ADG1609_CH_MAX);
        for(uint8_t j = TEMP_PT1; j < TEMP_MAX; j++) {
            env.qzq_data[i].temp_diag[j].temp_data = MVB_DEFAULT_TEMP;
        }
    }
    app_period_clb_diag_default(app_period_env.clb_data, sizeof(clb_port_info_t));
    local.temp_send = rt_malloc(sizeof(clb_temp_save_info_t));
    if(local.temp_send == RT_NULL) {
        app_log_msg(LOG_LVL_ERROR, true, "app qzq temp send maoolc failed");
    }
    else {
        rt_memset(local.temp_send, 0, sizeof(clb_temp_save_info_t));
    }

    for (int i = 0; i < QZQ_MAX; i++) {
        for (int j = 0; j < TEMP_MAX; j++) {
            local.pre_tmp[i][j] = MVB_DEFAULT_TEMP;
        }
    }
}

