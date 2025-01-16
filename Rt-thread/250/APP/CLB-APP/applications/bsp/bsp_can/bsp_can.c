/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "bsp_can.h"

/* macro */

/* type declaration */

/* variable declaration */
static uint32_t HAL_RCC_CAN1_CLK_ENABLED=0;
static uint32_t HAL_RCC_CAN2_CLK_ENABLED=0;

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(hcan->Instance==CAN1)
    {
        /* USER CODE BEGIN CAN1_MspInit 0 */

        /* USER CODE END CAN1_MspInit 0 */
        /* Peripheral clock enable */
        HAL_RCC_CAN1_CLK_ENABLED++;
        if(HAL_RCC_CAN1_CLK_ENABLED==1){
            __HAL_RCC_CAN1_CLK_ENABLE();
        }
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**CAN1 GPIO Configuration
        PA11     ------> CAN1_RX
        PA12     ------> CAN1_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USER CODE BEGIN CAN1_MspInit 1 */
        /* USER CODE END CAN1_MspInit 1 */
    }
    else if(hcan->Instance==CAN2)
    {
        /* USER CODE BEGIN CAN2_MspInit 0 */

        /* USER CODE END CAN2_MspInit 0 */
        /* Peripheral clock enable */
        HAL_RCC_CAN2_CLK_ENABLED++;
        if(HAL_RCC_CAN2_CLK_ENABLED==1){
            __HAL_RCC_CAN2_CLK_ENABLE();
        }

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**CAN2 GPIO Configuration
        PB12     ------> CAN2_RX
        PB13     ------> CAN2_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN CAN2_MspInit 1 */
        /* USER CODE END CAN2_MspInit 1 */
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
    if(hcan->Instance==CAN1)
    {
        /* USER CODE BEGIN CAN1_MspDeInit 0 */

        /* USER CODE END CAN1_MspDeInit 0 */
        /* Peripheral clock disable */
        HAL_RCC_CAN1_CLK_ENABLED--;
        if(HAL_RCC_CAN1_CLK_ENABLED==0){
            __HAL_RCC_CAN1_CLK_DISABLE();
        }
        /**CAN1 GPIO Configuration
        PA11     ------> CAN1_RX
        PA12     ------> CAN1_TX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

        /* USER CODE BEGIN CAN1_MspDeInit 1 */
        /* USER CODE END CAN1_MspDeInit 1 */
    }
    else if(hcan->Instance==CAN2)
    {
        /* USER CODE BEGIN CAN2_MspDeInit 0 */

        /* USER CODE END CAN2_MspDeInit 0 */
        /* Peripheral clock disable */
        HAL_RCC_CAN2_CLK_ENABLED--;
        if(HAL_RCC_CAN2_CLK_ENABLED==0){
            __HAL_RCC_CAN2_CLK_DISABLE();
        }
        /**CAN2 GPIO Configuration
        PB12     ------> CAN2_RX
        PB13     ------> CAN2_TX
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);

        /* USER CODE BEGIN CAN2_MspDeInit 1 */
        /* USER CODE END CAN2_MspDeInit 1 */
    }
}








