/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/
/* STM32F407VGT6主要验证的项点
 * 1、W25QXXX 使用文件系统  */

#ifndef __APP_BOARD_H__
#define __APP_BOARD_H__

/* include */
#include <rtdevice.h>
#include <rtthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <rtdbg.h>
#include <stddef.h>
#include <float.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netdev_ipaddr.h>
#include <netdev.h>
#include <dfs_posix.h>
#include <dfs_fs.h>

#ifdef RT_USING_FINSH
#include <finsh.h>
#endif
#include "app_utility.h"
#include "bsp_board.h"

/* macro */
#define APP_BOARD_SYS_LOG_STACK             17408  //日志保存线程堆栈大小,使用nandflash存储日志比较耗内存
#define APP_BOARD_CAN1_RX_STACK             2048   //CAN线程堆栈大小
#define APP_BOARD_CAN1_DEAL_STACK           2048
#define APP_BOARD_CAN1_TX_STACK             2048
#define APP_BOARD_CAN2_RX_STACK             2048   //CAN线程堆栈大小
#define APP_BOARD_CAN2_DEAL_STACK           2048
#define APP_BOARD_DIAG_STACK                13312
#define APP_BOARD_CAN2_TX_STACK             2048
#define APP_BOARD_PERIOD_STACK              2048
#define APP_BOARD_QZQ_STACK                 2816
#define APP_BOARD_SPI1_SEND_STACK           1536
#define APP_BOARD_SPI1_RECV_STACK           1536
#define APP_BOARD_SPI2_RECV_STACK           1536
#define APP_BOARD_MULT_RX_STACK             2048   //组播接收线程堆栈大小
#define APP_BOARD_UART1_RX_STACK            1024   //串口1接收线程堆栈大小
#define APP_BOARD_PARAS_STACK               2048   //参数更新线程堆栈大小
#define APP_BOARD_TCPS_STACK                2048   //TCP服务端线程堆栈大小
#define APP_BOARD_UDP_STACK                 2048

//数值越小表示优先级越高
#define APP_BOARD_SYS_LOG_THREAD_PRO        24     //日志保存线程优先级
#define APP_BOARD_CAN1_RX_THREAD_PRO        15     //CAN线程优先级
#define APP_BOARD_CAN1_DEAL_THREAD_PRO      16
#define APP_BOARD_CAN1_TX_THREAD_PRO        17
#define APP_BOARD_CAN2_RX_THREAD_PRO        15     //CAN线程优先级
#define APP_BOARD_CAN2_DEAL_THREAD_PRO      16
#define APP_BOARD_DIAG_THREAD_PRO           16
#define APP_BOARD_CAN2_TX_THREAD_PRO        17
#define APP_BOARD_PERIOD_THREAD_PRO         26
#define APP_BOARD_QZQ_THREAD_PRO            18
#define APP_BOARD_MULT_RX_THREAD_PRO        20     //组播接收线程优先级
#define APP_BOARD_UART1_RX_THREAD_PRO       25     //串口1接收线程优先级
#define APP_BOARD_PARAS_THREAD_PRO          27     //参数更新线程优先级
#define APP_BOARD_TCPS_THREAD_PRO           15     //TCP服务端线程优先级
#define APP_BOARD_UDP_THREAD_PRO            21

#define APP_BOARD_TEMP_OFFSET               0      //用于配置当前项目上屏的温度偏移
/* type declaration */
typedef enum
{
    DEV_INIT_NULL,
    DEV_INIT_SUCCESS,
    DEV_INIT_FAILED,
    DEV_OPERATE_ERROR,
}dev_state_e;

#endif /*__APP_BOARD_H__*/

