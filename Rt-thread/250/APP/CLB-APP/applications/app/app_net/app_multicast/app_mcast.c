/**
 * @file
 * @details
 * @author
 * @date    组播
 * @version
**/

/* include */
#include "app_mcast.h"
#include "app_paras.h"
#include "app_log.h"
#include "app_period.h"
#include "app_net.h"
#include "app_sys_msg.h"
#include "head_protocol.h"
#include "paras_protocol.h"
#include "app_ad7606.h"

/* macro */
#define APP_MCAST_RECV_BUFF_MAX     1024

/* type declaration */
typedef struct
{
    struct sockaddr_in tx_addr;
    struct sockaddr_in raw_tx_addr;
    uint8_t            rx_buff[APP_MCAST_RECV_BUFF_MAX];
}app_mcast_local_t;

/* variable declaration */
app_mcast_env_t app_mcast_env;
#define env app_mcast_env
app_mcast_local_t app_mcast_local;
#define local app_mcast_local

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_mcast_send_data(void *data, rt_uint16_t size)
{
    if((data == NULL) || (size == 0) || (env.dev_state != DEV_INIT_SUCCESS)) {
        return ;
    }
    sendto(env.sock_handle, data, size, 0, (struct sockaddr *)&local.tx_addr, sizeof(struct sockaddr_in));
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_mcast_send_raw_data(void *data, rt_uint16_t size)
{
    if((data == NULL) || (size == 0) || (env.dev_state != DEV_INIT_SUCCESS)) {
        return ;
    }
    sendto(env.sock_handle, data, size, 0, (struct sockaddr *)&local.raw_tx_addr, sizeof(struct sockaddr_in));
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_mcast_recv_entry(void *params)
{
    rt_uint32_t recv_size = 0;
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    while(1)
    {
        recv_size = recvfrom(env.sock_handle, local.rx_buff, APP_MCAST_RECV_BUFF_MAX, 0, (struct sockaddr *)&recv_addr, &addr_len);
        if(recv_size)
        {
            app_net_env.net_size = recv_size;
            if((recv_size == sizeof(txb_mvb_info_t)) && (*(uint16_t *)local.rx_buff == HEAD_MCAST_TXB_MVB_INFO))
            {
                txb_mvb_info_t *txb_mvb_info = (txb_mvb_info_t *)local.rx_buff;
                txb_mvb_data_t *txb_mvb_data = (txb_mvb_data_t *)txb_mvb_info->data;
                if(txb_mvb_info->check == crc8_maxim(txb_mvb_info->data, hton16(*(uint16_t *)txb_mvb_info->size)))
                {
                    uint8_t recv_ip[4];
                    uint8_t trdp_dev = 0;
                    memcpy(recv_ip, &(recv_addr.sin_addr.s_addr), 4);
                    if(txb_mvb_data->train_num == 1)
                    {
                        trdp_dev = (recv_ip[3] == 12) ? 1 : 0;
                        env.txb_info.mvb_info[TRAIN_HEAD][trdp_dev].recv_count += 1;
                        env.txb_info.mvb_info[TRAIN_HEAD][trdp_dev].data_state = txb_mvb_data->data_state;
                        env.txb_info.mvb_info[TRAIN_HEAD][trdp_dev].train_num = txb_mvb_data->train_num;
                        memcpy(&env.txb_info.mvb_info[TRAIN_HEAD][trdp_dev].data, txb_mvb_data->data, sizeof(ccu_public_t));
                    }
                    else
                    {
                        trdp_dev = (recv_ip[3] == 12) ? 1 : 0;
                        env.txb_info.mvb_info[TRAIN_TAIL][trdp_dev].recv_count += 1;
                        env.txb_info.mvb_info[TRAIN_TAIL][trdp_dev].data_state = txb_mvb_data->data_state;
                        env.txb_info.mvb_info[TRAIN_TAIL][trdp_dev].train_num = txb_mvb_data->train_num;
                        memcpy(&env.txb_info.mvb_info[TRAIN_TAIL][trdp_dev].data, txb_mvb_data->data, sizeof(ccu_public_t));
                    }
                }
            }
            /*
                         *  该判断为济南8特有，PTU需要实时查看某类振动波形下发，由记录板组播转发
             */
            else if((recv_size == 32) && (*(uint16_t *)local.rx_buff == HEAD_MCAST_CLB_REALTIME_INFO)) {
                app_ad7606_for_ptu(&local.rx_buff[2], recv_size - 2);
            }
            else
            {
                app_paras_config(local.rx_buff, recv_size);
                app_paras_qzq_config(local.rx_buff, recv_size);
                sys_msg_local_info(local.rx_buff, recv_size);
                sys_msg_qzq_info(local.rx_buff, recv_size);
            }
            recv_size = 0;
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_mcast_recv_thread(void)
{
    if(env.dev_state == DEV_INIT_SUCCESS)
    {
        env.recv_thread = rt_thread_create("mcast_recv", app_mcast_recv_entry, RT_NULL,
                                         APP_MULT_RECV_STACK, APP_MULT_RECV_THREAD_PRO, 20);
        if(env.recv_thread != RT_NULL) {
            rt_thread_startup(env.recv_thread);
        }
        else {
            app_log_msg(LOG_LVL_ERROR, true, "multicast recv thread create failed");
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_mcast_thread(void)
{
    app_mcast_recv_thread();
}

 /**
  * @brief
  * @param
  * @return
  * @note
 **/
 void app_mcast_init(void)
 {
      char mcast_ip[15];
      char mcast_raw_ip[15];
      struct sockaddr_in local_addr;
      struct ip_mreq multi_mreq;
      sys_paras_t *paras = app_paras_get();

      //设置原始数据发送的组播IP及端口
      sprintf(mcast_raw_ip, "%d.%d.%d.%d", paras->mcast_raw_ip[0], paras->mcast_raw_ip[1],paras->mcast_raw_ip[2],paras->mcast_raw_ip[3]);
      local.raw_tx_addr.sin_family = AF_INET;
      local.raw_tx_addr.sin_addr.s_addr = inet_addr(mcast_raw_ip);
      local.raw_tx_addr.sin_port = htons(DATA16(paras->mcast_raw_port[0], paras->mcast_raw_port[1]));

      env.dev_state = DEV_INIT_NULL;
      sprintf(mcast_ip, "%d.%d.%d.%d", paras->mcast_ip[0], paras->mcast_ip[1],paras->mcast_ip[2],paras->mcast_ip[3]);
      local.tx_addr.sin_family = AF_INET;
      local.tx_addr.sin_addr.s_addr = inet_addr(mcast_ip);
      local.tx_addr.sin_port = htons(DATA16(paras->mcast_port[0], paras->mcast_port[1]));
      if((env.sock_handle = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
      {
          env.dev_state = DEV_INIT_FAILED;
          app_log_msg(LOG_LVL_ERROR, true, "multicast receive socket failed");
          return ;
      }
      int yes = 1;
      if(setsockopt(env.sock_handle, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
      {
          env.dev_state = DEV_INIT_FAILED;
          app_log_msg(LOG_LVL_ERROR, true, "multicast reusing addr failed");
          return ;
      }
      rt_memset(&local_addr, 0, sizeof(struct sockaddr_in));
      local_addr.sin_family = AF_INET;
      local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      local_addr.sin_port = ntohs(DATA16(paras->mcast_port[0], paras->mcast_port[1]));
      if(bind(env.sock_handle, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in)) == -1)
      {
          env.dev_state = DEV_INIT_FAILED;
          app_log_msg(LOG_LVL_ERROR, true, "bind multicast failed");
          return ;
      }
      sprintf(mcast_ip, "%d.%d.%d.%d", paras->mcast_ip[0], paras->mcast_ip[1],paras->mcast_ip[2],paras->mcast_ip[3]);
      multi_mreq.imr_multiaddr.s_addr = inet_addr(mcast_ip);
      multi_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
      if(setsockopt(env.sock_handle, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multi_mreq, sizeof(struct ip_mreq)) < 0)
      {
          env.dev_state = DEV_INIT_FAILED;
          app_log_msg(LOG_LVL_ERROR, true, "setsockopt failed");
          return ;
      }
      env.dev_state = DEV_INIT_SUCCESS;
      app_log_msg(LOG_LVL_INFO, true, "multicast init success");
 }







