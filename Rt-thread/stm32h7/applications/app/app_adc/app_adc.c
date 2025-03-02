/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/

/* include */
#include "app_adc.h"

/* macro */
#define ADC_DEV_NAME        "adc1"      /* ADC 设备名称 */
#define ADC_DEV_CH_8        8
#define ADC_DEV_CH_9        9
#define ADC_DEV_CH_14       14
#define REFER_VOLTAGE       330         /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS        (1 << 12)   /* 转换位数为12位 */

/* type declaration */

/* variable declaration */

app_adc_env_t app_adc_env;
#define env app_adc_env
__IO uint16_t ADC1_RES[4];


ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
float_t adc_f[4];


//__attribute__((section(".ADC1_Area"))) uint16_t ADC1_RES[4] = {0,};


/* function declaration */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */



  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_16B;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 4;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
//  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
  /** Configure the ADC multi-mode
  */
//  multimode.Mode = ADC_MODE_INDEPENDENT;
//  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
//  {
//    Error_Handler();
//  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_19;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  ADC_Enable(&hadc1);

  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1_RES, 4);


  /* USER CODE END ADC1_Init 2 */

}
/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);


  /* ADC1 DMA Init */
  /* ADC1 Init */
  hdma_adc1.Instance = DMA1_Stream0;
  hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_adc1.Init.Mode = DMA_CIRCULAR;
  hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
  hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
  {
      rt_kprintf("HAL DMA INIT failed\n");
    Error_Handler();
  }

  __HAL_LINKDMA(&hadc1,DMA_Handle,hdma_adc1);

}
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_adc_thread_entry(void *paras)
{
    while(1) {
//        SCB_InvalidateDCache_by_Addr((uint32_t *)ADC1_RES, sizeof(ADC1_RES));

        adc_f[0] = ADC1_RES[0] * 3.3f / 65535;
        adc_f[1] = ADC1_RES[1] * 3.3f / 65535;
        adc_f[2] = ADC1_RES[2] * 3.3f / 65535;
        adc_f[3] = ADC1_RES[3] * 3.3f / 65535;

        rt_kprintf("%f  %f  %f  %f\n", adc_f[0], adc_f[1], adc_f[2], adc_f[3]);

        rt_thread_mdelay(500);
    }
}
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_adc_thread(void)
{
    if(env.dev_state == DEV_INIT_SUCCESS) {
        env.thread = rt_thread_create("app adc", app_adc_thread_entry, RT_NULL, \
                APP_ADC_STACK, APP_ADC_THREAD_PRO, 20);
        if(env.thread == RT_NULL) {
            rt_kprintf("app adc thrad create failed\n");
            return ;
        }
        rt_thread_startup(env.thread);
    }
}
/**
 * @brief
 * @param
 * @return
 * @note
**/
void app_adc_init(void)
{
    env.dev_state = DEV_INIT_NULL;

    MX_DMA_Init();
    MX_ADC1_Init();

    HAL_ADC_Start(&hadc1);
    env.dev_state = DEV_INIT_SUCCESS;

}


