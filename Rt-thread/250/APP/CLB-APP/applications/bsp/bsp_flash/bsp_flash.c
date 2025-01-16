/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "bsp_flash.h"

/* macro */
#define BSP_FLASH_SECTOR_BYTE_SIZE  (128 * 1024)
#define BSP_FLASH_START_ADDR        BSP_FLASH_SECTOR_11

/* function delcaration */
/**
 * @brief
 * @param
 * @return if ok return 0 else fail
 * @note
**/
uint8_t bsp_flash_read(uint8_t *data, uint32_t size)
{
	int result_size = 0;

   if(data == RT_NULL || size == 0 || size > BSP_FLASH_SECTOR_BYTE_SIZE) {
	   return 1;
    }
   result_size = stm32_flash_read(BSP_FLASH_START_ADDR, data, size);
   if(result_size != size) {
	   return 1;
	}
   return 0;
}

/**
 * @brief
 * @param
 * @return if ok return 0 else fail
 * @note
**/
uint8_t bsp_flash_write(uint8_t *data, uint32_t size)
{
	int result_size = 0;

   if(data == RT_NULL || size == 0 || size > BSP_FLASH_SECTOR_BYTE_SIZE) {
       return 1;
    }
   result_size = stm32_flash_erase(BSP_FLASH_START_ADDR, size);
   if(result_size != size) {
	   return 1;
   	}
   result_size = stm32_flash_write(BSP_FLASH_START_ADDR, data, size);
   if(result_size != size) {
	   return 1;
	}
   return 0;
}






