//
//#include "bsp_fmc.h"
///**
// * @brief
// * @param
// * @return
// * @note   外部RAM/AD7606 在访问NORSRAM时，会自动将NE片选信号设置为低电平，
//                      所以在进行SRAM测试的时候要件AD7606的NE引脚设置高电平，避免对SRAM的干扰
//**/
//int bsp_fsmc_sram_init(void)
//{
//
//
//    /* USER CODE BEGIN FMC_Init 0 */
//
//    /* USER CODE END FMC_Init 0 */
//
//    FMC_SDRAM_TimingTypeDef SdramTiming = {0};
//
//    /* USER CODE BEGIN FMC_Init 1 */
//
//    /* USER CODE END FMC_Init 1 */
//
//    /** Perform the SDRAM2 memory initialization sequence
//    */
//    hsdram2.Instance = FMC_SDRAM_DEVICE;
//    /* hsdram2.Init */
//    hsdram2.Init.SDBank = FMC_SDRAM_BANK1;
//    hsdram2.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
//    hsdram2.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
//    hsdram2.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
//    hsdram2.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
//    hsdram2.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_1;
//    hsdram2.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
//    hsdram2.Init.SDClockPeriod = FMC_SDRAM_CLOCK_DISABLE;
//    hsdram2.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
//    hsdram2.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
//    /* SdramTiming */
//    SdramTiming.LoadToActiveDelay = 16;
//    SdramTiming.ExitSelfRefreshDelay = 16;
//    SdramTiming.SelfRefreshTime = 16;
//    SdramTiming.RowCycleDelay = 16;
//    SdramTiming.WriteRecoveryTime = 16;
//    SdramTiming.RPDelay = 16;
//    SdramTiming.RCDDelay = 16;
//
//    if (HAL_SDRAM_Init(&hsdram2, &SdramTiming) != HAL_OK)
//    {
//      Error_Handler( );
//    }
//
//    HAL_SetFMCMemorySwappingConfig(FMC_SWAPBMAP_SDRAMB2);
//
//    /* USER CODE BEGIN FMC_Init 2 */
//
//    /* USER CODE END FMC_Init 2 */
//
//    return RT_EOK;
//}
//INIT_BOARD_EXPORT(bsp_fsmc_sram_init);
