//#include "bsp_adc.h"
//
///**
//* @brief ADC MSP Initialization
//* This function configures the hardware resources used in this example
//* @param hadc: ADC handle pointer
//* @retval None
//*/
//extern DMA_HandleTypeDef hdma_adc1;
//void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
//  if(hadc->Instance==ADC1)
//  {
//  /* USER CODE BEGIN ADC1_MspInit 0 */
//
//  /* USER CODE END ADC1_MspInit 0 */
//
//  /** Initializes the peripherals clock
//  */
//    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
//    PeriphClkInitStruct.PLL2.PLL2M = 5;
//    PeriphClkInitStruct.PLL2.PLL2N = 160;
//    PeriphClkInitStruct.PLL2.PLL2P = 25;
//    PeriphClkInitStruct.PLL2.PLL2Q = 2;
//    PeriphClkInitStruct.PLL2.PLL2R = 2;
//    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
////    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
//    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
//    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
//    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
//    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
//    {
//      Error_Handler();
//    }
//
////    /* ADC1 DMA Init */
////    /* ADC1 Init */
////    hdma_adc1.Instance = DMA1_Stream0;
////    hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
////    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
////    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
////    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
////    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
////    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
////    hdma_adc1.Init.Mode = DMA_CIRCULAR;
////    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
////    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
////    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
////    {
////        rt_kprintf("HAL DMA INIT failed\n");
////      Error_Handler();
////    }
////
////    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);
//
//    /* Peripheral clock enable */
//    __HAL_RCC_ADC12_CLK_ENABLE();
//
//    __HAL_RCC_GPIOA_CLK_ENABLE();
//    __HAL_RCC_GPIOB_CLK_ENABLE();
//    __HAL_RCC_GPIOF_CLK_ENABLE();
//    /**ADC1 GPIO Configuration
//    PA3     ------> ADC1_INP15
//    PA5     ------> ADC1_INP19
//    PB0     ------> ADC1_INP9
//    PF12     ------> ADC1_INP6
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5;
//    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//    GPIO_InitStruct.Pin = GPIO_PIN_0;
//    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    GPIO_InitStruct.Pin = GPIO_PIN_12;
//    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
//
//  /* USER CODE BEGIN ADC1_MspInit 1 */
//
//  /* USER CODE END ADC1_MspInit 1 */
//  }
//
//}
//
///**
//* @brief ADC MSP De-Initialization
//* This function freeze the hardware resources used in this example
//* @param hadc: ADC handle pointer
//* @retval None
//*/
//void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
//{
//  if(hadc->Instance==ADC1)
//  {
//  /* USER CODE BEGIN ADC1_MspDeInit 0 */
//
//  /* USER CODE END ADC1_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_ADC12_CLK_DISABLE();
//
//    /**ADC1 GPIO Configuration
//    PA3     ------> ADC1_INP15
//    PA5     ------> ADC1_INP19
//    PB0     ------> ADC1_INP9
//    PF12     ------> ADC1_INP6
//    */
//    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3|GPIO_PIN_5);
//
//    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);
//
//    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_12);
//
//    /* ADC1 DMA DeInit */
//    HAL_DMA_DeInit(hadc->DMA_Handle);
//  /* USER CODE BEGIN ADC1_MspDeInit 1 */
//
//  /* USER CODE END ADC1_MspDeInit 1 */
//  }
//
//}
