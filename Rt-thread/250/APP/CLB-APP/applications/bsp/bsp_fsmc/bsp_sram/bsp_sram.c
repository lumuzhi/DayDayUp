/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "bsp_sram.h"
#include "bsp_ad7606.h"
#include "bsp_fsmc.h"

/* macro */

/* variable declaration */

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note   FSMC GPIO Configuration
**/
/**
 * @brief
 * @param
 * @return
 * @note
**/
static uint32_t FSMC_Initialized = 0;
void HAL_FSMC_SRAM_MspInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    if(FSMC_Initialized) {
        return;
    }
    FSMC_Initialized = 1;

    /* Peripheral clock enable */
    __HAL_RCC_FSMC_CLK_ENABLE();

    /*  BSP_SRAM_DEV
    {
        PF0 ------> FSMC_A0   PF1 ------> FSMC_A1   PF2 ------> FSMC_A2   PF3 ------> FSMC_A3   PF4 ------> FSMC_A4   PF5 ------> FSMC_A5
        PF12------> FSMC_A6   PF13------> FSMC_A7   PF14------> FSMC_A8   PF15------> FSMC_A9   PG0 ------> FSMC_A10  PG1 ------> FSMC_A11
        PG2 ------> FSMC_A12  PG3 ------> FSMC_A13  PG4 ------> FSMC_A14  PG5 ------> FSMC_A15  PD11------> FSMC_A16  PD12------> FSMC_A17
        PD13------> FSMC_A18  PE3 ------> FSMC_A19  PE4 ------> FSMC_A20

        PD14------> FSMC_D0   PD15------> FSMC_D1   PD0 ------> FSMC_D2   PD1 ------> FSMC_D3   PE7 ------> FSMC_D4   PE8 ------> FSMC_D5
        PE9 ------> FSMC_D6   PE10------> FSMC_D7   PE11------> FSMC_D8   PE12------> FSMC_D9   PE13------> FSMC_D10  PE14------> FSMC_D11
        PE15------> FSMC_D12  PD8 ------> FSMC_D13  PD9 ------> FSMC_D14  PD10------> FSMC_D15

        PD4 ------> FSMC_NOE  PD5 ------> FSMC_NWE  PG10------> FSMC_NE3  PE0 ------> FSMC_NBL0 PE1 ------> FSMC_NBL1
    }
    */
    __HAL_RCC_GPIOE_CLK_ENABLE();        __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();        __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | \
            GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12 | \
            GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | \
            GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /*  BSP_ADC7606_DEV
    {
        PD14------> FSMC_D0   PD15------> FSMC_D1   PD0 ------> FSMC_D2   PD1 ------> FSMC_D3   PE7 ------> FSMC_D4   PE8 ------> FSMC_D5
        PE9 ------> FSMC_D6   PE10------> FSMC_D7   PE11------> FSMC_D8   PE12------> FSMC_D9   PE13------> FSMC_D10  PE14------> FSMC_D11
        PE15------> FSMC_D12  PD8 ------> FSMC_D13  PD9 ------> FSMC_D14  PD10------> FSMC_D15

        PD4 ------> FSMC_NOE  PG12------> FSMC_NE4  PD5 ------> FSMC_NWE

        PF7 -> AD7606_CONVST  PF8 --> AD7606_RESET  PF11 -> AD7606_BUSY1
    }
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_SET);
    bsp_ad7606_gpio_init();
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
static uint32_t FSMC_DeInitialized = 0;
void HAL_FSMC_SRAM_MspDeInit(void)
{
    if (FSMC_DeInitialized) {
        return;
    }
    FSMC_DeInitialized = 1;
    /* Peripheral clock enable */
    __HAL_RCC_FSMC_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12 | \
            GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_10);
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | \
            GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | \
            GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5);
}

