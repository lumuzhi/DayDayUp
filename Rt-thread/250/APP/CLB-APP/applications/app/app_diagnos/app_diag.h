/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __APP_DIAG_H__
#define __APP_DIAG_H__

/* include */
#include "app_board.h"
#include "app_rtc.h"
#include "app_ad7606.h"
#include "app_adg1608.h"
#include "clb_protocol.h"
#include "head_protocol.h"
//算法
//#include "fft_diag.h"
//#include "initial_pro.h"
//#include "ZXZC_Wheel_diag.h"
//#include "DJ_diag.h"
//#include "CLX_diag.h"
//#include "gear_diag.h"
//#include "polygon_diag.h"
//#include "corrugation_diag.h"
//#include "IIR_polygon.h"
#include "app_diag_config.h"
#include "algor_interface.h"

/* macro */
#define APP_DIAG_THREAD_PRO      APP_BOARD_DIAG_THREAD_PRO
#define APP_DIAG_STACK           APP_BOARD_DIAG_STACK
#define APP_DIAG_TEMP_OFFSET     APP_BOARD_TEMP_OFFSET

#define Diag_Num_ZXZC_Wheel         5
#define Diag_Num_DJ                 8
#define Diag_Num_CLX                12
#define Diag_NH_OFFSET              0
#define Diag_WH_OFFSET              1
#define Diag_GDT_OFFSET             2
#define Diag_BCJ_OFFSET             3
#define Diag_Num_COMMON             4
#define Diag_TM_PFFSET              4
/*------------save tz data type----------------------------------*/
#define PROC_CHANNEL_NUM  16
#define QZQ_TOTAL         4
#define ZXZC_NUM          8
#define DJ_NUM            4
#define CXL_NUM           4



/* type declaration */

typedef struct FFT_DIAG_SAVE diag_save_t;
typedef struct
{
    bool            diag_state;
    clb_data_type_e data_type;
    uint16_t        count;
    uint8_t         agd1608_ch;
    uint16_t        speed;
    float           wheel;
    uint16_t        now_id;
    uint16_t        next_id;
    local_time_t    time_info;
    uint16_t        *raw_data;
    float           *acc_data;
    bool            acc_state;
    diag_save_t     diag_save;

//    bool           diag_state;
//    int            out_result;
//    int            tm_result;
//    polygon_save_t polygon_save;
}diag_data_t;
typedef enum
{
    DIAG_NORMAL,   //正常
    DIAG_WARN,     //预警
    DIAG_ALARM_1,  //报警1
    DIAG_ALARM_2,  //报警2
}diag_state_e;
typedef struct
{
    diag_state_e tread[ADG1608_CH_MAX];         //踏面
    diag_state_e axle_bearing[ADG1608_CH_MAX];  //轴箱轴承
    diag_state_e motor_bearing[ADG1608_CH_MAX]; //电机轴承
    diag_state_e box_gear[ADG1608_CH_MAX];      //齿轮箱齿轮
    diag_state_e box_bearing[ADG1608_CH_MAX];   //齿轮箱轴承
}diag_msg_t;
typedef struct
{
    dev_state_e dev_state;
    rt_thread_t thread;
    rt_sem_t    diag_sem;
    diag_data_t *diag_data[AD7606_MAX];
    diag_msg_t  diag_msg;
    uint8_t     host_qzq_num;
    uint8_t     host_flag;//主机=1， 从机=0
    feature_data_t   tz_data;
}app_diag_env_t;


/*
 *
 *
 * copy from udp_client.h
 *
 *
 */

/************copy from udp_client.h***************/



/* variable */
extern app_diag_env_t app_diag_env;

/* function */
void app_diag_init(void);
void app_diag_thread(void);

#endif /*__APP_DIAG_H__*/



