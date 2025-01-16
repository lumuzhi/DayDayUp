/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __BSP_ETHERNET_H__
#define __BSP_ETHERNET_H__

/* include */
#include "bsp_board.h"

/* macro */
#define BSP_ETH_RESET_NUM       GET_PIN(A, 0)           /* PA0 */

/* type declaration */

/* variable declaration */

/* function declaration */
void HAL_ETH_MspInit(ETH_HandleTypeDef* heth);
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* heth);
void phy_reset(void);

#endif /*__BSP_ETHERNET_H__*/

