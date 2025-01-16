/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

#ifndef __BSP_HW_RTC_H__
#define __BSP_HW_RTC_H__

/* include */
#include "bsp_board.h"

/* macro */
#define BSP_RTCBKUP_LENS_MAX  80       //升级标志信息最大长度为80(RTC包含20个寄存器(80字节))

/* type declaration */

/* variable declaration */

/* function declaration */
int bsp_hwrtc_bkup_write(uint32_t BackupRegister, uint32_t *data, uint8_t size);
int bsp_hwrtc_bkup_read(uint32_t BackupRegister, uint32_t *data, uint8_t size);

#endif /*__BSP_HW_RTC_H__*/

