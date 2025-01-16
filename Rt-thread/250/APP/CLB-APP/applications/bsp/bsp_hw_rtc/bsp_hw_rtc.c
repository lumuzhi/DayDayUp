/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "bsp_hw_rtc.h"

/* macro */

/* type declaration */
typedef struct
{
    RTC_HandleTypeDef *hrtc;
}bsp_hwrtc_local_t;

/* variable declaration */
bsp_hwrtc_local_t bsp_hwrtc_local =
{
    .hrtc = NULL,
};
#define local bsp_hwrtc_local

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    if(hrtc->Instance==RTC)
    {
        /* USER CODE BEGIN RTC_MspInit 0 */

        /* USER CODE END RTC_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_RTC_ENABLE();
        /* USER CODE BEGIN RTC_MspInit 1 */
        /* USER CODE END RTC_MspInit 1 */
        local.hrtc = hrtc;
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
    if(hrtc->Instance==RTC)
    {
        /* USER CODE BEGIN RTC_MspDeInit 0 */

        /* USER CODE END RTC_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_RTC_DISABLE();
        /* USER CODE BEGIN RTC_MspDeInit 1 */
        /* USER CODE END RTC_MspDeInit 1 */
        local.hrtc = hrtc;
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
int bsp_hwrtc_bkup_write(uint32_t BackupRegister, uint32_t *data, uint8_t size)
{
    if((data == NULL) || (size == 0) || (BackupRegister > RTC_BKP_DR19) || (local.hrtc == NULL))
    {
        return -1;
    }
    for(uint8_t i = 0; i < size; i++)
    {
        if((BackupRegister + i) > RTC_BKP_DR19) {
            return -1;
        }
        HAL_RTCEx_BKUPWrite(local.hrtc, BackupRegister + i, data[i]);
    }
    return 0;
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
int bsp_hwrtc_bkup_read(uint32_t BackupRegister, uint32_t *data, uint8_t size)
{
    if((data == NULL) || (size == 0) || (BackupRegister > RTC_BKP_DR19) || (local.hrtc == NULL))
    {
        return -1;
    }
    for(uint8_t i = 0; i < size; i++)
    {
        if((BackupRegister + i) > RTC_BKP_DR19) {
            return -1;
        }
        data[i] = HAL_RTCEx_BKUPRead(local.hrtc, BackupRegister + i);
    }
    return 0;
}













