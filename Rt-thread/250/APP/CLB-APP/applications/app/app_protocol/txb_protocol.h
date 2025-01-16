/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __TXB_PROTOCOL_H__
#define __TXB_PROTOCOL_H__

/* include */
#include "app_board.h"

/* macro */

/* type declaration */
typedef enum
{
    TRAIN_HEAD,
    TRAIN_TAIL,

    TRAIN_MAX,
}train_e;
typedef union valid_flags
{
    struct
    {
        uint8_t time_valid  :1;//时间有效
        uint8_t time_set    :1;
        uint8_t wheel_valid :1;//轮径值有效
        uint8_t speed_valid :1;//速度有效
        uint8_t PA_valid    :1;
        uint8_t ATC_valid   :1;  //ATC报站信息有效
        uint8_t rcc         :2;
    }bits;
    uint8_t byte;//
}valid_flag_u;
typedef union
{
    struct
    {
        uint8_t v1_err:1;//
        uint8_t v2_err:1;//
        uint8_t trdp_err:1;//
        uint8_t mvb_err:1;
        uint8_t rcc:4;
    }bits;
    uint8_t byte;
}dev_st_t;
typedef union
{
    struct
    {
        uint8_t rcc:4;//
        uint8_t down:1;//列车下行
        uint8_t up:1;//列车上行
        uint8_t TC1_active:1;//TC1司机室激活
        uint8_t TC2_active:1;//TC1司机室激活
    }bits;
    uint8_t byte;
}train_state_t;
//通信板->记录板的public信息
typedef struct
{
    uint8_t version[2]; //上传给mvb的软件版本 byte8-9
    dev_st_t devs;           //通信板设备状态
    valid_flag_u valid_flag; //各种有效标志
    train_state_t train_state; //列车运行状态
    uint8_t cross_id;
    uint8_t year;
    uint8_t mon;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t train_line[2]; //线路号
    uint8_t train_num[2]; //列车号

    uint8_t wheel_1[2]; //1车轮径
    uint8_t wheel_2[2]; //2车轮径
    uint8_t wheel_3[2];
    uint8_t wheel_4[2];
    uint8_t wheel_5[2];
    uint8_t wheel_6[2];
    uint8_t wheel_7[2];
    uint8_t wheel_8[2];

    uint8_t speed[2];    //内部协议速度统一为 1=0.01km/h
    uint8_t start_id[2]; //起点站ID，当PA位置有效时，信息有效
    uint8_t curr_id[2];  //当前站ID，当PA位置有效时，信息有效
    uint8_t next_id[2];  //下一站ID，当PA位置有效时，信息有效
    uint8_t end_id[2];   //终点站ID，当PA位置有效时，信息有效
    uint8_t total_mileage[4]; //总里程
    uint8_t kilometer_post[2];//公里标

    uint8_t tc1_next_mileage[2];//tc1距离下一站距离 1=1m
    uint8_t tc2_next_mileage[2];//tc2距离下一站距离 1=1m
    uint8_t nc[12];
}ccu_public_t;
typedef struct
{
    uint8_t data_state;
    uint8_t train_num;
    uint8_t data[sizeof(ccu_public_t)];
}txb_mvb_data_t;
typedef struct
{
    uint8_t head[2];
    uint8_t size[2];   //除head、size、check外的字节数
    uint8_t check;
    uint8_t data[sizeof(txb_mvb_data_t)];
}txb_mvb_info_t;

/* variable */

/* function */

#endif /*__TXB_PROTOCOL_H__*/



