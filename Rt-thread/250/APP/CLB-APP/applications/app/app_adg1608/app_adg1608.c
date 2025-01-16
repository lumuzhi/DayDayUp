/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_adg1608.h"

/* macro */

/* type declaration */

/* variable declaration */
app_adg1608_env_t app_adg1608_env;
#define env app_adg1608_env

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note   对应齿轮电机
**/
void app_adg1608_1_switch(adg1608_ch_e adg1608_ch)
{
    switch (adg1608_ch)
    {
        case ADG1608_CH_1:
            rt_pin_write(APP_ADG1608_1_A2_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_1_A1_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_1_A0_PIN, PIN_LOW);
            break;
        case ADG1608_CH_2:
            rt_pin_write(APP_ADG1608_1_A2_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_1_A1_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_1_A0_PIN, PIN_HIGH);
            break;
        case ADG1608_CH_3:
            rt_pin_write(APP_ADG1608_1_A2_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_1_A1_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_1_A0_PIN, PIN_LOW);
            break;
        case ADG1608_CH_4:
            rt_pin_write(APP_ADG1608_1_A2_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_1_A1_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_1_A0_PIN, PIN_HIGH);
            break;
        case ADG1608_CH_5:
            rt_pin_write(APP_ADG1608_1_A2_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_1_A1_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_1_A0_PIN, PIN_LOW);
            break;
        case ADG1608_CH_6:
            rt_pin_write(APP_ADG1608_1_A2_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_1_A1_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_1_A0_PIN, PIN_HIGH);
            break;
        case ADG1608_CH_7:
            rt_pin_write(APP_ADG1608_1_A2_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_1_A1_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_1_A0_PIN, PIN_LOW);
            break;
        case ADG1608_CH_8:
            rt_pin_write(APP_ADG1608_1_A2_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_1_A1_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_1_A0_PIN, PIN_HIGH);
            break;
        default:
            break;
    }
}

/**
 * @brief
 * @param
 * @return
 * @note   对应轴端
**/
void app_adg1608_2_switch(adg1608_ch_e adg1608_ch)
{
    switch (adg1608_ch)
    {
        case ADG1608_CH_1:
            rt_pin_write(APP_ADG1608_2_A2_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_2_A1_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_2_A0_PIN, PIN_LOW);
            break;
        case ADG1608_CH_2:
            rt_pin_write(APP_ADG1608_2_A2_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_2_A1_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_2_A0_PIN, PIN_HIGH);
            break;
        case ADG1608_CH_3:
            rt_pin_write(APP_ADG1608_2_A2_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_2_A1_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_2_A0_PIN, PIN_LOW);
            break;
        case ADG1608_CH_4:
            rt_pin_write(APP_ADG1608_2_A2_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_2_A1_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_2_A0_PIN, PIN_HIGH);
            break;
        case ADG1608_CH_5:
            rt_pin_write(APP_ADG1608_2_A2_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_2_A1_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_2_A0_PIN, PIN_LOW);
            break;
        case ADG1608_CH_6:
            rt_pin_write(APP_ADG1608_2_A2_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_2_A1_PIN, PIN_LOW);
            rt_pin_write(APP_ADG1608_2_A0_PIN, PIN_HIGH);
            break;
        case ADG1608_CH_7:
            rt_pin_write(APP_ADG1608_2_A2_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_2_A1_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_2_A0_PIN, PIN_LOW);
            break;
        case ADG1608_CH_8:
            rt_pin_write(APP_ADG1608_2_A2_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_2_A1_PIN, PIN_HIGH);
            rt_pin_write(APP_ADG1608_2_A0_PIN, PIN_HIGH);
            break;
        default:
            break;
    }
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_adg1608_init(void)
{
    rt_pin_mode(APP_ADG1608_1_EN_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(APP_ADG1608_1_EN_PIN, PIN_HIGH);
    rt_pin_mode(APP_ADG1608_1_A0_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(APP_ADG1608_1_A1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(APP_ADG1608_1_A2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(APP_ADG1608_2_EN_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(APP_ADG1608_2_EN_PIN, PIN_HIGH);
    rt_pin_mode(APP_ADG1608_2_A0_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(APP_ADG1608_2_A1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(APP_ADG1608_2_A2_PIN, PIN_MODE_OUTPUT);
    app_adg1608_1_switch(ADG1608_CH_1);
    app_adg1608_2_switch(ADG1608_CH_1);
    env.current_ch = ADG1608_CH_1;
}

