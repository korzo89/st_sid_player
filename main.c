#include <config.h>
#include <RTT/SEGGER_RTT.h>
#include <stm32f7xx_hal.h>
#include <stm32746g_discovery.h>
#include <stm32746g_discovery_lcd.h>
#include <fatfs.h>
#include <ff.h>
#include <debug.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>

//----------------------------------------------

#define AUDIO_BUFFER_SIZE   2048

static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);

//----------------------------------------------

static void raise_error(const char *message)
{
    DBG_PRINTF("ERROR: %s\n", message);
    while (1)
    {
    }
}

//----------------------------------------------

void vApplicationTickHook(void)
{
}

void vApplicationIdleHook(void)
{
    asm volatile("wfi");
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    DBG_PRINTF("ERROR: stack overflow in %s\n", pcTaskName);
    while (1)
    {
    }
}

void vApplicationMallocFailedHook(void)
{
    raise_error("OS malloc failed\n");
}

//----------------------------------------------

static void dummy_task(void *p)
{
    int i = 0;
    while (1)
    {
        DBG_PRINTF("I'm a dummy task, %d\n", i++);
        vTaskDelay(1000);
    }
}

static void led_task(void *p)
{
    while (1)
    {
        BSP_LED_On(LED1);
        vTaskDelay(500);
        BSP_LED_Off(LED1);
        vTaskDelay(500);
    }
}

static void general_task(void *p)
{
    BSP_LED_Init(LED1);

    /* Configure the User Button in GPIO Mode */
    BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FRAME_BUFFER);

    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

    BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

    /* Clear the LCD */
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    /* Set the LCD Text Color */
    BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);

    MX_FATFS_Init();

    FATFS fs;
    FRESULT res;

    res = f_mount(&fs, SD_Path, 0);
    if (res != FR_OK)
        raise_error("f_mount failed\n");

#if 1
    DIR dir;
    res = f_opendir(&dir, "/MUSICI~1/B/BAX");
    if (res != FR_OK)
        raise_error("f_opendir failed\n");

    FILINFO info;
    while (f_readdir(&dir, &info) == FR_OK)
    {
        if (info.fname[0] == '\0')
            break;

        DBG_PRINTF("* %s\n", info.fname);
    }
#endif

    const char *file_path = "/MUSICI~1/B/BRANDIS/FADE_T~1.SID";

    FILINFO file_info;
    res = f_stat(file_path, &file_info);
    if (res != FR_OK)
        raise_error("f_stat failed\n");

    DBG_PRINTF("size %d\n", file_info.fsize);

    FIL file;
    res = f_open(&file, file_path, FA_READ);
    if (res != FR_OK)
        raise_error("f_open failed\n");

    uint8_t *buffer = malloc(file_info.fsize);
    if (res != FR_OK)
        raise_error("malloc failed\n");

    UINT read_size;
    res = f_read(&file, buffer, file_info.fsize, &read_size);
    if (read_size != file_info.fsize)
        raise_error("read_size != file_info.fsize\n");

    f_close(&file);

    extern unsigned short LoadSIDFromMemory(void *pSidData, unsigned short *load_addr, unsigned short *init_addr, unsigned short *play_addr, unsigned char *subsongs, unsigned char *startsong, unsigned char *speed, unsigned short size);

    unsigned short load_addr, init_addr, play_addr;
    unsigned char sub_songs_max, sub_song, song_speed;
    LoadSIDFromMemory(buffer, &load_addr, &init_addr, &play_addr,
            &sub_songs_max, &sub_song, &song_speed, file_info.fsize);

    xTaskCreate(dummy_task, "dummy_task", 100, NULL, 1, NULL);
    xTaskCreate(led_task, "led_task", 100, NULL, 1, NULL);

    while (1)
    {
    }
}

//----------------------------------------------

int main(void)
{
    CPU_CACHE_Enable();

    SEGGER_RTT_Init();

    HAL_Init();

    /* Configure the system clock to 200 Mhz */
    SystemClock_Config();


#if 0
    DIR dir;
    res = f_opendir(&dir, "/MUSICI~1/B/Brandis");
    if (res != FR_OK)
        raise_error("f_opendir failed\n");

    FILINFO info;
    while (f_readdir(&dir, &info) == FR_OK)
    {
        if (info.fname[0] == '\0')
            break;

        DBG_PRINTF("* %s\n", info.fname);
    }
#endif

//    if (BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 255, 44100) != AUDIO_OK)
//    {
//        SEGGER_RTT_printf(0, "BSP_AUDIO_OUT_Init failed");
//        while(1) {};
//    }
//
//    static uint16_t buffer[AUDIO_BUFFER_SIZE];
//
//    uint32_t i;
//    for (i = 0; i < AUDIO_BUFFER_SIZE; i++)
//        buffer[i] = i % 1000;
//
//    BSP_AUDIO_OUT_Play(buffer, AUDIO_BUFFER_SIZE);

    xTaskCreate(general_task, "general_task", 10000, NULL, 0, NULL);

    vTaskStartScheduler();

    while (1)
    {
    }
}

//----------------------------------------------

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* activate the OverDrive to reach the 180 Mhz Frequency */
  HAL_PWREx_ActivateOverDrive();

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

static void CPU_CACHE_Enable(void)
{
    (*(uint32_t *) 0xE000ED94) &= ~0x5;
    (*(uint32_t *) 0xE000ED98) = 0x0; //MPU->RNR
    (*(uint32_t *) 0xE000ED9C) = 0x20010000 |1<<4; //MPU->RBAR
    (*(uint32_t *) 0xE000EDA0) = 0<<28 | 3 <<24 | 0<<19 | 0<<18 | 1<<17 | 0<<16 | 0<<8 | 30<<1 | 1<<0 ; //MPU->RASE  WT
    (*(uint32_t *) 0xE000ED94) = 0x5;

    /* Invalidate I-Cache : ICIALLU register*/
    SCB_InvalidateICache();

    /* Enable branch prediction */
    SCB->CCR |= (1 <<18);
    __DSB();

    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_InvalidateDCache();
    SCB_EnableDCache();
}
