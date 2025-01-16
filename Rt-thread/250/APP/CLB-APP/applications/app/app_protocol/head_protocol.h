/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __HEAD_PROTOCOL_H__
#define __HEAD_PROTOCOL_H__

/* include */
#include "app_board.h"

/* macro */
/* 单播 */

/* 组播 */
//记录板相关协议头
#define HEAD_MCAST_RECORD_MSG_INFO               0xF101   //记录板状态信息，其他板卡根据该信息判断通信状态
#define HEAD_MCAST_RECORD_PORT_INFO              0xF102

//通信板相关协议头
#define HEAD_MCAST_TXB_MVB_INFO                  0xF201   //MVB信息

//处理板相关协议头
#define HEAD_MCAST_CLB_PORT_INFO                 0xF301   //TDS数据端口相关信息
#define HEAD_TCPS_CLB_PARAS_INFO                 0xF302
#define HEAD_CAN_CLB_QZQ_PARAS                   0xF303   //处理板到前置器参数
#define HEAD_CAN_CLB_QZQ_INFO                    0xF304
#define HEAD_MCAST_CLB_TEMP_RAW_DATA_SAVE        0xF305
#define HEAD_MCAST_CLB_VIB_RAW_DATA_SAVE         0xF306
#define HEAD_MCAST_CLB_FEATURE_DATA              0xF307   //特征数据
#define HEAD_MCAST_CLB_REALTIME_INFO             0xF308   //实时数据

//脱轨板1
#define HEAD_MCAST_TGB1_PORT_INFO                0xF401   //脱轨板自身状态信息
#define HEAD_MCAST_TGB1_ALARM_INFO               0xF402   //脱轨板故障上报及诊断结果上报

//脱轨板2
#define HEAD_MCAST_TGB2_PORT_INFO                0xF501

//转速总线板相关协议头
#define HEAD_MCAST_ZXB_MSG_INFO                  0xF601   //转速总线板相关信息，其他板卡根据该信息判断通信状态

//前置器升级
#define HEAD_CAN_QZQ1_UPDATE_INFO                0xAF01
#define HEAD_CAN_QZQ2_UPDATE_INFO                0xAF02
#define HEAD_CAN_QZQ3_UPDATE_INFO                0xAF03
#define HEAD_CAN_QZQ4_UPDATE_INFO                0xAF04

/* type declaration */

/* variable */

/* function */

#endif /*__HEAD_PROTOCOL_H__*/



