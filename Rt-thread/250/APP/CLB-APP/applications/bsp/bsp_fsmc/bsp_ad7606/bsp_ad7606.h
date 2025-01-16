/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __BSP_AD7606_H__
#define __BSP_AD7606_H__

/* include */
#include "bsp_board.h"

/* macro */

/* type declaration */

/* variable declaration */

/* function declaration */
void bsp_ad7606_gpio_init(void);
void bsp_ad7606_convst_high(void);
void bsp_ad7606_convst_low(void);
uint8_t bsp_ad7606_get_busy(void);

#endif /*__BSP_AD7606_H__*/

