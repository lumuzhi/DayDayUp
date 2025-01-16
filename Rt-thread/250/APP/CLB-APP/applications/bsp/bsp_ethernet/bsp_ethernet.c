/**
 * @file
 * @details
 * @author
 * @date     如果使用组播需要将驱动参数ETH_MULTICASTFRAMESFILTER_PERFECT改为ETH_MULTICASTFRAMESFILTER_NONE
 * @version
**/

/* include */
#include "bsp_ethernet.h"

/* macro */

/* type declaration */

/* variable declaration */

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
        ETH GPIO Configuration
        PC1     ------> ETH_MDC
        PA1     ------> ETH_REF_CLK
        PA2     ------> ETH_MDIO
        PA7     ------> ETH_CRS_DV
        PC4     ------> ETH_RXD0
        PC5     ------> ETH_RXD1
        PG11    ------> ETH_TX_EN
        PG13    ------> ETH_TXD0
        PG14    ------> ETH_TXD1
        PA0     ------> ETH_RESET
**/
void HAL_ETH_MspInit(ETH_HandleTypeDef* heth)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(heth->Instance==ETH)
    {
        /* Peripheral clock enable */
        __HAL_RCC_ETH_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* heth)
{
    if(heth->Instance==ETH)
    {
        __HAL_RCC_ETH_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7);
        HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14);
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void phy_reset(void)
{
    rt_pin_mode(BSP_ETH_RESET_NUM, PIN_MODE_OUTPUT);
    rt_pin_write(BSP_ETH_RESET_NUM, PIN_HIGH);
    rt_thread_mdelay(50);
    rt_pin_write(BSP_ETH_RESET_NUM, PIN_LOW);
    rt_thread_mdelay(50);
    rt_pin_write(BSP_ETH_RESET_NUM, PIN_HIGH);
}







