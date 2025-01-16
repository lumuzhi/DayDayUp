/**
 * @file
 * @details
 * @author
 * @date
 * @version
   SRAM -- Bank1  4*64M    0x60000000 - 0x6FFFFFFF

   FSMC bank1 NOR/PSRAM1   0x60000000 - 0x63FFFFFF
   FSMC bank1 NOR/PSRAM2   0x64000000 - 0x67FFFFFF
   FSMC bank1 NOR/PSRAM3   0x68000000 - 0x6BFFFFFF
   FSMC bank1 NOR/PSRAM4   0x6C000000 - 0x6FFFFFFF
**/

#ifndef __BSP_FSMC_H__
#define __BSP_FSMC_H__

/* include */
#include "bsp_board.h"

/* macro */
/*IS61WV204816BLL*/
#define BSP_SRAM_BEGIN      0x68000000
#define BSP_SRAM_SIZE       0x00400000

/* type declaration */
typedef enum
{
    BSP_FSMC_INIT_NULL,
    BSP_FSMC_INIT_OK,
    BSP_FSMC_INIT_FAIL,
}fsmc_state_e;
typedef struct
{
    SRAM_HandleTypeDef hsram;
    fsmc_state_e       sram_state;
    SRAM_HandleTypeDef ad7606;
    fsmc_state_e       ad7606_state;
}bsp_fsmc_env_t;

/* variable declaration */
extern bsp_fsmc_env_t bsp_fsmc_env;

/* function declaration */

#endif /*__BSP_FSMC_H__*/

