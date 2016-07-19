/*
 * system.c
 *
 *  Created on: 17 lip 2016
 *      Author: Korzo
 */

#include "system.h"
#include <debug.h>
#include <assert.h>
#include <stm32f7xx_hal.h>
#include <stm32746g_discovery.h>
#include <stm32746g_discovery_sdram.h>
#include <stm32746g_discovery_lcd.h>
#include <fatfs.h>
#include <gui/gui.h>

//----------------------------------------------

struct system_ctx
{
    FATFS fs;
};

static struct system_ctx ctx;

//----------------------------------------------

static void init_system_clock(void)
{
    (DBGMCU)->APB1FZ = 0x7E01BFF;
    (DBGMCU)->APB2FZ = 0x70003;

    HAL_StatusTypeDef res;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_CRC_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 432;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;

    res = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    ASSERT_CRIT(res == HAL_OK);

    res = HAL_PWREx_EnableOverDrive();
    ASSERT_CRIT(res == HAL_OK);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    res = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
    ASSERT_CRIT(res == HAL_OK);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC | RCC_PERIPHCLK_SAI2;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 100;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
    PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
    PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
    PeriphClkInitStruct.PLLSAIDivQ = 1;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI;

    res = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
    ASSERT_CRIT(res == HAL_OK);

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

//----------------------------------------------

void system_init(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();

    DBG_INIT();

    init_system_clock();

    HAL_Init();

    BSP_SDRAM_Init();
    BSP_LED_Init(LED1);
    BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

    MX_FATFS_Init();

    FRESULT res = f_mount(&ctx.fs, SD_Path, 0);
    ASSERT_WARN(res == FR_OK);

    gui_init();
}
