/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_diag.h"
#include "app_paras.h"
#include "app_period.h"
#include "app_mcast.h"
#include "app_log.h"
#include "app_hw_timer.h"
#include "ptu_protocol.h"
#include "app_qzq.h"
#include "app_can.h"
#include "test_data.h"
#include "app_qzq_can.h"
#include "app_iwdg.h"

/* macro */
#define DIAG_STRATEGY_COUNT       5  //该值要大于2，诊断策略的计数
//#define USE_DERAILMENT

#define DIAG_CHANNEL_NUM          8        //脱轨采集的轴箱数
#define DIAG_RATE                 4096
#define DIAG_STACK_LEN            10
#define DIAG_THRESHOLD_PROGNOSIS  17
#define DIAG_THRESHOLD_PREWARNING 22
#define DIAG_THRESHOLD_WARNING    27

/* type declaration */
typedef struct
{
    uint8_t       *raw_data_save;
    uint8_t       *tz_data_save;
}app_diag_local_t;

/* variable declaration */
app_diag_local_t app_diag_local;
#define local app_diag_local
app_diag_env_t app_diag_env;
#define env app_diag_env

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note   原始数据预处理
**/
static void app_diag_raw_preproccess(diag_data_t **diag_data)
{
    for(uint8_t i = 0; i < AD7606_MAX; i++)
    {
        if(i == AD7606_V1) //冲击
        {
            if(app_diag_env.diag_data[i]->agd1608_ch % 2) {
                app_diag_env.diag_data[i]->data_type = MOTOR_IMPACT; //电机
            }
            else {
                app_diag_env.diag_data[i]->data_type = GEAR_IMPACT;  //齿轮
            }
        }
        else if(i == AD7606_V2) //振动
        {
            if(app_diag_env.diag_data[i]->agd1608_ch % 2) {
                app_diag_env.diag_data[i]->data_type = MOTOR_VIBRATE; //电机，未使用
            }
            else {
                app_diag_env.diag_data[i]->data_type = GEAR_VIBRATE; //齿轮
            }
        }
        else if(i == AD7606_V3) //冲击
        {
            app_diag_env.diag_data[i]->data_type = AXLE_IMPACT;  //轴箱
        }
        else if(i == AD7606_V4) //振动
        {
            app_diag_env.diag_data[i]->data_type = AXLE_VIBRATE; //轴箱，多边形使用，在脱轨板，处理板不使用
        }
        else {
            app_diag_env.diag_data[i]->diag_state = false;
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note   原始数据保存
**/
static void app_diag_raw_to_save(diag_data_t **diag_data)
{
    sys_paras_t *paras = app_paras_get();
    static rt_uint16_t pack_cnt = 0;
    rt_kprintf("app_diag_raw_to_save!!!!!!!!! \n");
    if(env.dev_state != DEV_INIT_SUCCESS) {
        return;
    }
    clb_raw_save_info_t *save_info = (clb_raw_save_info_t *)local.raw_data_save;
    //传输头
    *(uint16_t *)save_info->head = HEAD_MCAST_CLB_VIB_RAW_DATA_SAVE;
    save_info->size[0] = DATA16_H(sizeof(clb_raw_save_info_t) - 5);
    save_info->size[1] = DATA16_L(sizeof(clb_raw_save_info_t) - 5);
    local_time_t time;
    app_rtc_get(&time);
    //存储协议头
    clb_save_head_t *save_head = (clb_save_head_t *)save_info->data;
    save_head->head_h = 0x66;
    save_head->head_l = 0xbb;
    uint16_t save_len = sizeof(clb_save_head_t) + AD7606_SF * sizeof(uint16_t);
    save_head->data_len[0] = DATA16_H(save_len);
    save_head->data_len[1] = DATA16_L(save_len);
    save_head->year = time.year - 2000;
    save_head->month = time.mon;
    save_head->day = time.day;
    save_head->hour = time.hour;
    save_head->min = time.min;
    save_head->sec = time.sec;
    save_head->pack_count_h = DATA16_H(pack_cnt);
    save_head->pack_count_l = DATA16_L(pack_cnt);
    pack_cnt++;
    if((paras->train_num == 1) || (paras->train_num == paras->group_num)) {
        save_head->host_slave_flag = 1;
    }
    else {
        save_head->host_slave_flag = 0;
    }
    save_head->train_id = paras->train_num;
    for(uint8_t i = 0; i < AD7606_MAX; i++)
    {
        if(i > AD7606_V4) {
            break;
        }

        save_head->type = diag_data[i]->data_type;
        save_head->channel = diag_data[i]->agd1608_ch + 1;  //解包软件是从1开始
        save_head->current_id_h = DATA16_H(diag_data[i]->now_id);
        save_head->current_id_l = DATA16_L(diag_data[i]->now_id);
        save_head->next_id_h = DATA16_H(diag_data[i]->next_id);
        save_head->next_id_l = DATA16_L(diag_data[i]->next_id);
        save_head->speed_h = DATA16_H(diag_data[i]->speed);
        save_head->speed_l = DATA16_L(diag_data[i]->speed);
        save_head->wheel_diameter_h = DATA16_H(app_period_env.clb_info.wheel);
        save_head->wheel_diameter_l = DATA16_L(app_period_env.clb_info.wheel);
        for(uint8_t j = 0; j < (AD7606_SF * sizeof(uint16_t)) / 1024; j++)  //分包发送
        {
            clb_raw_save_pkg_t *raw_save_pkg = (clb_raw_save_pkg_t *)&save_info->data[sizeof(clb_save_head_t)];
            raw_save_pkg->count[0] = DATA16_H(diag_data[i]->count);
            raw_save_pkg->count[1] = DATA16_L(diag_data[i]->count);
            raw_save_pkg->channel = i;
            raw_save_pkg->pkg_cnts = j;
            rt_memcpy(&save_info->data[sizeof(clb_save_head_t) + sizeof(clb_raw_save_pkg_t)], diag_data[i]->raw_data + j * 512, 1024);
            save_info->check = crc8_maxim(save_info->data, hton16(*(uint16_t *)save_info->size));
            app_mcast_send_raw_data(local.raw_data_save, sizeof(clb_raw_save_info_t));
            rt_thread_delay(1);
        }

    }
}

/**
 * @brief
 * @param
 * @return
 * @note   原始数据转换为加速度   (20mv/g)加速度 = (((AD-32768)/65536.0)*20/0.02)/1.25(g)
 *         //振动数据采集是20mv/g，65536是2的16次方，对应电压20v， 0.02是代表20mv/g, 1.25代表电路放大1.25倍
**/
static void app_diag_raw_to_acc(diag_data_t **diag_data)
{
    for(uint8_t i = 0; i < AD7606_MAX; i++)
    {
        if(diag_data[i]->diag_state)
        {
            for(uint16_t j = 0; j < AD7606_SF; j++)
            {
                diag_data[i]->acc_data[j] = ((diag_data[i]->raw_data[j] - 32768) / 65536.0 * 20 / 0.02) / 1.25;
            }
            diag_data[i]->acc_state = true;
        }
        // AD7606_V5和AD7606_V6未用到
        if(i > AD7606_V4) {
            break;
        }
    }
}

/**
 * @brief  踏面结果输出策略
 * @param
 * @return
 * @note
**/
//static diag_state_e tread_strategy(uint8_t ch, uint8_t result)
//{
//    static uint8_t tread[AD7606_MAX][DIAG_STRATEGY_COUNT] = { 0 };
//    diag_state_e diag_state;
//    tread[ch][0] = result;
//    uint8_t warn_cnt = 0, alarm1_cnt = 0, alarm2_cnt = 0;
//    for(uint8_t i = 0; i < DIAG_STRATEGY_COUNT; i++)
//    {
//        if(tread[ch][i] == 1)  //预警
//        {
//            warn_cnt++;
//        }
//        else if(tread[ch][i] == 2)  //一级报警，包括了预警
//        {
//            warn_cnt++;
//            alarm1_cnt++;
//        }
//        else if(tread[ch][i] == 3)  //二级报警，包括了预警和一级报警
//        {
//            warn_cnt++;
//            alarm1_cnt++;
//            alarm2_cnt++;
//        }
//    }
//    if(DIAG_STRATEGY_COUNT > 2)
//    {
//        //移动窗口
//        for(uint8_t i = DIAG_STRATEGY_COUNT - 2; i >= 0; i--)
//        {
//            tread[ch][i + 1] = tread[ch][i];
//            if(i == 0) {
//                break;
//            }
//        }
//    }
//    if(alarm2_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_ALARM_2;
//    }
//    else if(alarm1_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_ALARM_1;
//    }
//    else if(warn_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_WARN;
//    }
//    else {
//        diag_state = DIAG_NORMAL;
//    }
//    return diag_state;
//}

/**
 * @brief  轴箱轴承结果输出策略
 * @param
 * @return
 * @note
**/
//static diag_state_e axle_bearing_strategy(uint8_t ch, uint8_t result)
//{
//    static uint8_t axle_bearing[AD7606_MAX][DIAG_STRATEGY_COUNT] = { 0 };
//    diag_state_e diag_state;
//    axle_bearing[ch][0] = result;
//    uint8_t warn_cnt = 0, alarm1_cnt = 0, alarm2_cnt = 0;
//    for(uint8_t i = 0; i < DIAG_STRATEGY_COUNT; i++)
//    {
//        if(axle_bearing[ch][i] == 1)  //预警
//        {
//            warn_cnt++;
//        }
//        else if(axle_bearing[ch][i] == 2)  //一级报警，包括了预警
//        {
//            warn_cnt++;
//            alarm1_cnt++;
//        }
//        else if(axle_bearing[ch][i] == 3)  //二级报警，包括了预警和一级报警
//        {
//            warn_cnt++;
//            alarm1_cnt++;
//            alarm2_cnt++;
//        }
//    }
//    if(DIAG_STRATEGY_COUNT > 2)
//    {
//        //移动窗口
//        for(uint8_t i = DIAG_STRATEGY_COUNT - 2; i >= 0; i--)
//        {
//            axle_bearing[ch][i + 1] = axle_bearing[ch][i];
//            if(i == 0) {
//                break;
//            }
//        }
//    }
//    if(alarm2_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_ALARM_2;
//    }
//    else if(alarm1_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_ALARM_1;
//    }
//    else if(warn_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_WARN;
//    }
//    else {
//        diag_state = DIAG_NORMAL;
//    }
//    return diag_state;
//}

/**
 * @brief  踏面与轴箱轴承诊断
 * @param
 * @return
 * @note   踏面 轴箱诊断,使用轴箱冲击数据
**/
static void app_diag_tread_axle(diag_data_t **diag_data)
{
    struct FFT_DIAG_PARA fft_diag_paras = { 0 };
    static zxzc_stack_t zxzc_stack[ADG1608_CH_MAX];
    ZXZC_status_out_t *ZXZC_status = RT_NULL;
    uint8_t zxzc_status_out = 0;
    uint8_t tread_status_out = 0;
    diag_state_e diag_state = DIAG_NORMAL;
    char log_data[100];

    for(uint8_t i = 0; i < AD7606_MAX; i++)
    {
        if(diag_data[i]->diag_state && (diag_data[i]->acc_state == true) && (diag_data[i]->data_type == AXLE_IMPACT))
        {
            //必须要先调用fft_diag_ZXZC_Wheel_init()函数对fft_diag_env->fft_diag_paras参数初始化
            fft_diag_paras.Sample_rate = AD7606_SF;
            fft_diag_paras.diag_speed = diag_data[i]->speed;
            fft_diag_paras.wheel_diameter = diag_data[i]->wheel;
            fft_diag_ZXZC_Wheel_init(&fft_diag_paras);
//            for(int cnt_i = 0; cnt_i < AD7606_SF; cnt_i++) {
//                //将采集数据转换为加速度并滤波后存到准备计算的buff中
//                diag_data[i]->acc_data[cnt_i] = prewarning1[cnt_i];
//            }
            ZXZC_status = ZXZC_Wheel_diag(diag_data[i]->acc_data, fft_diag_paras, &diag_data[i]->diag_save, &zxzc_stack[diag_data[i]->agd1608_ch]);
            if(ZXZC_status == RT_NULL) {
                continue ;
            }
            zxzc_status_out = ZXZC_status->status_bearing_comprehensive;
            tread_status_out = ZXZC_status->status_Wheel;
            //踏面报警结果填充
//            diag_state = tread_strategy(i, tread_status_out);
            diag_state = tread_status_out;
            if(tread_status_out != diag_state)
            {
                memset(log_data, 0, sizeof(log_data));
                sprintf(log_data, "diag tread ad_ch = %d, adg_ch = %d, count = %d result %d - %d", i, diag_data[i]->agd1608_ch, \
                        diag_data[i]->count, tread_status_out, diag_state);
                app_log_msg(LOG_LVL_INFO, true, log_data);
            }
            if(env.diag_msg.tread[diag_data[i]->agd1608_ch] != diag_state)
            {
                memset(log_data, 0, sizeof(log_data));
                sprintf(log_data, "send diag tread ad_ch = %d, adg_ch = %d, count = %d result %d - %d", i, diag_data[i]->agd1608_ch, \
                        diag_data[i]->count, env.diag_msg.tread[diag_data[i]->agd1608_ch], diag_state);
                app_log_msg(LOG_LVL_INFO, true, log_data);
            }
            env.diag_msg.tread[diag_data[i]->agd1608_ch] = diag_state;
            //轴箱轴承报警结果填充
//            diag_state = axle_bearing_strategy(i, zxzc_status_out);
            diag_state = zxzc_status_out;
            if(zxzc_status_out != diag_state)
            {
                memset(log_data, 0, sizeof(log_data));
                sprintf(log_data, "diag axle_bearing ad_ch = %d, adg_ch = %d, count = %d result %d - %d", i, diag_data[i]->agd1608_ch, \
                        diag_data[i]->count, zxzc_status_out, diag_state);
                app_log_msg(LOG_LVL_INFO, true, log_data);
            }
            if(env.diag_msg.axle_bearing[diag_data[i]->agd1608_ch] != diag_state)
            {
                memset(log_data, 0, sizeof(log_data));
                sprintf(log_data, "send diag axle_bearing ad_ch = %d, adg_ch = %d, count = %d result %d - %d", i, diag_data[i]->agd1608_ch, \
                        diag_data[i]->count, env.diag_msg.axle_bearing[diag_data[i]->agd1608_ch], diag_state);
                app_log_msg(LOG_LVL_INFO, true, log_data);
            }
            env.diag_msg.axle_bearing[diag_data[i]->agd1608_ch] = diag_state;
        }
    }
}

/**
 * @brief  电机轴承结果输出策略
 * @param
 * @return
 * @note
**/
//static diag_state_e motor_bearing_strategy(uint8_t ch, uint8_t result)
//{
//    static uint8_t motor_bearing[AD7606_MAX][DIAG_STRATEGY_COUNT] = { 0 };
//    diag_state_e diag_state;
//    motor_bearing[ch][0] = result;
//    uint8_t warn_cnt = 0, alarm1_cnt = 0, alarm2_cnt = 0;
//    for(uint8_t i = 0; i < DIAG_STRATEGY_COUNT; i++)
//    {
//        if(motor_bearing[ch][i] == 1)  //预警
//        {
//            warn_cnt++;
//        }
//        else if(motor_bearing[ch][i] == 2)  //一级报警，包括了预警
//        {
//            warn_cnt++;
//            alarm1_cnt++;
//        }
//        else if(motor_bearing[ch][i] == 3)  //二级报警，包括了预警和一级报警
//        {
//            warn_cnt++;
//            alarm1_cnt++;
//            alarm2_cnt++;
//        }
//    }
//    if(DIAG_STRATEGY_COUNT > 2)
//    {
//        //移动窗口
//        for(uint8_t i = DIAG_STRATEGY_COUNT - 2; i >= 0; i--)
//        {
//            motor_bearing[ch][i + 1] = motor_bearing[ch][i];
//            if(i == 0) {
//                break;
//            }
//        }
//    }
//    if(alarm2_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_ALARM_2;
//    }
//    else if(alarm1_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_ALARM_1;
//    }
//    else if(warn_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_WARN;
//    }
//    else {
//        diag_state = DIAG_NORMAL;
//    }
//    return diag_state;
//}

/**
 * @brief
 * @param
 * @return
 * @note   电机轴承诊断结果，使用电机冲击数据
**/
static void app_diag_motor_bear(diag_data_t **diag_data)
{
    struct FFT_DIAG_PARA fft_diag_paras = { 0 };
    static dj_clx_stack_t dj_zc_stack[ADG1608_CH_MAX];
    int dj_zc_out = 0;
    diag_state_e diag_state = DIAG_NORMAL;
    char log_data[250];

    for(uint8_t i = 0; i < AD7606_MAX; i++)
    {
        if(diag_data[i]->diag_state && (diag_data[i]->acc_state == true) && (diag_data[i]->data_type == MOTOR_IMPACT))
        {
            //必须要先调用fft_diag_DJ_init()函数对fft_diag_env->fft_diag_paras参数初始化
            fft_diag_paras.Sample_rate = AD7606_SF;
            fft_diag_paras.diag_speed = diag_data[i]->speed;
            fft_diag_paras.wheel_diameter = diag_data[i]->wheel;
            fft_diag_DJ_init(&fft_diag_paras);
            //fft_diag()函数第一个参数为AD采集到的加速度数据，单位为G，第二个参数为数据长度为4096
            dj_zc_out = DJ_diag(diag_data[i]->acc_data, fft_diag_paras, &diag_data[i]->diag_save, &dj_zc_stack[diag_data[i]->agd1608_ch / 2]);
//            diag_state = motor_bearing_strategy(i, dj_zc_out);
            diag_state = dj_zc_out;
            if(dj_zc_out != diag_state)
            {
                memset(log_data, 0, sizeof(log_data));
                sprintf(log_data, "diag motor_bearing ad_ch = %d, adg_ch = %d, count = %d result %d - %d", i, diag_data[i]->agd1608_ch, \
                        diag_data[i]->count, dj_zc_out, diag_state);
                app_log_msg(LOG_LVL_INFO, true, log_data);
            }
            if(env.diag_msg.motor_bearing[diag_data[i]->agd1608_ch] != diag_state)
            {
                memset(log_data, 0, sizeof(log_data));
                sprintf(log_data, "send diag motor_bearing ad_ch = %d, adg_ch = %d, count = %d result %d - %d", i, diag_data[i]->agd1608_ch, \
                        diag_data[i]->count, env.diag_msg.motor_bearing[diag_data[i]->agd1608_ch], diag_state);
                app_log_msg(LOG_LVL_INFO, true, log_data);
            }
            env.diag_msg.motor_bearing[diag_data[i]->agd1608_ch] = diag_state;
        }
    }
}

/**
 * @brief  齿轮箱轴承结果输出策略
 * @param
 * @return
 * @note
**/
//static diag_state_e box_bearing_strategy(uint8_t ch, uint8_t result)
//{
//    static uint8_t box_bearing[AD7606_MAX][DIAG_STRATEGY_COUNT] = { 0 };
//    diag_state_e diag_state;
//    box_bearing[ch][0] = result;
//    uint8_t warn_cnt = 0, alarm1_cnt = 0, alarm2_cnt = 0;
//    for(uint8_t i = 0; i < DIAG_STRATEGY_COUNT; i++)
//    {
//        if(box_bearing[ch][i] == 1)  //预警
//        {
//            warn_cnt++;
//        }
//        else if(box_bearing[ch][i] == 2)  //一级报警，包括了预警
//        {
//            warn_cnt++;
//            alarm1_cnt++;
//        }
//        else if(box_bearing[ch][i] == 3)  //二级报警，包括了预警和一级报警
//        {
//            warn_cnt++;
//            alarm1_cnt++;
//            alarm2_cnt++;
//        }
//    }
//    if(DIAG_STRATEGY_COUNT > 2)
//    {
//        //移动窗口
//        for(uint8_t i = DIAG_STRATEGY_COUNT - 2; i >= 0; i--)
//        {
//            box_bearing[ch][i + 1] = box_bearing[ch][i];
//            if(i == 0) {
//                break;
//            }
//        }
//    }
//    if(alarm2_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_ALARM_2;
//    }
//    else if(alarm1_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_ALARM_1;
//    }
//    else if(warn_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_WARN;
//    }
//    else {
//        diag_state = DIAG_NORMAL;
//    }
//    return diag_state;
//}

/**
 * @brief
 * @param
 * @return
 * @note   齿轮箱轴承诊断结果，使用齿轮冲击数据
**/
static void app_diag_gear_bear(diag_data_t **diag_data)
{
    struct FFT_DIAG_PARA fft_diag_paras = { 0 };
    static dj_clx_stack_t clx_zc_stack[ADG1608_CH_MAX];
    int clx_zc_out = 0;
    diag_state_e diag_state = DIAG_NORMAL;
    char log_data[100];

    for(uint8_t i = 0; i < AD7606_MAX; i++)
    {
        if(diag_data[i]->diag_state && (diag_data[i]->acc_state == true) && (diag_data[i]->data_type == GEAR_IMPACT))
        {
            //必须要先调用fft_diag_CLX_init()函数对fft_diag_env->fft_diag_paras参数初始化
            fft_diag_paras.Sample_rate = AD7606_SF;
            fft_diag_paras.diag_speed = diag_data[i]->speed;
            fft_diag_paras.wheel_diameter = diag_data[i]->wheel;
            fft_diag_CLX_init(&fft_diag_paras);
            //fft_diag()函数第一个参数为AD采集到的加速度数据，单位为G，第二个参数为数据长度为4096
            clx_zc_out = CLX_diag(diag_data[i]->acc_data, fft_diag_paras, &diag_data[i]->diag_save, &clx_zc_stack[diag_data[i]->agd1608_ch / 2]);
//            diag_state = box_bearing_strategy(i, clx_zc_out);
            diag_state = clx_zc_out;
            if(clx_zc_out != diag_state)
            {
                memset(log_data, 0, sizeof(log_data));
                sprintf(log_data, "diag box_bearing ad_ch = %d, adg_ch = %d, count = %d result %d - %d", i, diag_data[i]->agd1608_ch, \
                        diag_data[i]->count, clx_zc_out, diag_state);
                app_log_msg(LOG_LVL_INFO, true, log_data);
            }
            if(env.diag_msg.box_bearing[diag_data[i]->agd1608_ch] != diag_state)
            {
                memset(log_data, 0, sizeof(log_data));
                sprintf(log_data, "send diag box_bearing ad_ch = %d, adg_ch = %d, count = %d result %d - %d", i, diag_data[i]->agd1608_ch, \
                        diag_data[i]->count, env.diag_msg.box_bearing[diag_data[i]->agd1608_ch], diag_state);
                app_log_msg(LOG_LVL_INFO, true, log_data);
            }
            env.diag_msg.box_bearing[diag_data[i]->agd1608_ch] = diag_state;
        }
    }
}

/**
 * @brief  齿轮箱齿轮结果输出策略
 * @param
 * @return
 * @note
**/
//static diag_state_e box_gear_strategy(uint8_t ch, uint8_t result)
//{
//    static uint8_t box_gear[AD7606_MAX][DIAG_STRATEGY_COUNT] = { 0 };
//    diag_state_e diag_state;
//    box_gear[ch][0] = result;
//    uint8_t warn_cnt = 0, alarm1_cnt = 0, alarm2_cnt = 0;
//    for(uint8_t i = 0; i < DIAG_STRATEGY_COUNT; i++)
//    {
//        if(box_gear[ch][i] == 1)  //预警
//        {
//            warn_cnt++;
//        }
//        else if(box_gear[ch][i] == 2)  //一级报警，包括了预警
//        {
//            warn_cnt++;
//            alarm1_cnt++;
//        }
//        else if(box_gear[ch][i] == 3)  //二级报警，包括了预警和一级报警
//        {
//            warn_cnt++;
//            alarm1_cnt++;
//            alarm2_cnt++;
//        }
//    }
//    if(DIAG_STRATEGY_COUNT > 2)
//    {
//        //移动窗口
//        for(uint8_t i = DIAG_STRATEGY_COUNT - 2; i >= 0; i--)
//        {
//            box_gear[ch][i + 1] = box_gear[ch][i];
//            if(i == 0) {
//                break;
//            }
//        }
//    }
//    if(alarm2_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_ALARM_2;
//    }
//    else if(alarm1_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_ALARM_1;
//    }
//    else if(warn_cnt == DIAG_STRATEGY_COUNT) {
//        diag_state = DIAG_WARN;
//    }
//    else {
//        diag_state = DIAG_NORMAL;
//    }
//    return diag_state;
//}

/**
 * @brief
 * @param
 * @return
 * @note   齿轮箱齿轮诊断结果，使用齿轮振动数据
**/
static void app_diag_gear(diag_data_t **diag_data)
{
    struct FFT_DIAG_PARA fft_diag_paras = { 0 };
    static int clx_cl_stack[4][3]; //4个齿轮数据通道，每个通道需要3s的stack
    int clx_cl_out = 0;
    diag_state_e diag_state = DIAG_NORMAL;
    char log_data[100];

    for(uint8_t i = 0; i < AD7606_MAX; i++)
    {
        if(diag_data[i]->diag_state && (diag_data[i]->acc_state == true) && (diag_data[i]->data_type == GEAR_VIBRATE))
        {
            //必须要先调用fft_diag_gear_init()函数对fft_diag_env->fft_diag_paras参数初始化
            fft_diag_paras.Sample_rate = AD7606_SF;
            fft_diag_paras.diag_speed = diag_data[i]->speed;
            fft_diag_paras.wheel_diameter = diag_data[i]->wheel;
            fft_diag_gear_init(&fft_diag_paras);
            clx_cl_out = gear_diag(diag_data[i]->acc_data, fft_diag_paras, &diag_data[i]->diag_save, clx_cl_stack[diag_data[i]->agd1608_ch / 2]);
//            diag_state = box_gear_strategy(i, clx_cl_out);
            diag_state = clx_cl_out;
            if(clx_cl_out != diag_state)
            {
                memset(log_data, 0, sizeof(log_data));
                sprintf(log_data, "diag box_gear ad_ch = %d, adg_ch = %d, count = %d result %d - %d", i, diag_data[i]->agd1608_ch, \
                        diag_data[i]->count, clx_cl_out, diag_state);
                app_log_msg(LOG_LVL_INFO, true, log_data);
            }
            if(env.diag_msg.box_gear[diag_data[i]->agd1608_ch] != diag_state)
            {
                memset(log_data, 0, sizeof(log_data));
                sprintf(log_data, "send diag box_gear ad_ch = %d, adg_ch = %d, count = %d result %d - %d", i, diag_data[i]->agd1608_ch, \
                        diag_data[i]->count, env.diag_msg.box_gear[diag_data[i]->agd1608_ch], diag_state);
                app_log_msg(LOG_LVL_INFO, true, log_data);
            }
            env.diag_msg.box_gear[diag_data[i]->agd1608_ch] = diag_state;
        }
    }
}
static void derail_init(struct DERAIL_CALC *calc)
{
    calc->acc_buf = (float *)malloc( AD7606_SF * sizeof(float));
    memset(calc->acc_buf, 0, AD7606_SF * sizeof(float));
//    calc->buf_len=SAMPLE_SIZE;
//    para.abs_alarm_num=10;
//    para.abs_alarm_thteshold=100.0;
//    para.amplitude_alarm_num=50;
//    para.amplitude_alarm_thteshold=30;
//    para.uplift_alarm_thteshold=3;
}
static void app_diag_polygon(diag_data_t **diag_data)
{
    static struct DERAIL_CALC calc;
    static struct POLYGON_DIAG_PARA polygon_para = {0};
    static struct POLYGON_DIAG_SAVE polygon_diag_save = {0};
    static float indicator[DIAG_CHANNEL_NUM];
    static float rail_corrugation[DIAG_CHANNEL_NUM];
    static struct CORRUGATION_DIAG_SAVE corrugation_diag_save = {0};//波磨诊断结果
    static polygon_diag_stack_t polygon_stack[8] = {0};
    static uint16_t wheel = 0;
    static uint16_t speed = 0;
    static float x[9] = {0};
    static float y[9] = {0};
    static uint8_t init_data_fisrt = 0;
    if(!init_data_fisrt) {
        derail_init(&calc);
        init_data_fisrt = 1;
    }

    for(uint8_t i = 0; i < AD7606_MAX; i++) {
        if((diag_data[i]->data_type == AXLE_VIBRATE) && (diag_data[i]->diag_state == true) && (diag_data[i]->acc_state == true)) {

            ccu_public_t *txb_public_info = (ccu_public_t *)app_mcast_env.txb_info.trust_mvb->data;
            if(txb_public_info->valid_flag.bits.wheel_valid) {
                wheel = (uint16_t)txb_public_info->wheel_1[0] << 8 | txb_public_info->wheel_1[1];
            }
            else {
                wheel = 805;
            }
            if(txb_public_info->valid_flag.bits.speed_valid) {
                speed = (uint16_t)txb_public_info->speed[0] << 8 | txb_public_info->speed[1];
            }
            else {
                speed = app_period_env.clb_info.speed;
            }

//            if (1)
//            {
//                //模拟公共信息参数,用于算法测试
//                speed = (uint16_t)0x27 << 8 | 0x10;
//                wheel = (uint16_t)0x03 << 8 | 0x25;
//            }
            //多边形诊断参数配置
            polygon_para.wheel_diameter = (float)wheel/1000;//单位 m    轮径，默认是0.805
            polygon_para.Sample_rate = DIAG_RATE;           //采样频率
            polygon_para.polygon_diag_speed = (float)speed/100;   // km/h 车辆运行的速度
//            polygon_para.polygon_diag_speed = (float)speed;
            polygon_para.threshold_prognosis = DIAG_THRESHOLD_PROGNOSIS;
            polygon_para.threshold_prewarning = DIAG_THRESHOLD_PREWARNING;
            polygon_para.threshold_warning = DIAG_THRESHOLD_WARNING;
            polygon_para.time_length = DIAG_STACK_LEN;

            if(diag_data[i]->agd1608_ch == 0) {
                memset(x, 0, sizeof(float)*9);
                memset(y, 0, sizeof(float)*9);
            }
            for(int cnt_i = 0; cnt_i < AD7606_SF; cnt_i++) {
                //将采集数据转换为加速度并滤波后存到准备计算的buff中
//                env.diag_data[i]->acc_data[cnt_i] = prewarning1[cnt_i];
                calc.acc_buf[cnt_i] = IIR_filter_polygon(env.diag_data[i]->acc_data[cnt_i], x, y);
            }

            polygon_diag(calc.acc_buf,polygon_para,&polygon_diag_save,&polygon_stack[diag_data[i]->agd1608_ch]);

            indicator[diag_data[i]->agd1608_ch] = polygon_diag_save.indicator;  //保存到数组中用于波磨计算
            rail_corrugation[diag_data[i]->agd1608_ch] = polygon_diag_save.nameda_result;   //保存到数组中用于波磨计算

            env.tz_data.dbx.polygon_tz[diag_data[i]->agd1608_ch].TZ1_order_h = polygon_diag_save.order_result>>8&0x000000ff;
            env.tz_data.dbx.polygon_tz[diag_data[i]->agd1608_ch].TZ1_order_l = polygon_diag_save.order_result&0x000000ff;
            env.tz_data.dbx.polygon_tz[diag_data[i]->agd1608_ch].TZ2_rms_h = (int)(polygon_diag_save.rms_result*100)>>8&0x000000ff;
            env.tz_data.dbx.polygon_tz[diag_data[i]->agd1608_ch].TZ2_rms_l = (int)(polygon_diag_save.rms_result*100)&0x000000ff;
            env.tz_data.dbx.polygon_tz[diag_data[i]->agd1608_ch].TZ3_DB_h = (int)(polygon_diag_save.DB_result*100)>>8&0x000000ff;
            env.tz_data.dbx.polygon_tz[diag_data[i]->agd1608_ch].TZ3_DB_l = (int)(polygon_diag_save.DB_result*100)&0x000000ff;
            env.tz_data.dbx.polygon_tz[diag_data[i]->agd1608_ch].diag_res = polygon_diag_save.status_polygon&0x000000ff;
            env.tz_data.dbx.polygon_tz[diag_data[i]->agd1608_ch].channel_id = diag_data[i]->agd1608_ch;

#if 1
            printf("polygon_para---> wheel=%f, speed=%f\n",
                    polygon_para.wheel_diameter,polygon_para.polygon_diag_speed);
            printf("polygon diag result: --->\n    order=%d, rms=%f, DB=%f, status=%d, speed=%f\n    k = %d n = %f\n",
                    polygon_diag_save.order_result,polygon_diag_save.rms_result,
                    polygon_diag_save.DB_result,polygon_diag_save.status_polygon,polygon_diag_save.speed,
                    polygon_diag_save.indicator, polygon_diag_save.nameda_result);
#endif
            //下面是波磨的程序，K1到K8是同车8个踏面位置算出来的多边形指标,nameda1到nameda8是同车8个踏面位置算出来的波长指标
            corrugation_diag(indicator,rail_corrugation, &corrugation_diag_save);
            //保存波磨TZ数据到发送buff中
            env.tz_data.dbx.wave_tz.indi1357_result_h = (int)(corrugation_diag_save.indicator1357_result*100)>>8&0x000000ff;
            env.tz_data.dbx.wave_tz.indi1357_result_l = (int)(corrugation_diag_save.indicator1357_result*100)&0x000000ff;
            env.tz_data.dbx.wave_tz.indi2468_result_h = (int)(corrugation_diag_save.indicator2468_result*100)>>8&0x000000ff;
            env.tz_data.dbx.wave_tz.indi2468_result_l = (int)(corrugation_diag_save.indicator2468_result*100)&0x000000ff;
            env.tz_data.dbx.wave_tz.nameda1357_result_h = (int)(corrugation_diag_save.nameda1357_result*100)>>8&0x000000ff;
            env.tz_data.dbx.wave_tz.nameda1357_result_l = (int)(corrugation_diag_save.nameda1357_result*100)&0x000000ff;
            env.tz_data.dbx.wave_tz.nameda2468_result_h = (int)(corrugation_diag_save.nameda2468_result*100)>>8&0x000000ff;
            env.tz_data.dbx.wave_tz.nameda2468_result_l = (int)(corrugation_diag_save.nameda2468_result*100)&0x000000ff;

        }
    }
}
/**
 * @brief
 * @param
 * @return
 * @note   填充保存齿轮箱数据
**/
static void diag_fill_clx_zc(save_dj_clx_t* save_zc, diag_data_t* diag_data)
{
    int DB_result[Diag_Num_CLX];
    for(int i = 0; i < Diag_Num_CLX; i++)
    {
        //直接取整
        DB_result[i] = (int)diag_data->diag_save.DB[i]; //DB值
    }
    //填充齿轮箱轴承诊断数据
    save_zc->channel_id = diag_data->agd1608_ch + 1;
    save_zc->speed_valid_flag = 1;// xxx 默认速度有效
    //齿轮箱输入电机侧
    save_zc->bearing_tz1.amplitude_inner_ring = (uint8_t)DB_result[Diag_NH_OFFSET];
    save_zc->bearing_tz1.amplitude_outer_ring = (uint8_t)DB_result[Diag_WH_OFFSET];
    save_zc->bearing_tz1.amplitude_rolling_body = (uint8_t)DB_result[Diag_GDT_OFFSET];
    save_zc->bearing_tz1.amplitude_holder = (uint8_t)DB_result[Diag_BCJ_OFFSET];
    //齿轮箱输入车轮侧
    save_zc->bearing_tz2.amplitude_inner_ring = (uint8_t)DB_result[Diag_NH_OFFSET+Diag_Num_COMMON];
    save_zc->bearing_tz2.amplitude_outer_ring = (uint8_t)DB_result[Diag_WH_OFFSET+Diag_Num_COMMON];
    save_zc->bearing_tz2.amplitude_rolling_body = (uint8_t)DB_result[Diag_GDT_OFFSET+Diag_Num_COMMON];
    save_zc->bearing_tz2.amplitude_holder = (uint8_t)DB_result[Diag_BCJ_OFFSET+Diag_Num_COMMON];
    //报警状态
    save_zc->alarm_bearing = env.diag_msg.box_bearing[diag_data->agd1608_ch];
    //发送报文信息填充
    memcpy(&env.tz_data.dj_clx_zc, save_zc, sizeof(save_dj_clx_t));

}
/**
 * @brief
 * @param
 * @return
 * @note   填充保存电机诊断数据
**/
static void diag_fill_dj(save_dj_clx_t* save_zc, diag_data_t* diag_data)
{

    int DB_result[Diag_Num_DJ];

    for(int i = 0; i < Diag_Num_DJ; i++)
    {
        //直接取整
        DB_result[i] = (int)diag_data->diag_save.DB[i]; //DB值
    }
    //填充齿轮箱轴承诊断数据
    save_zc->channel_id = diag_data->agd1608_ch + 1;
    save_zc->speed_valid_flag = 1;// xxx 默认速度有效
    //电机驱动端存储数据
    save_zc->bearing_tz1.amplitude_inner_ring = (uint8_t)DB_result[Diag_NH_OFFSET];
    save_zc->bearing_tz1.amplitude_outer_ring = (uint8_t)DB_result[Diag_WH_OFFSET];
    save_zc->bearing_tz1.amplitude_rolling_body = (uint8_t)DB_result[Diag_GDT_OFFSET];
    save_zc->bearing_tz1.amplitude_holder = (uint8_t)DB_result[Diag_BCJ_OFFSET];
    //电机非驱动端存储数据
    save_zc->bearing_tz2.amplitude_inner_ring = (uint8_t)DB_result[Diag_NH_OFFSET+Diag_Num_COMMON];
    save_zc->bearing_tz2.amplitude_outer_ring = (uint8_t)DB_result[Diag_WH_OFFSET+Diag_Num_COMMON];
    save_zc->bearing_tz2.amplitude_rolling_body = (uint8_t)DB_result[Diag_GDT_OFFSET+Diag_Num_COMMON];
    save_zc->bearing_tz2.amplitude_holder = (uint8_t)DB_result[Diag_BCJ_OFFSET+Diag_Num_COMMON];
    //报警状态

    save_zc->alarm_bearing = env.diag_msg.motor_bearing[diag_data->agd1608_ch];
    //发送报文信息填充
    memcpy(&env.tz_data.dj_clx_zc, save_zc, sizeof(save_dj_clx_t));
}
/**
 * @brief
 * @param
 * @return
 * @note   填充保存轴箱轴承诊断数据
**/
static void diag_fill_zxzc(save_zxzc_t* save_zc, diag_data_t* diag_data)
{

    int DB_result[Diag_Num_ZXZC_Wheel];

    for(int i = 0; i < Diag_Num_ZXZC_Wheel; i++)
    {
        //直接取整
        DB_result[i] = (int)diag_data->diag_save.DB[i]; //DB值
    }
    //填充齿轮箱轴承诊断数据
    save_zc->channel_id = diag_data->agd1608_ch + 1;
//  save_zc->speed_valid_flag = 1;// xxx 默认速度有效
    //轴箱轴承存储数据
    save_zc->bearing_tz.amplitude_inner_ring = (uint8_t)DB_result[Diag_NH_OFFSET];
    save_zc->bearing_tz.amplitude_outer_ring = (uint8_t)DB_result[Diag_WH_OFFSET];
    save_zc->bearing_tz.amplitude_rolling_body = (uint8_t)DB_result[Diag_GDT_OFFSET];
    save_zc->bearing_tz.amplitude_holder = (uint8_t)DB_result[Diag_BCJ_OFFSET];
    save_zc->tm_db = (uint8_t)DB_result[Diag_TM_PFFSET];
    //报警状态
    save_zc->alarm_bearing = env.diag_msg.axle_bearing[diag_data->agd1608_ch];
    save_zc->alarm_tm = env.diag_msg.tread[diag_data->agd1608_ch];
    //发送报文信息填充
    memcpy(&env.tz_data.zx_zc, save_zc, sizeof(save_zxzc_t));
}
/**
 * @brief
 * @param
 * @return
 * @note   填充保存齿轮诊断数据（使用齿轮振动数据计算）
**/
static void diag_fill_cl(save_gear_t* save_cl, diag_data_t* diag_data)
{

    int DB_result;
    int amp_result;
    int fre_result;

    //直接取整
    DB_result = (int)(diag_data->diag_save.DB[0]*100); //DB值
    amp_result = (int)(diag_data->diag_save.amp[0]*100); //DB值
    fre_result = (int)(diag_data->diag_save.fre[0]*100); //DB值

    //填充齿轮箱轴承诊断数据
    save_cl->channel_id = diag_data->agd1608_ch + 1;
    save_cl->gear_DB_h = (DB_result>>8) & 0x000000ff;
    save_cl->gear_DB_l = DB_result & 0x000000ff;
    save_cl->gear_amp_h = ((amp_result*100)>>8) & 0x000000ff;
    save_cl->gear_amp_l = (amp_result*100) & 0x000000ff;
    save_cl->gear_fre_h = ((fre_result*100)>>8) & 0x000000ff;
    save_cl->gear_fre_l = (fre_result*100) & 0x000000ff;
    save_cl->alarm_status = env.diag_msg.box_gear[diag_data->agd1608_ch];
    //发送报文信息填充
    memcpy(&env.tz_data.cl, save_cl, sizeof(save_gear_t));
}
/**
 * @brief
 * @param
 * @return
 * @note   填充保存多边形数据
**/
static void diag_fill_dbx(polygon_tz_t* save_dbx, diag_data_t* diag_data)
{
    //填充多边形诊断数据
//    save_dbx->channel_id = diag_data->agd1608_ch + 1;
//    save_dbx->diag_res = diag_data->polygon_save.status_polygon & 0x000000ff;// 多边形状态
//    save_dbx->TZ1_order_h = (diag_data->polygon_save.order_result>>8) & 0x000000ff;
//    save_dbx->TZ1_order_l = diag_data->polygon_save.order_result & 0x000000ff;
//    save_dbx->TZ2_rms_h = (int)(diag_data->polygon_save.rms_result*100) >> 8 & 0x000000ff;
//    save_dbx->TZ2_rms_l = (int)(diag_data->polygon_save.rms_result*100) & 0x000000ff;
//    save_dbx->TZ3_DB_h = (int)(diag_data->polygon_save.DB_result*100) >> 8 & 0x000000ff;
//    save_dbx->TZ3_DB_l = (int)(diag_data->polygon_save.DB_result*100) & 0x000000ff;

    //发送报文信息填充
    rt_memcpy(save_dbx, env.tz_data.dbx.polygon_tz, sizeof(polygon_tz_t));
    memcpy(&env.tz_data.dbx, save_dbx, sizeof(polygon_tz_t));
}
/**
 * @brief
 * @param
 * @return
 * @note   保存温度数据
**/
void diag_fill_temperature(void)
{
    qzq_data_t *qzq_data = NULL;
    save_tz_temp_t* save_tz_temp = &env.tz_data.temperature;
    save_cgq_t* save_cgq_state = &env.tz_data.cgq;
    sys_paras_t *paras = app_paras_get();

    save_tz_temp->train_id = paras->train_num;
    for (int i=0; i<QZQ_MAX; i++)
    {
        qzq_data = (qzq_data_t *)&app_qzq_env.qzq_data[i];
        if(i == QZQ_1)
        {
            //X车1号前置处理器故障（不区分4编组或者6编组，头尾车或者中间车）
            save_cgq_state->qzq_status.bits.qzq_1 = app_qzq_env.qzq_data[i].com_state ? 0 : 1;
            //--------前置器1的1位   不区分编组和车厢类型------
            //X车1位轴箱传感器故障（不区分4编组或者6编组，头尾车或者中间车）
            save_cgq_state->zxzc_cgq_status.bits.num_1 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
            //X车1位轴箱温度特征状态
            save_tz_temp->diag_res.zx_temp[AXLE_1] = qzq_data->temp_diag[TEMP_PT1].temp_state;
            //X车1位轴箱温度值
            save_tz_temp->temp.zx_temp[INDEX_1] = qzq_data->temp_diag[TEMP_PT1].temp_data;  //这个对应关系需要查看接线图
            //--------前置器1的2位   不区分编组和车厢类型--------
            //X车2位轴箱传感器故障（不区分4编组或者6编组，头尾车或者中间车）
            save_cgq_state->zxzc_cgq_status.bits.num_2 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
            //X车2位轴箱温度特征状态
            save_tz_temp->diag_res.zx_temp[AXLE_2] = qzq_data->temp_diag[TEMP_PT2].temp_state;
            //X车2位轴箱温度值
            save_tz_temp->temp.zx_temp[INDEX_2] = qzq_data->temp_diag[TEMP_PT2].temp_data;
            if (env.host_flag)
            {//主机
                //--------前置器1的3位   头尾车，不区分编组--------
                //X车3位轴箱传感器故障
                save_cgq_state->zxzc_cgq_status.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                //X车3位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[AXLE_3] = qzq_data->temp_diag[TEMP_PT3].temp_state;
                //X车3位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                //--------前置器1的4位   头尾车，不区分编组--------
                //X车4位轴箱传感器故障
                save_cgq_state->zxzc_cgq_status.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                //X车4位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[AXLE_4] = qzq_data->temp_diag[TEMP_PT4].temp_state;
                //X车4位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT4].temp_data;
            }
            else {
                //--------前置器1的3位   中间车，不区分编组--------
                //X车1轴齿轮箱传感器故障（前置器1的3位    4编组或者6编组中间车）
                save_cgq_state->clx_cgq_status.bits.num_1 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                //X车1位齿轮箱温度特征状态
                save_tz_temp->diag_res.clx_temp[INDEX_1] = qzq_data->temp_diag[TEMP_PT3].temp_state;
                //X车1位齿轮箱箱温度
                save_tz_temp->temp.clx_temp[INDEX_1] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                //--------前置器1的4位   中间车，不区分编组--------
                //X车1轴电机传感器故障（前置器1的4位    4编组或者6编组中间车）
                save_cgq_state->dj_cgq_status.bits.num_1 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                //X车1位电机温度特征状态
                save_tz_temp->diag_res.dj_temp[INDEX_1] = qzq_data->temp_diag[TEMP_PT4].temp_state;
                //X车1位电机温度
                save_tz_temp->temp.dj_temp[INDEX_1] = qzq_data->temp_diag[TEMP_PT4].temp_data;
            }
        }
        else if(i == QZQ_2)
        {
            //X车2号前置处理器故障（不区分4编组或者6编组，头尾车或者中间）
            save_cgq_state->qzq_status.bits.qzq_2 = app_qzq_env.qzq_data[i].com_state ? 0 : 1;
            if (env.host_flag)
            {
                //--------前置器2的1位   头尾车，不区分编组--------
                //X车5位轴箱传感器故障（4编组或者6编组头尾车，在前置器2的1位）
                save_cgq_state->zxzc_cgq_status.bits.num_5 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
                //X车5位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[INDEX_5] = qzq_data->temp_diag[TEMP_PT1].temp_state;
                //X车5位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_5] = qzq_data->temp_diag[TEMP_PT1].temp_data;
                //--------前置器2的2位   头尾车，不区分编组--------
                //X车6位轴箱传感器故障（4编组或者6编组头尾车，在前置器2的2位）
                save_cgq_state->zxzc_cgq_status.bits.num_6 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
                //X车6位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[INDEX_6] = qzq_data->temp_diag[TEMP_PT2].temp_state;
                //X车6位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_6] = qzq_data->temp_diag[TEMP_PT2].temp_data;
                if (env.host_qzq_num == 3)
                {
                    //--------前置器2的3位   头尾车，4编组--------
                    //X车3轴齿轮传感器故障（4编组头尾车，在前置器2的3位）
                    save_cgq_state->clx_cgq_status.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                    //X车3位电机温度特征状态
                    save_tz_temp->diag_res.clx_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT3].temp_state;
                    //X车3位齿轮箱温度
                    save_tz_temp->temp.clx_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                    //--------前置器2的4位   头尾车，4编组--------
                    //X车3轴电机传感器故障（4编组头尾车，在前置器2的4位）
                    save_cgq_state->dj_cgq_status.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                    //X车3位电机温度特征状态
                    save_tz_temp->diag_res.dj_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT4].temp_state;
                    //X车3位电机温度
                    save_tz_temp->temp.dj_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT4].temp_data;
                }
                else if (env.host_qzq_num == 2)
                {
                    //--------前置器2的3位   头尾车，6编组--------
                    //X车7位轴箱传感器故障（6编组头尾车，在前置器2的3位）
                    save_cgq_state->zxzc_cgq_status.bits.num_7 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                    //X车7位电机温度特征状态
                    save_tz_temp->diag_res.zx_temp[INDEX_7] = qzq_data->temp_diag[TEMP_PT3].temp_state;
                    //X车7位轴箱温度
                    save_tz_temp->temp.zx_temp[INDEX_7] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                    //--------前置器2的3位   头尾车，6编组--------
                    //X车8位轴箱传感器故障（6编组头尾车，在前置器2的4位）
                    save_cgq_state->zxzc_cgq_status.bits.num_8 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                    //X车7位电机温度特征状态
                    save_tz_temp->diag_res.zx_temp[INDEX_8] = qzq_data->temp_diag[TEMP_PT4].temp_state;
                    //X车8位轴箱温度
                    save_tz_temp->temp.zx_temp[INDEX_8] = qzq_data->temp_diag[TEMP_PT4].temp_data;
                }
            }
            else {
                //--------前置器2的1位   中间车，不区分编组--------
                //X车3位轴箱传感器故障（4编组或者6编组，中间车在前置器2的1位）
                save_cgq_state->zxzc_cgq_status.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
                //X车3位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT1].temp_state;
                //X车3位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_3] =qzq_data->temp_diag[TEMP_PT1].temp_data;
                //--------前置器2的2位   中间车，不区分编组--------
                //X车4位轴箱传感器故障（4编组或者6编组，中间车在前置器2的2位）
                save_cgq_state->zxzc_cgq_status.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
                //X车4位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT2].temp_state;
                //X车4位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT2].temp_data;
                //--------前置器2的3位   中间车，不区分编组--------
                //X车2轴齿轮传感器故障（4编组或者6编组中间车在前置器2的3位）
                save_cgq_state->clx_cgq_status.bits.num_2 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                //X车2位齿轮箱温度特征状态
                save_tz_temp->diag_res.clx_temp[INDEX_2] = qzq_data->temp_diag[TEMP_PT3].temp_state;
                //X车2位齿轮箱温度
                save_tz_temp->temp.clx_temp[INDEX_2] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                //--------前置器2的4位   中间车，不区分编组--------
                //X车2轴电机传感器故障（4编组或者6编组中间车在前置器2的4位）
                save_cgq_state->dj_cgq_status.bits.num_2 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                //X车2位电机温度特征状态
                save_tz_temp->diag_res.dj_temp[INDEX_2] = qzq_data->temp_diag[TEMP_PT4].temp_state;
                //X车2位电机温度
                save_tz_temp->temp.dj_temp[INDEX_2] = qzq_data->temp_diag[TEMP_PT4].temp_data;
            }
        }
        else if((i == QZQ_3))
        {

            if (env.host_flag && (env.host_qzq_num == 3))
            {
                //X车3号前置处理器故障（4编组或者6编组中间车都有3号前置器）
                save_cgq_state->qzq_status.bits.qzq_3 = app_qzq_env.qzq_data[i].com_state ? 0 : 1;
                //--------前置器3的1位   头尾车，4编组--------
                //X车7位轴箱传感器故障（4编组头尾车，在前置器3的1位）
                save_cgq_state->zxzc_cgq_status.bits.num_7 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
                //X车7位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[INDEX_7] = qzq_data->temp_diag[TEMP_PT1].temp_state;
                //X车7位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_7] = qzq_data->temp_diag[TEMP_PT1].temp_data;
                //--------前置器3的2位   头尾车，4编组--------
                //X车8位轴箱传感器故障（4编组头尾车，在前置器3的2位）
                save_cgq_state->zxzc_cgq_status.bits.num_8 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
                //X车8位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[INDEX_8] = qzq_data->temp_diag[TEMP_PT2].temp_state;
                //X车8位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_8] = qzq_data->temp_diag[TEMP_PT2].temp_data;
                //--------前置器3的3位   头尾车，4编组--------
                //X车5位轴箱传感器故障（4编组或者6编组中间车在前置器3的1位）
                save_cgq_state->clx_cgq_status.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                //X车4位齿轮箱温度特征状态
                save_tz_temp->diag_res.clx_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT3].temp_state;
                //X车4位齿轮箱温度
                save_tz_temp->temp.clx_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                //--------前置器3的4位   头尾车，4编组--------
                //X车4轴电机传感器故障（4编组头尾车，在前置器3的4位）
                save_cgq_state->dj_cgq_status.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                //X车4位电机温度特征状态
                save_tz_temp->diag_res.dj_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT4].temp_state;
                //X车4位电机温度
                save_tz_temp->temp.dj_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT4].temp_data;
            }
            else if (!env.host_flag)
            {
                //X车3号前置处理器故障（4编组或者6编组中间车都有3号前置器）
                save_cgq_state->qzq_status.bits.qzq_3 = app_qzq_env.qzq_data[i].com_state ? 0 : 1;

                //--------前置器3的1位   中间车，不区分编组--------
                //X车5位轴箱传感器故障（4编组或者6编组，中间车在前置器2的1位）
                save_cgq_state->zxzc_cgq_status.bits.num_5 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
                //X车5位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[INDEX_5] = qzq_data->temp_diag[TEMP_PT1].temp_state;
                //X车5位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_5] = qzq_data->temp_diag[TEMP_PT1].temp_data;
                //--------前置器3的2位   中间车，不区分编组--------
                //X车6位轴箱传感器故障（4编组或者6编组中间车在前置器3的2位）
                save_cgq_state->zxzc_cgq_status.bits.num_6 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
                //X车6位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[INDEX_6] = qzq_data->temp_diag[TEMP_PT2].temp_state;
                //X车6位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_6] = qzq_data->temp_diag[TEMP_PT2].temp_data;
                //--------前置器3的3位   中间车，不区分编组--------
                //X车3轴齿轮传感器故障（4编组或者6编组中间车在前置器3的3位）
                save_cgq_state->clx_cgq_status.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                //X车3位齿轮箱温度特征状态
                save_tz_temp->diag_res.clx_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT3].temp_state;
                //X车3位齿轮箱温度
                save_tz_temp->temp.clx_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                //--------前置器3的4位   中间车，不区分编组--------
                //X车3轴电机传感器故障（4编组或者6编组中间车在前置器3的4位）
                save_cgq_state->dj_cgq_status.bits.num_3 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                //X车3位电机温度特征状态
                save_tz_temp->diag_res.dj_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT4].temp_state;
                //X车3位电机温度
                save_tz_temp->temp.dj_temp[INDEX_3] = qzq_data->temp_diag[TEMP_PT4].temp_data;
            }
        }
        else if(i == QZQ_4)
        {
            if(!env.host_flag)
            {
                //X车3号前置处理器故障（4编组或者6编组，只用4编组的头尾车有）
                save_cgq_state->qzq_status.bits.qzq_4 = app_qzq_env.qzq_data[i].com_state ? 0 : 1;
                //--------前置器4的1位   中间车，不区分编组--------
                //X车7位轴箱传感器故障（4编组或者6编组，中间车在前置器4的1位）
                save_cgq_state->zxzc_cgq_status.bits.num_7 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_1] ? 0 : 1;
                //X车7位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[INDEX_7] = qzq_data->temp_diag[TEMP_PT1].temp_state;
                //X车7位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_7] = qzq_data->temp_diag[TEMP_PT1].temp_data;
                //--------前置器4的2位   中间车，不区分编组--------
                //X车8位轴箱传感器故障（4编组或者6编组，中间车在前置器4的2位）
                save_cgq_state->zxzc_cgq_status.bits.num_8 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_2] ? 0 : 1;
                //X车8位轴箱温度特征状态
                save_tz_temp->diag_res.zx_temp[INDEX_8] = qzq_data->temp_diag[TEMP_PT2].temp_state;
                //X车8位轴箱温度
                save_tz_temp->temp.zx_temp[INDEX_8] = qzq_data->temp_diag[TEMP_PT2].temp_data;
                //--------前置器4的3位   中间车，不区分编组--------
                //X车4轴齿轮传感器故障（4编组或者6编组，中间车在前置器4的3位）
                save_cgq_state->clx_cgq_status.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_3] ? 0 : 1;
                //X车4位齿轮箱温度特征状态
                save_tz_temp->diag_res.clx_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT3].temp_state;
                //X车4位齿轮箱温度
                save_tz_temp->temp.clx_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT3].temp_data;
                //--------前置器4的4位   中间车，不区分编组--------
                //X车4轴电机传感器故障（4编组或者6编组，中间车在前置器4的4位）
                save_cgq_state->dj_cgq_status.bits.num_4 = app_qzq_env.qzq_data[i].cgq_state[ADG1609_CH_4] ? 0 : 1;
                //X车4位电机温度特征状态
                save_tz_temp->diag_res.dj_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT4].temp_state;
                //X车4位电机温度
                save_tz_temp->temp.dj_temp[INDEX_4] = qzq_data->temp_diag[TEMP_PT4].temp_data;
            }
        }
        //环境温度
        save_tz_temp->env_temp[i] = qzq_data->temp_diag[TEMP_ENV].temp_data;
    }
}

/**
 * @brief
 * @param
 * @return
 * @note   诊断结果进行保存，浮点型数据放大1000倍存储，文件大小103K
**/
static void app_diag_result_save(diag_data_t **diag_data)
{
    sys_paras_t *paras = app_paras_get();

    clb_raw_save_info_t *raw_save_info = (clb_raw_save_info_t *)local.raw_data_save;
    clb_save_head_t *save_head = (clb_save_head_t *)raw_save_info->data;
    clb_feature_save_info_t *tz_save_info = (clb_feature_save_info_t *)local.tz_data_save;

    //传输头
    *(uint16_t *)tz_save_info->head = HEAD_MCAST_CLB_FEATURE_DATA;
    tz_save_info->size[0] = DATA16_H(sizeof(clb_feature_save_info_t) - 5);
    tz_save_info->size[1] = DATA16_L(sizeof(clb_feature_save_info_t) - 5);

    env.tz_data.train_id = paras->train_num;
    env.tz_data.year = save_head->year;
    env.tz_data.mon = save_head->month;
    env.tz_data.day = save_head->day;
    env.tz_data.hour = save_head->hour;
    env.tz_data.min = save_head->min;
    env.tz_data.sec = save_head->sec;
    diag_fill_temperature();
    for(int i=0; i<AD7606_MAX; i++)
    {
        if(diag_data[i]->diag_state && (diag_data[i]->acc_state == true))
        {
            save_zxzc_t save_zc = {0};
            save_dj_clx_t save_dj_clx = {0}; //踏面数据
            save_gear_t save_cl = {0};
#ifdef USE_DERAILMENT
            polygon_tz_t save_dbx = {0}; //多边形数据
#endif

            //根据采集数据类型保存
            switch (diag_data[i]->data_type)
            {
                case GEAR_IMPACT:  //齿轮冲击-app_diag_wheel_bearing()小齿轮箱车轮侧轴承诊断
                    diag_fill_clx_zc(&save_dj_clx, diag_data[i]);
                    break;

                case MOTOR_IMPACT: //齿端电机轴承诊断结果，使用电机冲击数据
                    diag_fill_dj(&save_dj_clx, diag_data[i]);
                    break;

                case GEAR_VIBRATE: //小齿轮箱车轮侧齿轮诊断结果，使用齿轮振动数据
                    diag_fill_cl(&save_cl, diag_data[i]);
                    break;

                case AXLE_IMPACT: //轴箱轴承与踏面诊断结果，使用轴箱轴承冲击数据
                    diag_fill_zxzc(&save_zc, diag_data[i]);
                    break;
#ifdef USE_DERAILMENT
                case AXLE_VIBRATE: //多边形诊断使用轴箱轴承振动数据
                    diag_fill_dbx(&save_dbx, diag_data[i]);
                    break;
#endif
                default:
                    break;
            }
        }
    }
    memcpy(tz_save_info->data, &env.tz_data, sizeof(feature_data_t));
    tz_save_info->check = crc8_maxim(tz_save_info->data, hton16(*(uint16_t *)tz_save_info->size));
    app_mcast_send_data(tz_save_info, sizeof(clb_feature_save_info_t));
//    rt_kprintf("send feature to jlb: size= len=\n");
//    for (int i=0; i<hton16(*(uint16_t *)tz_save_info->size); i++)
//    {
//        if (i!=0 && i%32==0) rt_kprintf("\n");
//        rt_kprintf(" %02x", tz_save_info->data[i]);
//    }
//    rt_kprintf("\n");
    rt_thread_delay(1);
}
/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_diag_finish(diag_data_t **diag_data)
{
    for(uint8_t i = 0; i < AD7606_MAX; i++)
    {
        diag_data[i]->diag_state = false;
        diag_data[i]->acc_state = false;
    }
    // 延时300为前置器采集温度留空
    rt_thread_mdelay(1000);
//    if(false == app_qzq_can_env.com_state) {
//        app_can_notify_qzq_gather_temp(0);
//    }
    app_hw_timer4_start();
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_diag_thread_entry(void * paras)
{
    while(1)
    {
//        if (rt_sem_trytake(env.diag_sem) == RT_EOK)
        if(rt_sem_take(env.diag_sem, RT_WAITING_FOREVER) == RT_EOK)
        {
            /*   通知前置器采集温度  */
            if(false == app_qzq_can_env.com_state) {
                app_can_notify_qzq_gather_temp(1);
            }
            app_diag_raw_preproccess(env.diag_data);
            app_diag_raw_to_save(env.diag_data);
            app_diag_raw_to_acc(env.diag_data);
            app_diag_tread_axle(env.diag_data);
            app_diag_motor_bear(env.diag_data);
            app_diag_gear_bear(env.diag_data);
            app_diag_gear(env.diag_data);
//            app_diag_polygon(env.diag_data);
            app_diag_result_save(env.diag_data);
            app_diag_finish(env.diag_data);
        }
        // 喂狗
        app_feed_dogs();
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_diag_thread(void)
{
    if(env.dev_state == DEV_INIT_SUCCESS)
    {
        env.thread = rt_thread_create("app_diag", app_diag_thread_entry, RT_NULL, APP_DIAG_STACK, APP_DIAG_THREAD_PRO, 20);
        if(env.thread != RT_NULL) {
            rt_thread_startup(env.thread);
        }
        else {
            app_log_msg(LOG_LVL_ERROR, true, "app diag thread create failed");
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_diag_init(void)
{
    sys_paras_t *paras = app_paras_get();
    fft_diag_init();
    diag_algorithm_init();
    env.dev_state = DEV_INIT_NULL;
    for(uint8_t i = 0; i < AD7606_MAX; i++)
    {
        env.diag_data[i] = (diag_data_t *)rt_malloc(sizeof(diag_data_t));
        if(env.diag_data[i] != RT_NULL)
        {
            env.diag_data[i]->diag_state = false;
            env.diag_data[i]->raw_data = rt_malloc(AD7606_SF * sizeof(uint16_t));
            if(env.diag_data[i]->raw_data == RT_NULL)
            {
                env.dev_state = DEV_INIT_FAILED;
                app_log_msg(LOG_LVL_ERROR, true, "app diagnos raw data malloc failed");
                return ;
            }
            else {
                rt_memset(env.diag_data[i]->raw_data, 0, AD7606_SF * sizeof(uint16_t));
            }
            env.diag_data[i]->acc_data = rt_malloc(AD7606_SF * sizeof(float));
            if(env.diag_data[i]->acc_data == RT_NULL)
            {
                env.dev_state = DEV_INIT_FAILED;
                app_log_msg(LOG_LVL_ERROR, true, "app diagnos acc data malloc failed");
                return ;
            }
            else {
                rt_memset(env.diag_data[i]->acc_data, 0, AD7606_SF * sizeof(float));
            }
            env.diag_data[i]->acc_state = false;
        }
        else
        {
            env.dev_state = DEV_INIT_FAILED;
            app_log_msg(LOG_LVL_ERROR, true, "app diagnos data malloc failed");
            return ;
        }
    }
    env.diag_sem = rt_sem_create("diag_sem", 0, RT_IPC_FLAG_PRIO);
    if(env.diag_sem == RT_NULL)
    {
        env.dev_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "app diagnos sem failed");
        return ;
    }
    memset(&env.diag_msg, 0, sizeof(diag_msg_t));
    local.raw_data_save = rt_malloc(sizeof(clb_raw_save_info_t));
    if(local.raw_data_save == RT_NULL)
    {
        env.dev_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "app diagnos raw data malloc failed");
        return ;
    }
    else {
        rt_memset(local.raw_data_save, 0, sizeof(clb_raw_save_info_t));
    }
    local.tz_data_save = rt_malloc(sizeof(clb_feature_save_info_t));
    if(local.tz_data_save == RT_NULL)
    {
        env.dev_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "app diagnos tz data malloc failed");
        return ;
    }
    else {
        rt_memset(local.raw_data_save, 0, sizeof(clb_feature_save_info_t));
    }
    env.dev_state = DEV_INIT_SUCCESS;
    //初始化主机前置器个数
    if (paras->group_num == GROUP_NUM_4)
    {
        env.host_qzq_num = 3;
    }
    else {
        env.host_qzq_num = 2;
    }
    //初始化主从机标识
    if((paras->group_num > 0) && (paras->group_num <= GROUP_NUM_6) && \
            (paras->train_num > 0) && (paras->train_num <= GROUP_NUM_6))
    {
        if(((paras->group_num == GROUP_NUM_4) && (paras->train_num == GROUP_NUM_4)) || \
                ((paras->group_num == GROUP_NUM_6) && (paras->train_num == GROUP_NUM_6)) || \
                (paras->train_num == 1))
        {
            env.host_flag = 1;
        }
        else {
            env.host_flag = 0;
        }
    }

    app_log_msg(LOG_LVL_INFO, true, "app diagnos init success");
}







