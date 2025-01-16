/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "bsp_ad7606.h"
#include "app_utility.h"

/* macro */

/* type declaration */

/* variable declaration */

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void bsp_ad7606_gpio_init(void)
{
    //复位
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);
    sys_delay(10);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);
    sys_delay(10);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void bsp_ad7606_convst_high(void)
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_SET);
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void bsp_ad7606_convst_low(void)
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
uint8_t bsp_ad7606_get_busy(void)
{
    uint8_t result = 0;
    if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_11) == GPIO_PIN_SET) {
        result = 1;
    }
    else {
        result = 0;
    }
    return result;
}






