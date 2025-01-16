/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_AD7606_H__
#define __APP_AD7606_H__

/* include */
#include "app_board.h"
#include "bsp_ad7606.h"

/* macro */
#define AD7606_SF                  4096   //采样频率4KHz
#define ad7606_convst(ctrl)        bsp_ad7606_convst_##ctrl()

/* type declaration */
typedef enum
{
    GATHER_SUCCESS,  //采集成功
    GATHER_FAILED,   //采集失败
}ad_gather_state_e;
typedef enum
{
    AD7606_V1,
    AD7606_V2,
    AD7606_V3,
    AD7606_V4,
    AD7606_V5,
    AD7606_V6,

    AD7606_MAX,
}ad7606_ch_e;
typedef struct
{
    uint16_t *ch_data;
}ad7606_data_t;
typedef struct
{
    dev_state_e       ad7606_state;
    ad_gather_state_e ad_gather_state;   //AD采集状态
    ad7606_data_t     ad7606_data[AD7606_MAX];

    // add for linwanrong
    uint8_t             is_realtime; // ptu是否查看实时振动波形
    uint8_t             is_realtimeType; // 实时振动的波形类型
}app_ad7606_env_t;

/* variable */
extern app_ad7606_env_t app_ad7606_env;

/* function */
void app_ad7606_init(void);
void app_ad7606_read_adc(void);
void app_ad7606_for_ptu(uint8_t *data, uint16_t size);

#endif /*__APP_AD7606_H__*/
