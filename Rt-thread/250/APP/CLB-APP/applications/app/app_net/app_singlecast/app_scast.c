/**
 * @file
 * @details
 * @author
 * @date    单播包括：TCP Server，TCP Client，UDP
 * @version
**/

/* include */
#include "app_scast.h"
#include "app_log.h"
#include "app_update.h"
#include "app_paras.h"
#include "app_net.h"
#include "app_utility.h"
#include "app_diag.h"

/* macro */
#define APP_UDP_RECV_MAX         1024

/* type declaration */
typedef struct
{
    int     tcps_socket;
    uint8_t tcps_total_connect;
    uint8_t tcps_id;
    uint8_t udp_recv_data[APP_UDP_RECV_MAX];
}app_scast_local_t;

/* variable declaration */
app_scast_local_t app_scast_local;
#define local app_scast_local
app_scast_env_t app_scast_env;
#define env app_scast_env

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_tcps_send_data(uint8_t *data, uint16_t size)
{
    if((data == RT_NULL) || (size == 0) || (env.tcps_dev_state != DEV_INIT_SUCCESS)) {
        return ;
    }
    for(uint8_t i = 0; i < APP_TCPS_CONNECT_MAX; i++)
    {
        if(env.tcps_info[i].connect != -1)
        {
            rt_kprintf("tcps send size : %d \n", size);
            send(env.tcps_info[i].connect, data, size, 0);
//            if(res == 10)
//            {
//                uint16_t pkg_num = ntoh16(*((uint16_t *)&data[6]));
//                rt_kprintf("pkg_num = %u \n",pkg_num);
//            }
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_tcps_handler(void *parameter)
{
    char log_data[256] = { 0 };
    rt_uint8_t id = *(rt_uint8_t *)parameter;
    int recv_size = 0;
    struct timeval client_timeout;
    client_timeout.tv_sec = 0;
    client_timeout.tv_usec = 1000 * 50;
    while(1)
    {
        FD_ZERO(&env.tcps_info[id].readset_client);
        FD_SET(env.tcps_info[id].connect, &env.tcps_info[id].readset_client);
        if(select(env.tcps_info[id].connect + 1, &env.tcps_info[id].readset_client, RT_NULL, RT_NULL, &client_timeout) == 0)
        {
            rt_thread_delay(10);
            continue;
        }
        recv_size = recv(env.tcps_info[id].connect, env.tcps_info[id].recv_data, APP_TCPS_RECV_MAX, 0);
        if(recv_size > 0)
        {
            app_net_env.net_size = recv_size;
            app_update_read_data(env.tcps_info[id].recv_data, recv_size);
            app_log_read_data(env.tcps_info[id].recv_data, recv_size);
//            app_log_info(env.tcps_info[id].recv_data, recv_size);
//            rt_kprintf("recv_packet: %d %d %d %d\n", env.tcps_info[id].recv_data[14], env.tcps_info[id].recv_data[15], env.tcps_info[id].recv_data[16], env.tcps_info[id].recv_data[17]);
        }
        else
        {
            closesocket(env.tcps_info[id].connect);
            env.tcps_info[id].connect = -1;
            local.tcps_total_connect--;
            memset(log_data, 0, sizeof(log_data));
            sprintf(log_data, "tcps close connection (%s , %d)", inet_ntoa(env.tcps_info[id].client_addr.sin_addr), \
                    ntohs(env.tcps_info[id].client_addr.sin_port));
            app_log_msg(LOG_LVL_INFO, true, log_data);
            break;
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_tcps_thread_entry(void * parameter)
{
    char log_data[256] = { 0 };
    struct sockaddr_in server_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    if((local.tcps_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        app_log_msg(LOG_LVL_ERROR, true, "tcps socket error");
        goto __exit;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(APP_TCPS_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
    if(bind(local.tcps_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        app_log_msg(LOG_LVL_ERROR, true, "tcps unable to bind");
        goto __exit;
    }
    if(listen(local.tcps_socket, APP_TCPS_CONNECT_MAX) == -1)
    {
        app_log_msg(LOG_LVL_ERROR, true, "tcps listen error");
        goto __exit;
    }
    for(uint8_t i = 0; i < APP_TCPS_CONNECT_MAX; i++) {
        env.tcps_info[i].connect = -1;
    }
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    local.tcps_total_connect = 0;
    local.tcps_id = 0;
    while(1)
    {
        if(local.tcps_total_connect < APP_TCPS_CONNECT_MAX)
        {
            while(env.tcps_info[local.tcps_id].connect != -1) {
                local.tcps_id = (local.tcps_id + 1) % APP_TCPS_CONNECT_MAX;
            }
            FD_ZERO(&env.tcps_info[local.tcps_id].readset);
            FD_SET(local.tcps_socket, &env.tcps_info[local.tcps_id].readset);
            if(select(local.tcps_socket + 1, &env.tcps_info[local.tcps_id].readset, RT_NULL, RT_NULL, &timeout) == 0)
            {
                rt_thread_delay(10);
                continue;
            }
            env.tcps_info[local.tcps_id].connect = accept(local.tcps_socket, (struct sockaddr *)&env.tcps_info[local.tcps_id].client_addr, &sin_size);
            /* 返回的是连接成功的socket */
            if(env.tcps_info[local.tcps_id].connect < 0)
            {
                rt_kprintf("accept connection failed! errno = %d", errno);
                continue;
            }
            rt_kprintf("accept successful!!! \n");
            /* 接受返回的client_addr指向了客户端的地址信息 */
            memset(log_data, 0, sizeof(log_data));
            sprintf(log_data, "tcps get a connection from (%s , %d)", inet_ntoa(env.tcps_info[local.tcps_id].client_addr.sin_addr), \
                    ntohs(env.tcps_info[local.tcps_id].client_addr.sin_port));
            app_log_msg(LOG_LVL_INFO, true, log_data);
            char thread_name[8] = { 0 };
            sprintf(thread_name,"tcps_%d", local.tcps_id);
            rt_thread_t thread = rt_thread_create(thread_name, app_tcps_handler, &local.tcps_id, APP_TCPS_STACK, APP_TCPS_THREAD_PRO - 1, 20);
            if(thread != RT_NULL) {
                rt_thread_startup(thread);
            }
            else {
                app_log_msg(LOG_LVL_ERROR, true, "tcps client thread create fail");
            }
            local.tcps_id = (local.tcps_id + 1) % APP_TCPS_CONNECT_MAX;
            local.tcps_total_connect++;
        }
        else {
            rt_thread_delay(100);
        }
    }
    __exit:
        return;
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_tcps_thread(void)
{
    if(env.tcps_dev_state == DEV_INIT_SUCCESS)
    {
        env.tcps_thread = rt_thread_create("tcps", app_tcps_thread_entry, \
                RT_NULL, APP_TCPS_STACK, APP_TCPS_THREAD_PRO, 20);
        if(env.tcps_thread != RT_NULL) {
            rt_thread_startup(env.tcps_thread);
        }
        else {
            app_log_msg(LOG_LVL_ERROR, true, "tcps recv thread create fail");
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_tcps_init(void)
{
    env.tcps_dev_state = DEV_INIT_NULL;
    for(uint8_t i = 0; i < APP_TCPS_CONNECT_MAX; i++)
    {
        memset(&env.tcps_info[i], 0, sizeof(tcps_info_t));
        env.tcps_info[i].recv_data = rt_malloc(APP_TCPS_RECV_MAX);
        if(env.tcps_info[i].recv_data == RT_NULL)
        {
            env.tcps_dev_state = DEV_INIT_FAILED;
            app_log_msg(LOG_LVL_ERROR, true, "tcps recv data malloc failed");
            return;
        }
    }
    env.tcps_dev_state = DEV_INIT_SUCCESS;
    app_log_msg(LOG_LVL_INFO, true, "tcps init success");
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_tcpc_thread(void)
{
    ;
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_tcpc_init(void)
{
    ;
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
int app_udp_send_data(uint8_t *data, uint16_t size, struct sockaddr_in *sockaddr)
{
    if((data == NULL) || (size == 0) || (sockaddr == NULL) || (env.udp_dev_state != DEV_INIT_SUCCESS)) {
        return -1;
    }
    return sendto(env.udp_sock, data, size, 0, (struct sockaddr *)sockaddr, sizeof(struct sockaddr_in));
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_udp_thread_entry(void * parasm)
{
    int recv_size = 0;
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    while(1)
    {
        recv_size = recvfrom(env.udp_sock, local.udp_recv_data, APP_UDP_RECV_MAX, 0, (struct sockaddr *)&recv_addr, &addr_len);
        if(recv_size > 0)
        {
            app_net_env.net_size = recv_size;  //网络指示灯使用
//            sys_msg_help_info(local.udp_recv_data, recv_size);
//            sys_msg_local_info(local.udp_recv_data, recv_size);
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
static void app_udp_thread(void)
{
    if(env.udp_dev_state == DEV_INIT_SUCCESS)
    {
        env.udp_thread = rt_thread_create("udp_thread", app_udp_thread_entry, \
                RT_NULL, APP_UDP_STACK, APP_UDP_THREAD_PRO, 20);
        if(env.udp_thread != RT_NULL) {
            rt_thread_startup(env.udp_thread);
        }
        else {
            app_log_msg(LOG_LVL_ERROR, true, "udp thread create fail");
        }
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_udp_init(void)
{
    char server_ip[15];
    struct sockaddr_in server_addr;
    sys_paras_t *paras = app_paras_get();

    env.udp_dev_state = DEV_INIT_NULL;
    if((env.udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        env.udp_dev_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "udp socket error");
        return;
    }
    const int on = 1;
    if(setsockopt(env.udp_sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
    {
        env.udp_dev_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "add broadcast group failed");
        return ;
    }
    sprintf(server_ip, "%d.%d.%d.%d", paras->local_ip[0], paras->local_ip[1],paras->local_ip[2],paras->local_ip[3]);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(APP_UDP_PORT);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
    if(bind(env.udp_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        env.udp_dev_state = DEV_INIT_FAILED;
        app_log_msg(LOG_LVL_ERROR, true, "tcp server unable to bind");
        return;
    }
    env.udp_dev_state = DEV_INIT_SUCCESS;
    app_log_msg(LOG_LVL_INFO, true, "udp init success");
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_scast_thread(void)
{
    app_tcps_thread();
    app_tcpc_thread();
    app_udp_thread();
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_scast_init(void)
{
    app_tcps_init();
    app_tcpc_init();
    app_udp_init();
}












