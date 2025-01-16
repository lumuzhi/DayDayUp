/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_sdio.h"
#include <dfs_fs.h>
#include "app_log.h"

/* macro */

/* type declaration */

/* variable */
app_sdio_env_t app_sdio_env;
#define env app_sdio_env

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_sdio_init(void)
{
    env.dev_state = DEV_INIT_NULL;
    rt_thread_mdelay(1500);
    if(dfs_mount("sd0", "/", "elm", 0, 0) < 0)
    {
        dfs_mkfs("elm", "sd0");
        if(dfs_mount("sd0", "/", "elm", 0, 0) < 0)
        {
            env.dev_state = DEV_INIT_FAILED;
            app_log_msg(LOG_LVL_ERROR, true, "app sdio init failed");
            return ;
        }
    }
    env.dev_state = DEV_INIT_SUCCESS;
    app_log_msg(LOG_LVL_INFO, true, "app sdio init success");
}






