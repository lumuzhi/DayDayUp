/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_ad7606.h"
#include "bsp_spi.h"
#include "app_adg1608.h"
#include "app_diag.h"
#include "app_rtc.h"
#include "app_period.h"
#include "app_log.h"
#include "app_hw_timer.h"
#include "head_protocol.h"
#include "ptu_protocol.h"
#include "app_paras.h"

/* macro */
#define app_ad7606_result()         *(__IO uint16_t *)0x6C000000
#define AD7606_FILTER_SIZE          512
#define AD7606_CH_BUFF_SIZE_MAX     4096

/* type declaration */
typedef struct
{
    uint16_t count;
}app_ad7606_local_t;

/* variable */
app_ad7606_env_t app_ad7606_env;
#define env app_ad7606_env
app_ad7606_local_t app_ad7606_local;
#define local app_ad7606_local

/* function declaration */
/**
 * @brief   ptu要查看的指定类别波形，不进行通道切换
 * @param
 * @return
 * @note
**/
void app_ad7606_for_ptu(uint8_t *data, uint16_t size)
{
    if((data == RT_NULL) || (size == 0) || (*(uint16_t *)data != hton16(APP_PTU_HEAD))) {
        return ;
    }
    if(CheckSum16(data, size - 2) == DATA16(data[size - 2], data[size - 1])) {
        sys_paras_t *paras = app_paras_get();
        ptu_monitor_protocol_t *p = (ptu_monitor_protocol_t *)data;
        switch (p->cmd) {
            case 0x60:
                // 不是发给本车的命令
                if(paras->train_num == p->data_info[1]) {
                    rt_kprintf("train_num = %d is_realtime = %d type = %d\n", p->data_info[1], p->data_info[0], p->data_info[2]);
                    env.is_realtime     = p->data_info[0];
                    env.is_realtimeType = (p->data_info[2] > 8) ? (p->data_info[2] - 8) : p->data_info[2];
                }
                break;
            default:
                break;
        }
    }
}
/**
 * @brief  将采集到的电信号做原、补码转换
 * @param  data 原始电信号采集值
 * @return 补码转换后的值
 * @note
**/
static uint16_t complement_to_source(uint16_t data)
{
   if(data >= 32768) {
       return (data - 32768);
    }
   else {
      return (data + 32768);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_ad7606_read_adc(void)
{
    static uint16_t read_cnts = 0, count = 0, ad_valid_count = 0;
    static bool ad_value_valid = true;
    uint32_t adc_cnts = 0;
    ad7606_convst(high);
    while(bsp_ad7606_get_busy()) //busy = 1 表示正在转换
    {
        adc_cnts++;  //正常情况下转换完成adc_cnts=308
        if((adc_cnts >= 1024) && (env.ad_gather_state == GATHER_SUCCESS))
        {
            env.ad_gather_state = GATHER_FAILED;
            app_log_msg(LOG_LVL_ERROR, true, "app ad7606 read adc error");
            break;
        }
    }
    if(adc_cnts < 1024)
    {
        if(env.ad_gather_state == GATHER_FAILED) {
            app_log_msg(LOG_LVL_INFO, true, "app ad7606 read adc get right");
        }
        env.ad_gather_state = GATHER_SUCCESS;
    }
    ad7606_convst(low);
    if((env.ad7606_state == DEV_INIT_SUCCESS) && (env.ad_gather_state == GATHER_SUCCESS))
    {
        if(ad_value_valid)
        {
            for(uint8_t i = 0; i < AD7606_MAX; i++)
            {
                if(read_cnts < AD7606_CH_BUFF_SIZE_MAX) {
                    env.ad7606_data[i].ch_data[read_cnts] = complement_to_source(app_ad7606_result());
                }
            }
            read_cnts++;
            if(read_cnts >= AD7606_CH_BUFF_SIZE_MAX)
            {
                uint8_t diag_count = 0;
                for(uint8_t i = 0; i < AD7606_MAX; i++)
                {
                    if(app_diag_env.diag_data[i]->diag_state == false) {
                        diag_count++;
                    }
                }
                if((diag_count == AD7606_MAX) && (app_diag_env.dev_state == DEV_INIT_SUCCESS))
                {
                    for(uint8_t i = 0; i < AD7606_MAX; i++)
                    {
                        app_diag_env.diag_data[i]->count = count++;
                        app_diag_env.diag_data[i]->agd1608_ch = app_adg1608_env.current_ch;
                        app_diag_env.diag_data[i]->speed = app_period_env.clb_info.speed;
                        app_diag_env.diag_data[i]->wheel = (float)(app_period_env.clb_info.wheel) / 1000.0f; //算法中时缩小了1000倍
                        app_diag_env.diag_data[i]->now_id = app_period_env.clb_info.now_id;
                        app_diag_env.diag_data[i]->next_id = app_period_env.clb_info.next_id;
                        app_rtc_get(&app_diag_env.diag_data[i]->time_info);
                        rt_memcpy(app_diag_env.diag_data[i]->raw_data, env.ad7606_data[i].ch_data, AD7606_SF * sizeof(uint16_t));
                        app_diag_env.diag_data[i]->diag_state = true;
                    }

                    switch (env.is_realtime) {
                        case 1:
                            if(env.is_realtimeType)
                                app_adg1608_env.current_ch = env.is_realtimeType - 1;
                            break;
                        default:
                            app_adg1608_env.current_ch++;
                            if(app_adg1608_env.current_ch >= ADG1608_CH_MAX) {
                                app_adg1608_env.current_ch = ADG1608_CH_1;
                            }
                            break;
                    }
//                    rt_kprintf("app_adg1608_env.current_ch = %d\n", app_adg1608_env.current_ch);
                    app_adg1608_1_switch(app_adg1608_env.current_ch);
                    app_adg1608_2_switch(app_adg1608_env.current_ch);
                    app_hw_timer4_stop();  //数据采集完成关闭定时器中断，4K的采样率,244us进入中断资源消耗太大
                    rt_sem_release(app_diag_env.diag_sem);
                }
                read_cnts = 0;
                ad_value_valid = false;
            }
        }
        else
        {
            ad_valid_count++;
            if(ad_valid_count >= AD7606_FILTER_SIZE)
            {
                ad_valid_count = 0;
                ad_value_valid = true;
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
void app_ad7606_init(void)
{
    env.ad7606_state = DEV_INIT_NULL;
    if(bsp_fsmc_env.ad7606_state != BSP_FSMC_INIT_OK)
    {
        env.ad7606_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "bsp ad7606 init failed");
        return ;
    }
    for(uint8_t i = 0; i < AD7606_MAX; i++)
    {
        env.ad7606_data[i].ch_data = rt_malloc(AD7606_SF * sizeof(uint16_t));
        if(env.ad7606_data[i].ch_data == RT_NULL)
        {
            env.ad7606_state = DEV_INIT_FAILED;
            app_log_msg(LOG_LVL_ERROR, true, "app ad7606 ch data malloc failed");
            return ;
        }
    }
    env.ad_gather_state = GATHER_SUCCESS;
    env.ad7606_state = DEV_INIT_SUCCESS;
    app_log_msg(LOG_LVL_INFO, true, "app ad7606 init success");
}














