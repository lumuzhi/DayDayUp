#ifndef __TRDP_PROTOCOL_H__
#define __TRDP_PROTOCOL_H__

#include "app_board.h"

/* type declaration */
typedef enum {
    TRDP_JLB,
    TRDP_TXB,

    TRDP_DEV_MAX,
}trdp_dev_e;
typedef struct
{
    uint8_t head[2];             //协议头为：FA 01
    uint8_t local_ip[4];
    uint8_t netmask[4];
    uint8_t gateway[4];
    uint8_t mult_tx_ip[4];       //组播发送地址
    uint8_t mult_tx_port[2];
    uint8_t mult_tx_comid1[2];   //1车COMID
    uint8_t mult_tx_comid2[2];   //尾车COMID
    uint8_t mult_rx_ip[4];       //组播接收地址
    uint8_t mult_rx_port[2];
    uint8_t mult_rx_comid1[2];
    uint8_t checksum;
}paras_config_t;

typedef struct
{
    uint8_t sequenceCounter[4];  //message sequence counter
    uint8_t protocolVersion[2];  //protocol version
    uint8_t msgType[2];          //type of the message
    uint8_t comId[4];            //communication identifier
    uint8_t etbTopoCnt[4];       //etbTopoCnt value
    uint8_t opTrnTopoCnt[4];     //opTrnTopoCnt value
    uint8_t datasetLength[4];    //length of the array ‘dataset’
    uint8_t reserved01[4];       //reserved for future use (= 0)
    uint8_t replyComId[4];       //requested communication identifier
    uint8_t replyIpAddress[4];   //reply IP address
    uint8_t headerFcs[4];        //header checksum
}trdp_head_t;


typedef struct {
    uint8_t trdp_ip[4];
    uint8_t netmask[4];
    uint8_t gateway[4];
    uint8_t mult_tx_ip[4];       //组播发送地址
    uint8_t mult_tx_port[2];
    uint8_t mult_tx_comid1[2];   //1车COMID
    uint8_t mult_tx_comid2[2];   //尾车COMID
    uint8_t mult_rx_ip[4];       //组播接收地址
    uint8_t mult_rx_port[2];
    uint8_t mult_rx_comid1[2];
}trdp_net_config_protocol_t;

typedef struct {
    uint8_t board_type; // 板卡类型 只有记录板和通信版有trdp
    uint8_t data[sizeof(trdp_net_config_protocol_t)];
}trdp_net_config_data_t;

typedef struct {
    uint8_t head[2];
    uint8_t size[2];
    uint8_t check;
    uint8_t data[sizeof(trdp_net_config_data_t)];
}trdp_net_config_info_t;

#endif
