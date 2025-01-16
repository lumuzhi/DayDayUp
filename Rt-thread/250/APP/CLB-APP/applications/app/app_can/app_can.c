/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_can.h"
#include "app_can1.h"
#include "app_can2.h"

/* macro */

/* type declaration */

/* variable declaration */
app_can_env_t app_can_env;

/* function declaration */


/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_can_notify_qzq_gather_temp(uint8_t flag)
{
    uint8_t send_con[3] = {0};
    send_con[0] = 0xAF;
    send_con[1] = 0xBF;
    send_con[2] = flag;
    /*  发送3次，确保成功  */
    for(uint8_t i = 0; i < 2; i++) {
        app_can1_send(QZQ1_BOARD, send_con, 3);
        app_can1_send(QZQ2_BOARD, send_con, 3);
        app_can1_send(QZQ3_BOARD, send_con, 3);
        app_can1_send(QZQ4_BOARD, send_con, 3);

        app_can2_send(QZQ1_BOARD, send_con, 3);
        app_can2_send(QZQ2_BOARD, send_con, 3);
        app_can2_send(QZQ3_BOARD, send_con, 3);
        app_can2_send(QZQ4_BOARD, send_con, 3);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_can_thread(void)
{
    app_can1_thread();
    app_can2_thread();
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_can_init(void)
{
    app_can1_init();
    app_can2_init();
}







