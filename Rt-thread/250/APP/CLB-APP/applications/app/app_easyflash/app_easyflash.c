/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_easyflash.h"
#include "easyflash.h"
#include "spi_flash_sfud.h"
#include "drv_spi.h"
#include <fal.h>
#include <dfs_fs.h>

/* macro */

/* type declaration */

/* variable */
app_easyflash_env_t app_easyflash_env;
#define env app_easyflash_env

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_easyflash_init(void)
{
    env.dev_state = DEV_INIT_NULL;
    rt_hw_spi_device_attach("spi1", "spi1_0", GPIOA, GPIO_PIN_4);
    if(rt_sfud_flash_probe("W25Q256", "spi1_0") == RT_NULL)
    {
        env.dev_state = DEV_INIT_FAILED;
        rt_kprintf("rt sfud flash probe fail ! \n");
        return ;
    }
    fal_init();
    if(easyflash_init() != EF_NO_ERR)
    {
        env.dev_state = DEV_INIT_FAILED;
        rt_kprintf("easyflash init failed \n");
        return ;
    }
    env.dev_state = DEV_INIT_SUCCESS;
    rt_kprintf("easyflash init success \n");
}






