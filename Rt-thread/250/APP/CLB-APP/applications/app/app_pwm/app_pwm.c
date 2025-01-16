/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-18     plum       the first version
 */
#include "app_pwm.h"

#define PWM_DEV_NAME        "pwm1"  /* PWM设备名称 */
#define PWM_DEV_CHANNEL     1       /* PWM通道 */

struct rt_device_pwm *pwm_dev;


void app_pwm_init()
{
    rt_uint32_t period, pulse;
    rt_err_t ret;

    period = 25000;    /* pwm频率为80Khz */
    pulse = 12500;          /* PWM脉冲宽度值，单位为纳秒ns */

    /* 查找设备 */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("pwm run failed! can't find %s device!\n", PWM_DEV_NAME);
        return ;
    }

    /* 设置PWM周期和脉冲宽度默认值 */
    ret = rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);
    if(ret != RT_EOK)
    {
        rt_kprintf("pwm rt_pwm_set result: %d!\n", ret);
    }
    /* 使能设备 */
    ret = rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
    if(ret != RT_EOK)
    {
        rt_kprintf("pwm rt_pwm_enable result: %d!\n", ret);
    }
}
