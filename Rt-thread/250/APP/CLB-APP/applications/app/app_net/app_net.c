/**
 * @file
 * @details
 * @author
 * @date
 * @version
            char net_infor[16];
            strcpy(net_infor, inet_ntoa(recv_addr.sin_addr));  //将接收到的IP地址转为字符串形式
**/

/* include */
#include "app_net.h"
#include "app_paras.h"
#include "app_scast.h"
#include "app_mcast.h"

/* macro */

/* type declaration */

/* variable */
app_net_env_t app_net_env;

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_net_thread(void)
{
    app_scast_thread();
    app_mcast_thread();
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static void app_net_set_paras(char* netdev_name)
{
    struct netdev *netdev = RT_NULL;
    ip_addr_t addr;
    char net_infor[16];
    sys_paras_t *sys_paras = app_paras_get();

    netdev = netdev_get_by_name(netdev_name);
    if(netdev == RT_NULL)
    {
        rt_kprintf("bad network interface device name(%s)\n", netdev_name);
        RT_ASSERT(netdev != RT_NULL);
    }
    /* set IP address */
    rt_memset(net_infor, '\0', 16);
    sprintf(net_infor, "%d.%d.%d.%d", sys_paras->local_ip[0], sys_paras->local_ip[1], sys_paras->local_ip[2], sys_paras->local_ip[3]);
    if(inet_aton(net_infor, &addr))
    {
        rt_kprintf("set network ipaddr->%s\n", net_infor);
        netdev_set_ipaddr(netdev, &addr);
    }
    /* set gateway address */
    rt_memset(net_infor, '\0', 16);
    sprintf(net_infor, "%d.%d.%d.%d", sys_paras->gateway[0], sys_paras->gateway[1], sys_paras->gateway[2], sys_paras->gateway[3]);
    if(inet_aton(net_infor, &addr))
    {
        rt_kprintf("set network gateway->%s\n", net_infor);
        netdev_set_gw(netdev, &addr);
    }
    /* set netmask address */
    rt_memset(net_infor, '\0', 16);
    sprintf(net_infor, "%d.%d.%d.%d", sys_paras->netmask[0], sys_paras->netmask[1], sys_paras->netmask[2], sys_paras->netmask[3]);
    if(inet_aton(net_infor, &addr))
    {
        rt_kprintf("set network netmask->%s\n", net_infor);
        netdev_set_netmask(netdev, &addr);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_net_init(void)
{
    app_net_env.net_size = 0;
    app_net_set_paras("e0");
    app_scast_init();
    app_mcast_init();
}


