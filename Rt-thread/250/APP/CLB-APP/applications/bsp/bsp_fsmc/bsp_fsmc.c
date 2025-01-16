/**
 * @file
 * @details
 * @author
 * @date
 * @version
**/
/* include */
#include "bsp_fsmc.h"
#include "bsp_sram.h"
#include "bsp_ad7606.h"

/* macro */
#define bsp_sram_write(addr, data) (*(volatile uint16_t *)(addr) = data)
#define bsp_sram_read(addr)        (*(volatile uint16_t *)(addr))

/* variable declaration */
#ifdef RT_USING_MEMHEAP_AS_HEAP
static struct rt_memheap system_heap;
#endif
bsp_fsmc_env_t bsp_fsmc_env;

/* function declaration */
/**
 * @brief
 * @param
 * @return
 * @note
**/
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    HAL_FSMC_SRAM_MspInit();
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef* hsram)
{
    HAL_FSMC_SRAM_MspDeInit();
}

/**
 * @brief
 * @param
 * @return
 * @note
**/
uint8_t bsp_sram_test(void)
{
    uint8_t result;
    uint16_t ram_data = 0x8243, ram_read = 0;
    uint32_t error_cnts = 0;

    for(rt_uint32_t i = 0; i < BSP_SRAM_SIZE / 4; i++)
    {
        bsp_sram_write(BSP_SRAM_BEGIN + i, ram_data);
        ram_read = bsp_sram_read(BSP_SRAM_BEGIN + i);
        if(ram_data != ram_read) {
            error_cnts++;
        }
        ram_data++;
    }
    if(error_cnts)
    {
        result = 0;
        rt_kprintf("bsp sram test fail ! \n");
    }
    else
    {
        result = 1;
        rt_kprintf("bsp sram test success ! \n");
    }
    return result;
}

/**
 * @brief
 * @param
 * @return
 * @note   外部RAM/AD7606 在访问NORSRAM时，会自动将NE片选信号设置为低电平，
                      所以在进行SRAM测试的时候要件AD7606的NE引脚设置高电平，避免对SRAM的干扰
**/
int bsp_fsmc_sram_init(void)
{
    FSMC_NORSRAM_TimingTypeDef Timing = { 0 };

    bsp_fsmc_env.sram_state = BSP_FSMC_INIT_NULL;
    bsp_fsmc_env.hsram.Instance = FSMC_NORSRAM_DEVICE;
    bsp_fsmc_env.hsram.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
    /* hsram.Init */
    bsp_fsmc_env.hsram.Init.NSBank = FSMC_NORSRAM_BANK3;
    bsp_fsmc_env.hsram.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
    bsp_fsmc_env.hsram.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
    bsp_fsmc_env.hsram.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    bsp_fsmc_env.hsram.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    bsp_fsmc_env.hsram.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    bsp_fsmc_env.hsram.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
    bsp_fsmc_env.hsram.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    bsp_fsmc_env.hsram.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    bsp_fsmc_env.hsram.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    bsp_fsmc_env.hsram.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
    bsp_fsmc_env.hsram.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    bsp_fsmc_env.hsram.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
    bsp_fsmc_env.hsram.Init.PageSize = FSMC_PAGE_SIZE_NONE;

    /* Timing */
    Timing.AddressSetupTime = 4;
    Timing.AddressHoldTime = 0;
    Timing.DataSetupTime = 5;
    Timing.BusTurnAroundDuration = 0;
    Timing.CLKDivision = 0;
    Timing.DataLatency = 0;
    Timing.AccessMode = FSMC_ACCESS_MODE_A;
    /* ExtTiming */

    if(HAL_SRAM_Init(&bsp_fsmc_env.hsram, &Timing, &Timing) != HAL_OK)
    {
        rt_kprintf("bsp sram init failed !\n");
        bsp_fsmc_env.sram_state = BSP_FSMC_INIT_FAIL;
    }
    else if(bsp_sram_test())
    {
        rt_kprintf("bsp sram init success !\n");
        bsp_fsmc_env.sram_state = BSP_FSMC_INIT_OK;
#ifdef RT_USING_MEMHEAP_AS_HEAP
        rt_memheap_init(&system_heap, "sram", (void*)BSP_SRAM_BEGIN, BSP_SRAM_SIZE);
#endif
    }
    else
    {
        rt_kprintf("bsp sram init failed !\n");
        bsp_fsmc_env.sram_state = BSP_FSMC_INIT_FAIL;
    }

    return RT_EOK;
}
INIT_BOARD_EXPORT(bsp_fsmc_sram_init);

/**
 * @brief
 * @param
 * @return
 * @note
**/
int bsp_fsmc_ad7606_init(void)
{
    FSMC_NORSRAM_TimingTypeDef Timing = { 0 };

    bsp_fsmc_env.ad7606_state = BSP_FSMC_INIT_NULL;
    bsp_fsmc_env.ad7606.Instance = FSMC_NORSRAM_DEVICE;
    bsp_fsmc_env.ad7606.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
    /* hsram.Init */
    bsp_fsmc_env.ad7606.Init.NSBank = FSMC_NORSRAM_BANK4;
    bsp_fsmc_env.ad7606.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
    bsp_fsmc_env.ad7606.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
    bsp_fsmc_env.ad7606.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    bsp_fsmc_env.ad7606.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    bsp_fsmc_env.ad7606.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    bsp_fsmc_env.ad7606.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
    bsp_fsmc_env.ad7606.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    bsp_fsmc_env.ad7606.Init.WriteOperation = FSMC_WRITE_OPERATION_DISABLE;
    bsp_fsmc_env.ad7606.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    bsp_fsmc_env.ad7606.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
    bsp_fsmc_env.ad7606.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    bsp_fsmc_env.ad7606.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
    bsp_fsmc_env.ad7606.Init.PageSize = FSMC_PAGE_SIZE_NONE;

    /* Timing */
    Timing.AddressSetupTime = 5;
    Timing.AddressHoldTime = 0;
    Timing.DataSetupTime = 7;
    Timing.BusTurnAroundDuration = 1;
    Timing.CLKDivision = 0;
    Timing.DataLatency = 0;
    Timing.AccessMode = FSMC_ACCESS_MODE_A;
    /* ExtTiming */

    if(HAL_SRAM_Init(&bsp_fsmc_env.ad7606, &Timing, &Timing) != HAL_OK)
    {
        rt_kprintf("bsp ad7606 init failed !\n");
        bsp_fsmc_env.ad7606_state = BSP_FSMC_INIT_FAIL;
    }
    else {
        bsp_fsmc_env.ad7606_state = BSP_FSMC_INIT_OK;
    }
    return RT_EOK;
}
INIT_BOARD_EXPORT(bsp_fsmc_ad7606_init);



