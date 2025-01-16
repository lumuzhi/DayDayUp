/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __BSP_HW_TIMER_H__
#define __BSP_HW_TIMER_H__

/* include */
#include "bsp_board.h"

/* macro */

/* type declaration */

/* variable declaration */

/* function declaration */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle);
#endif /*__BSP_HW_TIMER_H__*/

