#include <stdio.h>      // 标准输入输出函数
#include <stdlib.h>     // 标准库函数，如malloc, free等
#include <unistd.h>     // POSIX操作系统API，如close, read, write等
#include <sys/types.h>  // 数据类型定义，如socket, bind, accept等
#include <sys/socket.h> // 套接字函数及数据结构定义，如socket, bind, accept等
#include <netinet/in.h> // Internet地址族定义，如sockaddr_in等
#include <arpa/inet.h>  // 定义IP地址转换函数，如inet_pton, inet_ntop等
#include <sys/select.h> // select函数及相关数据结构定义
#include <fcntl.h>      // 文件控制定义，如fcntl函数
#include <errno.h>      // 错误号定义，如errno
#include <pthread.h>    // 线程函数定义，如pthread_create, pthread_join等
#include <string.h>     // 字符串处理函数定义，如memset, memcpy等

#define MAX_CLIENT  10
#define LISTEM_PORT 12345

typedef struct {
    int         maxfd;  // 最大文件描述符
    pthread_t   tid;    // 线程ID
    fd_set read_set;    // 读文件描述符集合
    fd_set write_set;   // 写文件描述符集合
    fd_set ready_read_set;  // 就绪读文件描述符集合
    fd_set ready_write_set; // 就绪写文件描述符集合
    int nready; // 就绪文件描述符数量
    int clientfd[MAX_CLIENT]; // 客户端套接字
    int nclient;    // 当前客户端数量
} app_select_local_t;
app_select_local_t app_select_local;
#define local app_select_local


typedef struct {
    int fd; // 文件描述符
    int events; // 事件类型
    void *arg; // 回调函数参数
    void (*call_back)(int fd, int events, void *arg); // 回调函数
} event_t;  // 事件结构体

void *app_select_entry(void *arg)
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket error");
        return ;
    }
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(LISTEM_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind error");
        return;
    }

    if (listen(listenfd, 5) < 0) {
        perror("listen error");
        return;
    }

    local.nclient = 0;
    int sd;
    while (1) {
        // 清空文件描述符集合并添加监听套接字
        FD_ZERO(&local.read_set);
        FD_ZERO(&local.write_set);
        FD_SET(listenfd, &local.read_set);

        local.maxfd = listenfd;
        // 把所有客户端套接字加入到文件描述符集合中
        for(uint8_t i = 0; i < MAX_CLIENT; i++) {
            if(local.clientfd[i]) FD_SET(local.clientfd[i], &local.read_set);
            if(local.clientfd[i] > local.maxfd) local.maxfd = local.clientfd[i];
        }

        // 设置超时时间（可选）
        struct timeval timeout = {1, 0};  // 1 秒超时

        int active = select(local.maxfd + 1, &local.read_set, &local.write_set, NULL, &timeout);
        if (active < 0) {
            perror("select error");
            return;
        } else if (active == 0) {
            // 指定超时时间内没有文件描述符就绪
            continue;
        } else {
            // 有文件描述符就绪
            if (FD_ISSET(listenfd, &local.read_set)) {
                // 有新的连接请求
                int connfd = accept(listenfd, NULL, NULL);
                if (connfd < 0) {
                    perror("accept error");
                }
                // 将新的连接套接字添加到文件描述符集合中
                for(uint8_t i = 0; i < MAX_CLIENT; i++) {
                    if (local.clientfd[i] == 0) {
                        local.clientfd[i] = connfd;
                        break;
                    }
                }
            }
            // 遍历所有的客户端套接字
            for(uint8_t i = 0; i < MAX_CLIENT; i++) {
                if (FD_ISSET(local.clientfd[i], &local.read_set)) {
                    // 有客户端套接字有数据可读
                    char buf[1024];
                    int ret = read(local.clientfd[i], buf, sizeof(buf));
                    if (ret < 0) {
                        /*
                        * 1. EAGAIN: 读缓冲区没有数据可读
                        * 2. EINTR: 读操作被信号中断
                        * 3. ECONNRESET: 连接被重置
                        * 4. EPIPE: 写端关闭
                        * 5. EWOULDBLOCK: 读缓冲区没有数据可读
                        * 6. EIO: I/O错误
                        * 7. EBADF: 无效的文件描述符
                        * 8. EINVAL: 无效的参数
                        * 9. EFAULT: 内存访问错误
                        * 10. EISDIR: 是一个目录
                        * 11. ENXIO: 没有这样的设备或地址
                        * 12. ECONNABORTED: 连接被终止
                        * 13. ECONNREFUSED: 连接被拒绝
                        * 14. ECONNRESET: 连接被重置
                        * 15. ENOTCONN: 连接没有建立
                        * 16. ETIMEDOUT: 连接超时
                        * 17. EPIPE: 写端关闭
                        * 18. EINTR: 被信号中断
                        */
                        perror("read error");
                        // 根据错误码处理
                    } else if (ret == 0) {
                        // 客户端关闭连接
                        close(local.clientfd[i]);
                        local.clientfd[i] = 0;
                    } else {
                        // 处理客户端数据
                    }
                }
            }
        }
    }
    close(listenfd);
}
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_select_thread(void)
{
	if(pthread_create(&local.tid, NULL, (void *)&app_select_entry, NULL) != 0) {
		perror("app select pthread_create");
	}
}