/**
 * @file
 * @details
 * @author  rentianwen
 * @date
 * @version
**/

/* include */
#include "app_update.h"
#include "app_hw_timer.h"
#include "app_adg1608.h"
#include "app_ad7606.h"
#include "app_easyflash.h"
#include "app_paras.h"
#include "app_led.h"
#include "app_net.h"
#include "app_can.h"
#include "app_diag.h"
#include "app_sdio.h"
#include "app_qzq.h"
#include "app_log.h"
#include "app_period.h"
#include "app_iwdg.h"
#include "app_qzq_can.h"
#include "app_pwm.h"


/*
 *  2024-11-26-linwanrong
 *  1.修改看门狗，不在中断触发，改线程
 *  2.偏置电压连续20次不正常才上报
 *  3.修改前置器上报故障时间为1分钟
 *
 *  2025-01-03 linwanrong
 *  1.新增ptu实时波形，指定某个通道
 *
 *
 *
 */






/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note   main function
**/
int main(void)
{
    rt_kprintf("    -> TFDS-JNL8-CLB  update time :2025-01-03 09:00\n");
    /* init source */
//    app_easyflash_init();
    app_log_init();
    app_paras_init();
//    app_sdio_init();
//    app_led_init();
    app_net_init();
//    app_can_init();
//    app_qzq_can_init();  //使用新版底包这个文件相关的所有都可以删除
    app_update_init();
    app_hw_timer_init();
//    app_adg1608_init();
//    app_pwm_init();
//    app_ad7606_init();
//    app_diag_init();
//    app_qzq_init();
//    app_period_init();
//    app_iwdg_init(5);
    /* running thread */
//    app_diag_thread();
//    app_can_thread();
//    app_qzq_thread();
    app_net_thread();
//    app_period_thread();

    return RT_EOK;
}



