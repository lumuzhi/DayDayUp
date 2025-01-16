/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __BSP_BOARD_H__
#define __BSP_BOARD_H__

/* include */
#include <rtthread.h>
#include <rtdevice.h>
#include <drv_common.h>
#include <rtdbg.h>
#include <stm32f4xx.h>
#include <time.h>
#include "bsp_fsmc.h"
#include "app_utility.h"

/* macro */

#define DEF_BSP_SAMPLE_TIMER_FRE           512                           //Timer frequency 采样定时器频率
#define DEF_BSP_STABLE_SAMPLE_FRE_HZ       DEF_BSP_SAMPLE_TIMER_FRE
#define DEF_BSP_INSTAB_SAMPLE_FRE_HZ       DEF_BSP_SAMPLE_TIMER_FRE / 2
#define DEF_BSP_STABLE_CH_MAX              6                             //平稳
#define DEF_BSP_STABLE_CH_USE              6
#define DEF_BSP_INSTAB_CH_MAX              4                             //失稳
#define DEF_BSP_INSTAB_CH_USE              2

#define DEF_BSP_ETH_MDC_PORT                GPIOC
#define DEF_BSP_ETH_MDC_PIN                 GPIO_PIN_1
#define DEF_BSP_ETH_REF_CLK_PORT            GPIOA
#define DEF_BSP_ETH_REF_CLK_PIN             GPIO_PIN_1
#define DEF_BSP_ETH_MDIO_PORT               GPIOA
#define DEF_BSP_ETH_MDIO_PIN                GPIO_PIN_2
#define DEF_BSP_ETH_CRS_DV_PORT             GPIOA
#define DEF_BSP_ETH_CRS_DV_PIN              GPIO_PIN_7
#define DEF_BSP_ETH_RXD0_PORT               GPIOC
#define DEF_BSP_ETH_RXD0_PIN                GPIO_PIN_4
#define DEF_BSP_ETH_RXD1_PORT               GPIOC
#define DEF_BSP_ETH_RXD1_PIN                GPIO_PIN_5
#define DEF_BSP_ETH_TX_EN_PORT              GPIOG
#define DEF_BSP_ETH_TX_EN_PIN               GPIO_PIN_11
#define DEF_BSP_ETH_TXD0_PORT               GPIOG
#define DEF_BSP_ETH_TXD0_PIN                GPIO_PIN_13
#define DEF_BSP_ETH_TXD1_PORT               GPIOG
#define DEF_BSP_ETH_TXD1_PIN                GPIO_PIN_14
#define DEF_BSP_ETH_RESET_PORT              GPIOA
#define DEF_BSP_ETH_RESET_PIN               GPIO_PIN_0

/* type declaration */
typedef enum
{
    BSP_INIT_NULL,
    BSP_INIT_OK,
    BSP_INIT_FAIL,
}bsp_state_e;
typedef enum
{
    START_DIAG_EVENT    = (1 << 0),
    RAW_DATA_SAVE_EVENT = (1 << 1),  //原始数据保存事件
}bsp_event_e;

#endif /*__BSP_BOARD_H__*/


