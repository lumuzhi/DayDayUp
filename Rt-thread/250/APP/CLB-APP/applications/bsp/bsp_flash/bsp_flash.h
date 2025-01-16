/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

/* include */
#include <stm32f4xx.h>
#include <rtthread.h>
#include "drv_flash.h"

/* macro */
/* STM32F407XGT6*/
#define BSP_FLASH_SECTOR_0      ((uint32_t)0x08000000) /* Base @ of sector 0, 16  Kbytes */
#define BSP_FLASH_SECTOR_1      ((uint32_t)0x08004000) /* Base @ of sector 1, 16  Kbytes */
#define BSP_FLASH_SECTOR_2      ((uint32_t)0x08008000) /* Base @ of sector 2, 16  Kbytes */
#define BSP_FLASH_SECTOR_3      ((uint32_t)0x0800C000) /* Base @ of sector 3, 16  Kbytes */
#define BSP_FLASH_SECTOR_4      ((uint32_t)0x08010000) /* Base @ of sector 4, 64  Kbytes */
#define BSP_FLASH_SECTOR_5      ((uint32_t)0x08020000) /* Base @ of sector 5, 128 Kbytes */
#define BSP_FLASH_SECTOR_6      ((uint32_t)0x08040000) /* Base @ of sector 6, 128 Kbytes */
#define BSP_FLASH_SECTOR_7      ((uint32_t)0x08060000) /* Base @ of sector 7, 128 Kbytes */
#define BSP_FLASH_SECTOR_8      ((uint32_t)0x08080000) /* Base @ of sector 8, 128 Kbytes */
#define BSP_FLASH_SECTOR_9      ((uint32_t)0x080A0000) /* Base @ of sector 9, 128 Kbytes */
#define BSP_FLASH_SECTOR_10     ((uint32_t)0x080C0000) /* Base @ of sector 10,128 Kbytes */
#define BSP_FLASH_SECTOR_11     ((uint32_t)0x080E0000) /* Base @ of sector 11,128 Kbytes */

/* type declaration */

/* function declaration */
uint8_t bsp_flash_read(uint8_t *data, uint32_t size);
uint8_t bsp_flash_write(uint8_t *data, uint32_t size);

#endif /*__BSP_INTERNAL_FLASH_H__*/


