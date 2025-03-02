/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-02-26     RT-Thread    first version
 */

#include <rtthread.h>

#include "app_easyflash.h"
#include "app_adc.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int main(void)
{
    app_easyflash_init();
    app_adc_init();

//    app_adc_thread();

    return RT_EOK;
}
